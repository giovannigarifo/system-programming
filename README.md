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

## Classi in C++

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

## Azioni del compilatore

Il compilatore invoca costruttore e distruttore al procedere del ciclo di vita di un oggetto.

Le variabili globali sono costruite prima dell'esecuzione del main e sono distrutte dopo la sua terminazione.

Le variabili locali sono costruite all'ingreso in un blocco di codice (scope) definito tra graffe.

Le variabili allocate dinamicamente sono allocate e deallocate nello Heap esplicitamente attraverso l'uso di `new` e `delete`.


## Allocazione dinamica

Dopo la chiamata a `delete`, è bene impostare il puntatore a NULL.

```
// allocazione dinamica di array di oggetti
ClassName* ptr = new[10] ClassName;
...
// deallocazione di array di oggetti
delete[] ptr; // vengono chiamati i distruttori di tutti gli oggetti
```

Sebbene C++ consente l'utilizzo dei puntatori nativi, in associazione alle operazioni di `new` e `delete` **è bene non usarli**, utilizzando i tipi ad alto livello offerti dal C++: Array e Smart Pointers.


## Oggetti composti

in C++, un oggetto può contenere più oggetti in due differenti modi:

* come parte integrante della propria struttura dati: conviene se gli oggetti contenuti sono in quantità fissa e non troppo grandi (non si ottiene una struttura dati allocata troppo grande)

* facendo riferimento ad essi tramite puntatori: nel costruttore vengono allocati gli oggetti, nel distruttore devono essere deallocati.


## Passaggio di parametri

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

## Trasferimento di proprietà

E' possibile tasferire la proprietà di un dato ad una funzione sfruttando il passaggio di parametro rvalue ed il costruttore di movimento:

```
void sink(vector<int>&& v){
	...
}

std::vector<int> data = {1,2,3};
sink(std::move(data)); //chiamata esplicita a movimento
```

## Parametri in uscita

si può restituire un solo valore od una tupla:

```
return make_tuple(status, something()); //make_tuple è una funzione generica, accetta n parametri
```

## Copia ed Assegnazione

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

#### Risolvere le problematiche di copia ed assegnazione: Il paradigma Copy&Swap

...



## Ereditarietà in C++

Molto differente rispetto a java: **una classe può ereditare da una o più classi**. Non esiste una classe radice (Object per java).

I metodi della classe di base possono essere invocati usando l'operatore di scope `::`, i.e. `ClasseBase::NomeMetodo()`.

Si può ereditare sia in modo pubblico che privato:

* ereditarietà pubblica: `class Derived: public Base{}` rappresenta la relazione `is-a`
* ereditarietà privata: `class Derived: Base{}` rappresenta la relazione `has-a`


## Polimorfismo in C++

Se B estende A, allora è possibile assegnare a `A&` un oggetto B: sfruttabile solo quando si ha a che fare con puntatori, perchè la memoria allocata per A è sempre minore di quella allocata per B, non ci entrerebbe se allocato staticamente.

Override di metodi: per default, il compilatore utilizza l'implementazione definita nella classe a cui ritiene appartenga l'oggetto. Per forzare un metodo come polimorfico, occorre utilizzare la keyword `virtual`. In questo caso verrà chiamata l'implementazione corretta. **Solo le funzioni dichiarate `virtual` sono polimorfiche**
  
I metodi virtuali astratti sono dichiarati "=0". Una **classe astratta** contiene almeno una funzione virtuale astratta. Una classe che contiene solo funzioni virtuali è equivalente ad una interfaccia java. Una classe astratta, che cioè contiene almeno un metodo puramente virtuale (dichiarato `=0` e non implementato) non può essere istanziata.

Anche **i distruttori non sono polimorfici** per default. È opportuno che tutte le classi che posseggono risorse ed espongono tali risorse con funzioni virtuali, abbiano il distruttore dichiarato virtuale

Gli oggetti C++ non posseggono alcun campo che definisce il tipo della classe, per chiamare il metodo corretto, viene utilizzato il meccanismo della `V-table.`

I metodi virtual sono aggiunti nella `V-table` che è la tabella dei metodi virtuali. Ogni volta che viene creato un oggetto che contiene almeno la dichiarazione o implentazione di un metodo virtuale, nella memoria allocata si aggiunge il puntatore alla **V-table della classe**. Questo significa che la chiamata ad un metodo virtuale costa un pò di più.

