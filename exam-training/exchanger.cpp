/*
La classe generica Exchanger<T> permette a due thread di scambiarsi un valore di tipo T.
Essa offre il metodo T exchange(T t) che blocca il thread corrente fino a che un altro thread non invoca lo stesso metodo sulla stessa istanza.
Quando questo avviene, il metodo restituisce l'oggetto passato come parametro dal thread opposto.
Si implementi la classe C++, usando la libreria standard C++11.
*/


template <class T>
class Exchanger {
private:
	T value;
	condition_variable<mutex> exchangeDone;
	int prec = 0;
	mutex m_prec;
public:
	Exchanger(){};

	T exchange(T t){

		m_prec.lock();
		if(prec = 0){
			//first thread, set value and wait
			unique_lock<mutex> ul(m);
			value = T;
			prec++;
			m_prec.unlock();
			exchangeDone.wait(ul);
			//exchange done, return the new value
			return value;
		} else {
			//second thread, get the other value, set his value and notify exchange done
			lock_guard(m_prec);
			T otherValue = value;
			value = t;
			prec--;
			exchangeDone.notify_one();
			return otherValue;
		}
	}
}
