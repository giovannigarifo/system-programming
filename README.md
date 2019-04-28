# Programmazione di Sistema

Link utili:

* [Compilazione condizionale con direttive al preprocessor (#ifdef...)](http://www.fe.infn.it/u/spizzo/prog09/lezioni04/direttive_def.html)


# 1. Piattaforme di esecuzione

Un'applicazione, fatto salvo nei sistemi elementare, è eseguita nel contesto di un sistema operativo: esso offre i servizi, le funzionalità e le convenzioni che permettono il corretto funzionamento dell'applicazione.

Per sfruttare tali servizi, l'applicazione deve conformarsi alle specifiche del SO: sia a livello di codice sorgente, che a livello di codice eseguibile: anche il compilatore deve essere configurato in modo tale da produrre codice compliant con il SO di destinazione.

* **API**: definisce un set di funzioni e di strutture dati offerte al programmatore per interfacciarsi con i servizi offerti dal sistema sottostante. Le API low level offerte dal SO sono annegate in funzioni di libreria (i.e. syscall `open()` annegata in funzione della C stdlin `fopen()`). Questo è necessario dato che l'implementazione delal syscall dipende dall'architettura hardware.

* **ABI**: definisce il formato che deve avere un SW per essere compatibile con il SO: convezioni da utilzzare (i.e. accesso a strutture dati del sistema attraverso Handle/FileDescriptor), uso dei registri, innalzamento di privilegio, ecc. E' supportata dagli strumenti che compongolo la toolchain.

## Gestione degli errori

Le funzioni delle API possono avere successo o fallire, occorre gestirne gli errori: bisogna verificare il tipo di ritorno, e se si è verificato un errore, utilizzare una specifica funzione per ottenere il codice dell'errore (in windows la funzione `GetLastError()`, in linux la pseudo-variabile globale `errno`).


## Character Sets

### Windows

due tipi base: 

* `char`, codifica ASCII estesa (256 caratteri) su 8bit
* `wchar_t`, codifica Unicode UTF-16, ogni carattere composto da una o due parole da 16 bit.

Tutte le API del sistema che prevedono l'uso di caratteri e stringhe sono offerte in due versioni, anche se viene utilizzata solo la versione generica, definita in fase di compilazione con la direttiva al preprocessore `#ifdef` (compilazione condizionale) in base al fatto che la macro `UNICODE` sia stata definita o meno, viene definita la versione generica che corrisponde alla versione che utilizza codifica unicode o ascii.

```
BOOL CreateDirectoryA(
	  LPCSTR lpPathName,
	  LPSECURITY_ATTRIBUTES lpSecAtt);
BOOL CreateDirectoryW(
	  LPCWSTR lpPathName,
	  LPSECURITY_ATTRIBUTES lpSecAtt);

#ifdef UNICODE
#define CreateDirectory CreateDirectoryW
#else
#define CreateDirectory CreateDirectoryA
#endif // !UNICODE
```

### Linux

GCC codifica i caratteri non ASCII con la codifica UTF-8, all'interno della stessa stringa, che è comunque terminata da `\0`, questo può creare alcuni problemi con funzioni che utilizzano l'associazione caratteri=byte come `strln()`. utilizzare invece la funzione `mbtowcs()`

In linux il tipo `wchar_t` esiste ma ha lunghezza di 4B e può ospitare qualsiasi carattere UNICODE.

```
wchar_t* s= L"Ω€®™åß∂ƒ∞∆ªøπº¬∑∫µ" //definizione preceduta da "L"
```


## Lo standard POSIX: Portable Operating System Interface

Definisce un modello concettuale, un insieme di API a basso livello e un set di comandi shell volti a rendere portabili gli applicativi tra i diversi sistemi operativi minimizzandone le differenze.


Le assunzioni principali riguardano
* La struttura del file system
* L'uso di handle per fare riferimento all'interno di un programma ai descrittori dei file
* L'uso di un sottoinsieme di caratteri portabili
* La gestione degli errori
* L'aritmetica e la rappresentazione dei numeri
* L'utilizzo di thread e processi


# 2. Il modello di esecuzione


# 4. Introduzione al C++

https://cppinsights.io/

Principi del C++: 

* "zero cost abstraction". Ogni astrazione è implementata senza penalizzare le prestazioni.

* espessività: i tipi/classi definiti dal'utente (UDF) devono possedere lo stesso livello di espressività offerto dai tipi base del linguaggio

* Sostituzione: è possibilire unsare un UDT ovunque sia possibile usare un tipo base

Paradigmi supportati:

* programmazione ad oggetti
* programmazione strutturata
* programmazione generica
* programmazione funzionale

Libreria standard limitata a sole 150 classi implementate come generiche.

Non esiste il garbage collector.

Innovazioni nel C++ moderno (ultimo standard C++20):

* Smart pointer: shared, unique, weak pointers
* contenitori generici dotati di algoritmi standard, i.e. Vector, std::array
* Migliore gestione delle eccezzioni
* Funzioni lambda
* Multithreading e sincronizzazione attraverso costrutti portabili tra SO
* RValue

Nuovi tipi base:

* bool

Nuovi tipi derivati:

* enum
```
enum colors_t {black, blue, green, cyan, red, purple, yellow, white};
```

* Array: `int x[10]`

* Puntatori nativi: `int* ptr`

* Riferimenti:

```
int i = 0;
int& r = i; //LValue reference: r è alias di i, non può essere messo a NULL
int&& s; //Rvalue reference
```

* struct
 
* union

### Classi in C++

```
class ResultCode {
	private:
		int code;
	public:
	 	int get_code(){return this->code;} //implementazione in line
		char* get_description(); //implementazione separata
}

char* ResultCode::get_description(){/*implementazione*/}

// `<NomeClasse>::` è chiamato scope operator.
```

se non è specificato, un metodo/attributo è private.

E' cosa comune mettere la definizione di classe in un file `.h` e l'implementazione dei metodi un file `.c` che include il `.h`, esempio:

* `myclass.h`

```
#ifndef MYCLASS //on modern compiler `pragma once` do the same exact thing
#define MYCLASS
class MyClass{
	char* ptr;
	public:
		int doWork();
}
```

* `myclass.c`

```
#include "File.h"

int MyClass::doWork(){
	//codice
	return 0;
}
```

Gli attributi definiti come `static` sono sempre allocato nella memoria globale e non nella stessa area in cui è memorizzato l'oggetto.

Un oggetto può essere allocato in:

* memoria globale
* stack (local store)
* heap (free store)
* all'interno di un altro oggetto, che a sua volta è allocato in una delle aree precedenti.


* Costruttori

```
class ResultCode {
	public:
		ResultCode(): code(0) {} //costruttore che inizializza code a zero
  		ResultCode(int c) { code=c; } 
  		int getCode();
  		char* getDescription();

	private:
 		int code;
};
```

un oggetto può essere creato chiamando il costruttore (allocazione statica):

```
ResultCode mycode_one(1); //sintassi tradizionale
ResultCode mycode_two{2}; //sintassi moderna
```

oppure mediante allocazione dinamica (si ottiene un puntatore):

```
ResultCode* mycode = new ResultCode(3); //alloca e restituisce il puntatore
...
delete mycode; //dealloca
```

oppure mediante costruttore di copia, istanziando un oggetto a partire da un'altra istanza della stessa classe, di fatto clonando un oggetto:

```
// classe con costruttore di copia
class CBuffer {
  	int size;
  	char* ptr;
  	
	CBuffer(int size): size(size){
    	ptr = new char[size];
  	}

	// costruttore di copia:
  	CBuffer(const CBuffer& source);
};

//implementazione costruttore di copia:
CBuffer::CBuffer(const CBuffer& source):
	size(source.size){
		this->ptr = new char[size];
		memcpy(this->ptr, source.ptr, size);
	}

...
//uso il costruttore di copia:
CBuffer buf1{50};
CBuffer buf2{buf1}; //copia, riceve come parametro un oggetto della stessa classe
```

SI può vietare la creazione di un duplicato dichiarando privato il costruttore di copia e richiedendone l'eliminazione nella dichiarazione della classe:

```
CBuffer(const CBuffer& source) = delete;
```

* Movimento

alcune delle variabili locali di un oggetto possono essere travasate in un altro oggetto prima della distruzione. 

Bisogna definire il costruttore di movimento:

```
TypeName(Typename&& source); //il doppio && indica un candidato al movimento del tipo che lo precede, viene detto RValue reference
``` 

esempio:

```
class CBuffer {
  	int size;
  	char* ptr;
public:
  	CBuffer(CBuffer&& source) { 
		
		//nb: modifica l'oggetto source, tanto verrà distrutto
		this->size=source.size;
		this->ptr=source.ptr;
		source.ptr=NULL;
  	}
};
```

* Distruttore

Ha il compito di rilasciare le risorse contenute in un oggetto. Se un oggetto non possiede risorse esterne, non occorre dichiararlo. Non va mai chiamato esplicitamente, la chiamata sarà inserita nel programma dal compilatore quando opportuno.

Utile dichiararlo quanod un oggetto incapsula puntatori a memoria dinamica o ad altre risorse del sistema. In questo modo le delete delle risorse create sono legate al ciclo di vita dell'oggetto:

```
class CBuffer {
  	int size;
  	char* ptr;
  	CBuffer(int size): size(size){
    	ptr = new char[size];
  	}

	//distruttore
  	~CBuffer() {
    	delete[] ptr;
  	}
};
```

### Azioni del compilatore

Il compilatore invoca costruttore e distruttore al procedere del ciclo di vita di un oggetto.

Le variabili globali sono costruite prima dell'esecuzione del main e sono distrutte dopo la sua terminazione.

Le variabili locali sono costruite all'ingreso in un blocco di codice (scope) definito tra graffe.

Le variabili allocate dinamicamente sono allocate e deallocate nello Heap esplicitamente attraverso l'uso di `new` e `delete`.


### Allocazione dinamica

Dopo la chiamata a `delete`, è bene impostare il puntatore a NULL.

```
// allocazione dinamica di array di oggetti
ClassName* ptr = new[10] ClassName;
...
// deallocazione di array di oggetti
delete[] ptr; // vengono chiamati i distruttori di tutti gli oggetti
```

Sebbene C++ consente l'utilizzo dei puntatori nativi, in associazione alle operazioni di `new` e `delete` **è bene non usarli**, utilizzando i tipi ad alto livello offerti dal C++: Array e Smart Pointers.


### Oggetti composti

in C++, un oggetto può contenere più oggetti in due differenti modi:

* come parte integrante della propria struttura dati: conviene se gli oggetti contenuti sono in quantità fissa e non troppo grandi (non si ottiene una struttura dati allocata troppo grande)

* facendo riferimento ad essi tramite puntatori: nel costruttore vengono allocati gli oggetti, nel distruttore devono essere deallocati.


### Passaggio di parametri

Può avvenire:

* Per valore: si passa una copia duplicata (nello stack), si basa sul costruttore di copia: è un'operazione onerosa. Da usare per tipi base od oggetti semplici.

```
void func(Buffer b){

	std::cout << buf.size;
}
```

* Per indirizzo: si passa il puntatore all'originale, può essere modificato. La funzione chiamata non può fare assunzioni sul tempo di vita dell'indirizzo.


```
void func(Buffer* b){

	std::cout << *buf.size;
}
```

* Per riferimento: si passa l'originale, sia lettura che scrittura. Sintatticamente ricorda un passaggio per valore, semanticamente corrisponde ad un passaggio per indirizzo che però **non può** essere mai NULL.

```
void func(Buffer& b){

	std::cout << buf.size;
}
```

* Per riferimento costante: si passa l'originale, solo in lettura. Si ottiene precedendo la keyword `const` alla variabile passata.


```
void func(const Buffer& b){

	std::cout << buf.size;
	//buf.size = 0; //exception!
}
```

* Per [riferimento RValue](https://smartbear.com/blog/develop/c11-tutorial-introducing-the-move-constructor-and/): ottimizzazione utilizzata per il passaggio di parametri in librerie, da non usare, troppo complesso.

```
void func(Buffer&& b){

	std::cout << buf.size;
}
```

### Trasferimento di proprietà

E' possibile tasferire la proprietà di un dato ad una funzione sfruttando il passaggio di parametro rvalue ed il costruttore di movimento:

```
void sink(vector<int>&& v){
	...
}

std::vector<int> data = {1,2,3};
sink(std::move(data)); //chiamata esplicita a movimento
```

### Parametri in uscita

si può restituire un solo valore od una tupla:

```
return make_tuple(status, something()); //make_tuple è una funzione generica, accetta n parametri
```

### Copia ed Assegnazione

Se l'oggetto a sinistra dell'assegnazione esiste già, si tratta di una assegnazione. Se invece l'oggetto non esiste, allora si tratta di una copia: viene invocato il costruttore di copia.

```
Cpoint A, B;

A = B; //assegnazione, A esiste, viene invocata assegnazione
Cpoint C = B; //C non esiste, viene invocato costruttore di copia
Cpoint D(B); //D non esiste, viene invocata copia
```

Durante l'assegnazione di un oggetto, se devono essere assegnati attributi che sono puntatori, quello che fa il compilatore è copiare il puntatore sorgente in quello destinazione, così i due puntatori punteranno allo stesso oggetto/area di memoria (male) e l'area puntata dal puntatore destinazione non sarà più accesibile (male, memory leakage).

Per questo motivo, occorre sempre **ridefinire l'operatore di assegnazione per la classe**, se essa contiene puntatori o riferimenti a memoria allocata dinamicamente:

```
CBuffer& operator=(const CBuffer& source){

	if (this != &source){ //protezione

		delete[] this->ptr;
		this->ptr = null; //in caso di eccezione della successiva new, il distruttore farebbe un'altra delete
		
		this->size = source.size;
		this->ptr = new char[size];
		memcpy(this->ptr, source.ptr, size);
	}

	return *this;
}
```

L'operatore di assegnazione di sovrappone al distruttore: **deve** rilasciare tutte le risorse possedute.

In ogni caso, costruttore di copia ed operatore di assegnazione **devono** essere semanticamente equivalenti.

**Regola dei tre**: se una classe dispone di una qualunque di queste funzioni membro: *costruttore di copia, operatore di assegnazione, distruttore*, allora **deve** implementare le altre due. In mancaza di ciò, il compilatore fornirà la propria implementazione, **non generando alcun errore o warning**, e che sarà nella maggior parte dei casi errata.


In alcuni casi la copia non è possibile, ma il movimento si ( i.e. `std::unique_ptr`). In questi casi occorre ridefinire l'operatore di movimento, nel caso in cui siano presenti membri allocati dinamicamente:

```
CBuffer& operator=(CBuffer&& source){

	if(this != &source){
	
		delete[] this-> ptr;
		this->size = source.size;
	
		this->ptr=source.ptr;
		source.ptr = nullptr;
	}
	
	return *this;
}
```



### Ereditarietà in C++

Molto differente rispetto a java: **una classe può ereditare da una o più classi**. Non esiste una classe radice (Object per java).

I metodi della classe di base possono essere invocati usando l'operatore di scope `::`, i.e. `ClasseBase::NomeMetodo()`.


### Polimorfismo in C++

Se B estende A, allora è possibile assegnare a `A&` un oggetto B: sfruttabile solo quando si ha a che fare con puntatori, perchè la memoria allocata per A è sempre minore di quella allocata per B, non ci entrerebbe se allocato staticamente.

Override di metodi: per default, il compilatore utilizza l'implementazione definita nella classe a cui ritiene appartenga l'oggetto. Per forzare un metodo come polimorfico, occorre utilizzare la keyword `virtual`. In questo caso verrà chiamata l'implementazione corretta.
  
I metodi virtuali astratti sono dichiarati "=0". Una **classe astratta** contiene almeno una funzione virtuale astratta. Una classe che contiene solo funzioni virtuali è equivalente ad una interfaccia java.

Gli oggetti C++ non posseggono alcun campo che definisce il tipo della classe, per chiamare il metodo corretto, viene utilizzato il meccanismo della `V-table.`

I metodi virtual sono aggiunti nella `V-table` che è la tabella dei metodi virtuali. Ogni volta che viene creato un oggetto che contiene almeno la dichiarazione o implentazione di un metodo virtuale, nella memoria allocata si aggiunge il puntatore alla **V-table della classe**. Questo significa che la chiamata ad un metodo virtuale costa un pò di più.

```
Base* b1 = new Der1(); //Base definisce f come virtual, Der1 lo implementa
Base* b2 = new Der2(); //Base definisce f come virtual, Der2 lo implementa (diverso da quello di Der1)

b1->f(); //il compilatore accede alla vtable di b1 da cui legge il puntatore ad f per Der1
```


### Typecast


### Eccezzioni

Gestite con il costrutto try-catch-catch...

`std::exception` e le sue sottoclassi possono essere estese per gestire eccezioni personalizzate, incapsulano una stringa accesibile dal metodo `what()`. NON usare questa stringa per discernere la tipologia di eccezione, è disponibile a solo scopo informativo. 

* Pattern "Second Phase Construction": se potrebbe verificarsi un'eccezione nel costruttore, fare nel costruttore il minimo indispensabile (sicuri che non sorgano eccezioni) e poi fare l'inizializzazione in un metodo da chiamare, simile all' `onCreate()` di Android.

In C++ esiste la notazione `catch(...)` per catturare qualunque eccezione.

chiamare `throw` dentro un blocco catch rilancia all'esterno l'eccezione catturata dal catch.

** Mai scrivere un blocco catch che non esegue nessuna strategia di riallineamento **




### Resource Acquisition Is Initialization (RAII)

Uno dei paradigmi del C++: se devo utilizzare una risorsa, scrivo una classe in cui nel costruttore (inizializzazione) acquisisco la risorsa (i.e. apro file) e nel distruttore rilascio la risorsa. In questo modo il ciclo di vita dell'oggetto rispecchierà il ciclo di vita dell'uso della risorsa: quando il programma uscirà dallo scope in cui è stata inizializata la classe, il distruttore verrà chiamato e la risorsa rilasciata.

Questo paradigma è supportao dal fattoche lo stack viene contratto in fase di lancia di un'eccezione, in modo che le risorse acquisite alla costruzione siano liberate.