```
Base* b1 = new Der1(); //Base definisce f come virtual, Der1 lo implementa
Base* b2 = new Der2(); //Base definisce f come virtual, Der2 lo implementa (diverso da quello di Der1)

b1->f(); //il compilatore accede alla vtable di b1 da cui legge il puntatore ad f per Der1
```


## Typecast

* `static_cast<T>(p)`, utile per scendere nell'albero di ereditarietà
* `dynamic_cast<T>(p)`, utile per salire nell'albero di ereditarietà
* `reinterpret_cast<T>(p)`, analogo al cast c-style
* `const_cast<T>(p)`, elimina la caratteristica di costante da p


## Eccezioni

Gestite con il costrutto try-catch-catch...

`std::exception` e le sue sottoclassi possono essere estese per gestire eccezioni personalizzate, incapsulano una stringa accesibile dal metodo `what()`. NON usare questa stringa per discernere la tipologia di eccezione, è disponibile a solo scopo informativo. 

* Pattern "Second Phase Construction": se potrebbe verificarsi un'eccezione nel costruttore, fare nel costruttore il minimo indispensabile (sicuri che non sorgano eccezioni) e poi fare l'inizializzazione in un metodo da chiamare, simile all' `onCreate()` di Android.

In C++ esiste la notazione `catch(...)` per catturare qualunque eccezione.

chiamare `throw` dentro un blocco catch rilancia all'esterno l'eccezione catturata dal catch.

**Mai scrivere un blocco catch che non esegue nessuna strategia di riallineamento**


### Resource Acquisition Is Initialization (RAII)

Uno dei paradigmi del C++: se devo utilizzare una risorsa, scrivo una classe in cui nel costruttore (inizializzazione) acquisisco la risorsa (i.e. apro file) e nel distruttore rilascio la risorsa. In questo modo il ciclo di vita dell'oggetto rispecchierà il ciclo di vita dell'uso della risorsa: quando il programma uscirà dallo scope in cui è stata inizializata la classe, il distruttore verrà chiamato e la risorsa rilasciata.

Questo paradigma è supportao dal fatto che lo stack viene contratto in fase di lancio di un'eccezione, in modo che le risorse acquisite alla costruzione siano liberate.


## Funzioni ed operatori

* **Operator overloading**:

per via del **principio di espressività** del C++, se è lecito effettuare operazioni (+,-,<<) tra tipi base, lo è anche tra gli oggetti: per questi oggetti, è necessario ridefinire le operazioni base.

Non è possibile definire nuovi operatori, o cambiare le precedenze o l'arità (numero di argomenti dell'operatore) dell'operatore.

L'operatore ternario `? :` non è ridefinibile, così come l'operatore di scope `::`, l'operatore di accesso `.` e `sizeof`.

Si può invece ridefinire:

1) subscript operator `[]`
2) puntatore a funzione: `<tipo-ritornato> (* myfunc) (<argomenti>)`, si può omettere l'asterisco

```
int f(int i, double d){
	//corpo
}

//definisco puntatore a funzione
int (*var)(int, double);
var = f; //inizializzo
var = &f;
var(10, 3.14);
```

Esistono gli **oggetti funzionali**: istanza di una qualsiasi classe che abbia ridefinito la funzione membro `operator ()`
```
class FC{
	public:
		int operator() (int v){
			return v*2;
		}
}

FC fc;
int i = fc(5);
```

Se voglio scrivere una funzione che può ricevere come parametro sia un oggetto funzionale che un puntatore a funzione, posso definirla così: 

```
template <typename F>
void some_func(F& f) {
	f();
}
```

Esiste **l'operatore di conversione**

```
operator Tipo_o_UDT() const {
	//..conversione del dato ad un'altro tipo o UDT..
	}
```

* **Funzioni Lamba**

```
std::for_each(v.begin(), v.end(), [](int i){/*code*/})

//oppure, se torna dei valori

[](int num, int den) -> double {
	if(den=0)
		return std::NaN;
	else
		return (double)num/den;
}
```

