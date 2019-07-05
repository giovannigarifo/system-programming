/*
Tema: Una classe concorrente gestisce la classifica di un gioco online: ogni concorrente è identificato da un nickname e ha un punteggio associato. 
Un metodo ritorna una copia della classifica, un altro inserisce un nuovo punteggio relativo a un concorrente (inserendo un nuovo concorrente in 
classifica se non esistente o aggiornando il punteggio se maggiore di quello vecchio), un metodo si mette in attesa di variazioni sulla classifica.
*/

#import <map>
#import <mutex>
#import <condition_variable>

using namespace std;



class Classifica {

private:
	map<string,int>* nomePunteggio;
	mutex proteggiMappa;
	condition_variable attendiModifiche;
public:
    Classifica(){ nomePunteggio = new map<string,int>(); }
    ~Classifica(){ delete[] nomePunteggio;}
    
	void inserisciPunteggio(string nome, int punteggio){
		lock_guard(proteggiMappa);

		pair<string, int> concorrente;
		if((concorrente = nomePunteggio->find(nome)) != nomePunteggio->end()){
			//aggiorna punteggio del concorrente
			if((*concorrente)->second < punteggio){
				(*concorrente)->second = punteggio;
				attendiModifiche.notify_all();
			}
		} else {
			//inserisci concorrente in classifica
			nomePunteggio->emplace(nome, punteggio);
			attendiModifiche.notify_all();
		}
	}

	void attendiVariazioni(){

		unique_lock<mutex> ul(m);
		attendiModifiche.wait(ul);
		cout << "classifica è variata." << endl;
	}

	map<string,int> ritornaCopiaClassifica(){

		lock_guard(proteggimappa);
		auto map(*NomePunteggio); //copy constructor
		return move(map);
	}

}
