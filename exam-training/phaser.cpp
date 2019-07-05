/*
Tema: La classe Phaser ha i metodi "void attendi()", "void aggiungi()" e "void rimuovi()".
La classe permette a N-1 thread di attendersi tra loro, ma quando si arriva a N-1 i thread si sbloccano.
I metodi aggiungi e rimuovi modificano il valore di N (specificato nella costruzione dell'oggetto) e di conseguenza sbloccano i thread se necessario.
*/

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;


class Phaser {

private:
	int N;
	int numThread;
	mutex m;
	condition_variable cv;

public:
	Phaser(int n):N(n),numThread(0){};
	~Phaser();

	void attendi(){
		unique_lock<mutex> ul(m);
		numThread++;
		if(numThread < N-1){
			cv.wait(ul, [this]()->bool{numThread>=N-1;}); //condizione riferificata nella lambda, posso notificare sempre la cv
		}
	}

	void aggiungi(int n){
		lock_guard<mutex> lg(m);
		N += n;
		cv.notify_all();
	}

	void rimuovi(int n){
		lock_guard<mutex> lg(m);
		N-=n;
		cv.notify_all();
	}

};


int main(int argc, char** argv){

	Phaser p(5);
	vector<thread> ts = new vector<thread>();

	for(int i=0; i<10; i++){
		ts.push_back(move(thread( [&p,i]()->{p.attendi(); p.aggiungi(i); p.rimuovi(i);} )));
	}

	for(int i=0; i<10; i++){
		ts[i].join();
	}

	delete[] ts;
}