Se si vogliono rendere disponibili delle variabili locali all'interno delle funzioni lambda, possiamo aggiungerli all'interno della notazione lambda (passati sia per riferimento che per valore) `[]`. Se si usa la notazione `[&]` tutte le variabili utilizzate nella funzione lambda che non sono passate come parametri sono prese dal contesto in cui è chiamata la funzione lambda. **Nota che questi valori sono mantenuti, come per gli oggetti funzionali, come se fossero definiti static dentro la lambda**, questo perchè una funzione lambda viene trasformata dal compilatore in un oggetto funzionale!

Una funzione lambda che cattura dei valori viene detta **closure**.

```
int k = 3
auto f = [k] (int i) {return i*k}
```

* Il tipo `std::function` permette di modellare una funzione che riceve argomenti di tipo Args e restituisce un valore di tipo R. A questo tipo possono essere assegnati oggetti funzionali, lambda, puntatori a funzione purchè la firma sia conforme a quella indicata.

```
import <functional>

std::function<float(float,float)> test_fun;

test_fun = std::fmaxf;
test_fun = std::multiplies<float>();
int x = 1;
test_fun = [x](float a, float b) {return a*x*b;} //lambda 
test_fun = [x](auto a, auto b){returb a*x*b;} //lambda generica
```

* Funzioni membro `const`:

Dichiarare una funzione membro come `const` forza la funzione ad essere "read-only", è una funzione che non modifica l'oggetto sulla quale è chiamata.

```
class Base { //an abstract class
	public:
		virtual int getName() const = 0;
}
```

* Funzioni membro `static`:

Permetto di essere invocate senza che un oggetto della classe sia stato istanziato.


## Programmazione generica e Smart Pointers

Si può definire una funzione in modo che operi su un tipo di dato non ancora precisato, vengono chiamate **template**:

```
template <typename T>
const T& max(const T& t1, const T& t2){
	return (t1 < t2 ? t2 : t1);
}
```
Opera sul tipo generico T a patto che supporti:

* l'operatore "<"
* il construttore di copia, per derivare da una variabile temporanea a partire dal dato costante

Ogni volta che si usa la funzione, il compilatore determina quale tipo effettivo assegnare a "T". I parametri sono passati come `const` proprio perchè devono essere valutabili in fase di compilazione.

Esempio di uso:

```
int i = max(19,29); // T-> int
double d = max<double>(2, 3.14); // forza scelta di T a double
```

Si possono definire anche **classi generiche**:

```
template <typename T>
class Accum {
	T total;
public:
	Accum(T start): total(start){}
	T operator+=(const T& t){ return total+t;}
	T value() { return total; }
}

Accum<std::string> stringAccum("");
Accum<int> intAccum(0);
```

* NOTA: i template delle classi generiche vanno definite nella stessa unità di traduzione (i.e. file.h), non è possibile separare dichiarazione e definizione dell'elemento generico (i.e. dichiarazione in .h e definizione in .c)


### Smart Pointer

La ridefinizione degli operatori permetti di dare una semantica alle operazioni `*` ed `->` anche a dati che non sono (solo) puntatori: ciò permette di creare oggetti che si utilizzano come puntatori ma offrono molto di più.

I template per gli smart pointer sono disponibili tramite `#include <memory>`.

* `std::unique_ptr<BaseType>`: impedisce la copia, ma permette il trasferimento. Implementa il concetto di proprietà. Se utilizzato per puntatore ad array, bisogna utilizzare `<BaseType[]>`. Ha gli stessi tempi di esecuzione e la stessa occupazione di memoria di un puntatore nativo.

* `std::shared_ptr<BaseType>`

Permette proprietà condivisa con un meccanismo di conteggio dei riferimenti. Utilizza più risorse di un puntatore nativo. La funzoine `std::make_shared<BaseType>(Args...)` viene usata per creare un oggetto di tipo BaseType, passando al suo costruttire gli argomenti `Args...`, e ritornarne uno shared pointer.

Cast tra shared_ptr, sono disponibili le due funzioni:

```
std::shared_ptr<T> static_pointer_cast<T>(shared_ptr<U>)
std::shared_ptr<T> dynamic_pointer_cast<T>(shared_ptr<U>) //usare per scendere in albero ereditarietà (i.e. da classe puntatore ad oggetto classe base voglio quello della classe derivata, se non possibile ritorna nullptr)
std::shared_ptr<T> const_pointer_cast<T>(shared_ptr<U>)
std::shared_ptr<T> reinterpret_pointer_cast<T>(shared_ptr<U>)
```

Permettono di creare uno shared_ptr a una classe base e farne il cast a uno shared_ptr di classe derivata.



* `std::weak_ptr<BaseType>`

Permette di osservare il contenuto di uno shared_ptr senza partecipare al conteggi dei riferimenti. Offre i metodi `lock()` ed `expired()` per la sua gestione.



Per tutti i puntatori, il metodo `reset()` permette di rilasciare il puntatore senza eliminare l'oggetto pointer, in modo tale da riutilizzarlo, l'area di memoria puntata viene rilasciata.


## Input/Output

oltre alle funzioni della libreria `stdio.h` come printf/scanf, il c++ offre un'alternativa per la gestione dei flussi di input/output, basata su una gerarchia di classi per la scrittura/lettura di stream:

`ios` è la classe base, virtuale. `istream` ed `ostream` derivano da `ios` e definiscono metodi per input ed output su stream generici, ridefiniscono anche gli operatori `<<` e `>>`. Consentono accesso indiretto allo stream tramite il buffer interno.

`iostream` esegue operazioni di lettura e scrittura.

* Operazioni sui file:

Le classi base sono `ifstream` ed `ofstream`, `fstream` eredita da entrambe e permette accesso sia in lettura che scrittura.

Le operazioni di IO **non sollevano eccessioni**: il programma deve testare esplicitamente il risultato di ogni operazione effettuata. Le funzioni `eof()`, `fail()`, `bad()`, `good()` chiamate sullo stream permettono di discernere la condizione che si è verificata in seguito all'ultima operazione di IO. In uno stato bad, tutte le successive operazioni di lettura/scrittura falliscono silenziosamente. Il metodo `clear()` invocato sullo stream ripristina lo stato dle flusso: chiamare questo metodo dopo aver sistemato lo stream in seguito ad un fail/bad.




## Standard Template Library (STL)

Libreria basata su contenitori, algoritmi ed iteratori. La librerria è scritta utilizzanto template (programmazione generica).

### Container

* Sequenziali: organizzano linearmente una collezzione di oggetti. Array, vector, deque, list, forward_list

* Associativi: contenitori non lineari, organizzati in coppie chiave-valore: accesso agli oggetti tramite chiave. Ordinati: set, map, multiset, multimap. Non ordinati: unordered_set, unordered_map, unordered_multiset, unordered_multimap.

In c++ per ogni tipo UDT fa definira una funzione hashcode, questo perchè non esiste come in java la definizione della classe padre (object per java).

* Container adapter: sono contenitori sequenziali specializzati. stack (LIFO), queue (FIFO), priority_queue. NON supportano gli iteratori.

## std::vector

usa internamente un array allocato dinamicamente.

Elementi accesibili non solo con iteratori ma anche con puntatori tradizionali + offset (memoria è allocata contiguamente). Può espandersi e contrarsi a seconda del bisogno. `capacity` e `size` ritornano rispettivamente lo spazio allocato per il vettore e la dimensione del vettore.

* std::string equivale a `std::vector<char>`

std::string non fa parte dei contenitori della STL! 	
  

## Contenitori associativi

I dati sono aggregati non per posizione, ma per chiave. Supportano iteratori bidirezionali (elemento successivo - elemento precedente). 

* **set, multiset, map, multimap**: mantengono gli oggetti **ordinati per chiave**, devono supportare l'operatore `<`. Tempo di accesso O(log(n)). Per le mappe, `std::make_pair<KeyType, ValueType>(key, value)` permette di creare una pair da inserire.

* **unordered_set, unordered_multiset, unordered_map, unordered_multimap**: oggetti indicizzati per hash. Tempo di accesso costate. Non sono presenti elementi duplicati (set). La chiave deve essere equal comparable, copiabile o movibile, e **disponibile come valore hash**.

 Per entrambe i ll valore (oggetto) deve avere un costruttore di default e deve essere copiabile e movibile.


 ## Interfaccia comune per i Container

 Nonostante esistano diversi tipi di container, essi hanno molto in comune, molte oeprazioni non dipendono dal tipo di container:

 * creazione, cancellazione, accesso e swap di elementi.
 * accessibilità attraverso iterator.
 * tranne std::array, ogni container ha un allocatore che lavora in background, volendo, si può fornire un allocator personalizzato invece di quello di default. L'allocator permette di espandere e contrarre dinamicamente il container.

I container supportano il confronto, vengono confrontate le chiavi.

```
vector<int> v1{1,2,3,4};
vector<int> v2{1,2,3,4};
count << (v1==v2) <<endl; //true
std::swap(v1,v2)
for (auto v: v1) std::cout << v << " ";
```

## Iteratori

```
class c;
vector<C> v(10);
vector<C>::iterator iter = v.begin();
vector<C>::iterator end = v.end();

for(; iter!=end; iter++){ C elem = *iter;}
```

La classe dell'iteratore definisce le operazioni necessarie per lavorare con il contenitore utilizzando l'iteratore: il compilatore sceglie l'implementazione corretta.

**per ogni contenitore esiste un tipo di iteratore** che implementa la corretta semantica delle operazioni.

Quando l'iteratore lavora sulla collezione, la collezione non può essere modificata.

Tipologie di iteratori:

* forward iterator
* bidirectional iterator
* random access iterator

Per ottenere un iteratore oltre a container.begin() e container.end() si possono usare:

```
std::begin(cont);
std::end(cont);
std::rbegin(cont); //punta all'ultimo ed itera all'indietro
std::rend(cont);
std::cbegin(cont);
std::cend(cont);
std::crbegin(cont);
std::crend(cont);
std::prev(iter);
std::next(iter);
std::advance(iter, n);
```


## Container Adaptor

Contenitori sequenziali che hanno memoria di come sono stati inseriti gli oggetti:

* `std::stack`: push, pop, top. Supporta gli operatori di confronto. complessità O(1).
* `std::queue`: push, pop, front, back. 
* `std::priority_queue`

Non possono essere usati con gli algoritmi della STL., supportano un'interfaccia ridotta rispetto agli altri container. Non supportano gli iteratori e conoscono la loro dimensione.

## Algoritmi

STL fornisce algoritmi per lavoare sui container ed i loro elementi: sono implementati come funzioni template e sono indipendenti dal tipo di elementi del container. L'algoritmo viene applicato in loop ad un intervallo di dati.

* in `<numeric>` si trovano algoritmi numerici come accumulate (che non lavora solo sui numeri), inner_product_adjacent_difference...

* in `<algorithm>` sono raccolti algoritmi per ricerca e ricerca binaria, trasformazione dei dati, partizionamento, ordinamento, merge, operazioni insiemistiche, operazioni su heap, comparazioni lessografiche...

**Algoritmi non modificanti**: applicano funzioni agli elementi senza modificarne la struttura: 

* all_of, any_of, none_of
* for_each, for_each_n, applicano una funzione ad un range di elementi
* find, find_if, find_if_not

**Algoritmi modificanti**: algoritmi che possono modificare la struttura di un container.

* copy, copy_if,copy_... copiano range di elementi in una nuova locazione
* move, move_if, move_backward muovono un range di elementi in una nuova locazione
* remove, remove_if rimuovono gli elementi che soddisfano uno specifico criterio
* fill, fill_n
* transform, applica un mapping
* generate, generate_n, trovano il primo elemento che soddisfa un criterio

**Algoritmi per il partizionamento**:

* partition
* partition_copy
* stable_partition
* is_partitioned
* partition_point

**Algoritmi per l'ordinamento**:

* sort, stable_sort
* partial_sort
* is_sorted
* is_sorted_until

**Algoritmi per ricerca binaria**:

* lower_bound
* upper_bound
* binary_search
* equal_range

La maggior parte degli algoritmi della TL accetta come parametro una delle execution policy definite in `<execution>`, permettono l'esecuzione sequenziale, parallela o parallela con vettorizzazione.

* `std::execution::seq` esecuzione sequenziale
* `std::execution::par` esecuzione parallela con multithreading (genera un numero di thread pari a quelli del processore)
* `std::execution::par_unseq` esecuzione vettorizzata con SIMD (per GPU: single instruction (algorithm), multiple data)







