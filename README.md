# **Programmazione di Sistema**

Link utili:

* [Compilazione condizionale con direttive al preprocessor (#ifdef...)](http://www.fe.infn.it/u/spizzo/prog09/lezioni04/direttive_def.html)


# **Piattaforme di esecuzione**

Fatto salvo per i sistemi elementari (i.e. embedded), **una applicazione è eseguita nel contesto di un sistema operativo**: esso offre i **servizi**, le **funzionalità** e le **convenzioni** che permettono il corretto funzionamento dell'applicazione.

Per sfruttare tali servizi, l'applicazione deve conformarsi alle specifiche del SO: sia a livello di codice sorgente, che a livello di codice eseguibile: anche il compilatore deve essere configurato in modo tale da produrre codice compliant con il SO di destinazione.

## API

definisce un **set di funzioni e di strutture dati** offerte al programmatore per interfacciarsi con i servizi offerti dal sistema sottostante. 

Le API low level offerte dal SO, le **System Call** (SysCall), sono annegate in funzioni di libreria (i.e. syscall `open()` annegata in funzione della C stdlib `fopen()`). Questo è necessario per evitare di dover conoscere i dettagli delle architetture target, dato che **l'implementazione della syscall dipende dall'architettura hardware**. Per utilizzare le API è necessario includere gli appositi file header, e.g. per windows includere il file `Windows.h`. In alcuni casi può essere necessario specificare al linker le librerie da collegare prima di generare l'eseguibile, e.g. la direttiva `-pthread` per specificare a GCC di includere la libreria per la gestione della concorrenza POSIX compliant in ambiente unix-like.

Le API permettono di interfacciarsi con le strutture dati che il SO gestisce per ogni singolo processo (e.g. la Open File Table) attraverso l'utilizzo di riferimenti opachi, detti **handle** in Windows o **file descriptor** in linux. Di fatto, sono implementati come interi utilizzati per indicizzare le suddette strutture dati attraverso la API.


## ABI

definisce **il formato** che deve avere un prodotto SW per essere compatibile con il SO: **convezioni** da utilizzare (i.e. accesso a strutture dati del sistema attraverso Handle/FileDescriptor), uso dei registri, innalzamento di privilegio, caricamento dinamico di moduli, emulazione dlele piattaforme di esecuzione, ecc. E' supportata dagli strumenti che compongolo la toolchain. 


## Gestione degli errori

Le funzioni di una API possono avere successo o fallire, occorre gestirne gli errori: bisogna verificare il tipo di ritorno, e se si è verificato un errore, utilizzare una specifica funzione per ottenere il codice dell'errore (in windows la funzione `GetLastError()`, in linux la pseudo-variabile globale `errno`).

1. **Windows**

Occorre verificare il valore ritornato dalla chiamata alla funzione della API:

* `BOOL`, è un typedef di `int`, 0 in caso di fallimento, qualunque altro valore in caso di successo.
* `HANDLE`, è un typedef di `void*`, 0 o -1 in caso di fallimento (dipende dalla versione di windows), un valore >0 in caso di successo.
* `PVOID`, typedef di `void*`, NULL indica fallimento, un puntatore valido indica successo.
* `LONG` o `DWORD`, typedef di `unsigned long`, dipende dal significato del valore ritornato.

Una volta appurato che si è verificato un errore, si possono ottenere i dettagli sull'errore che si è verificato attraverso la chiamata alla funzione `DWORD getLastError()`, che ritorna il codice dell'errore (su 32bit). E' eseguita nel contesto del thread, il valore ritornato non è un indicatore dello stato di errore globale dell'applicazione, ma del singolo thread che chiama `getLastError()`.

2. **Linux**

Non esiste uno standard per i valori di ritorno, prevalenentemente `-1` indica il fallimento.

Si accede al codice dell'errore ispezionando il contenuto della **pseudo-variabile globale** errno. E' definita come una macro:

```
#define errno (*__errno_location())
```

La funzione ritorna un puntatore ad un intero che punta al codice di errore. Prima che il kernel linux supportasse il multi threading era implementata come una variabile globale.

Per entrambe i SO, **occorre verificare il tipo di errore che si è verificato immediatamente** dopo il ritorno: qualunque altra chiamata a funzione potrebbe scatenare un secondo errore, che sovrascrirebbe l'errore precedente.


## Character Sets

Il consorzio UNICODE ha definito una rappresentazione standard dei simboli. Tre rappresentazioni possibili, di cui due a lunghezza variabile:

* `UTF-32`: ogni simbolo occupa 32 bit.
* `UTF-16`: un simbolo può occupare una o due parole da 16 bit.
* `UTF-8`: un simbolo può occupare da una a quattro parole da 8 bit. Per efficienza, i simboli più frequenti sono codificati su 8 bit.


1. Caratteri in **Windows**

due tipi base: 

* `char`, codifica ASCII estesa (256 caratteri) su 8bit
* `wchar_t`, codifica Unicode `UTF-16`.

Tutte le API del sistema che prevedono l'uso di caratteri e stringhe sono offerte in due versioni, anche se viene utilizzata solo la versione generica basata sul tipo `TCHAR`, definita in fase di compilazione con la direttiva al preprocessore `#ifdef` (compilazione condizionale) in base al fatto che la macro `UNICODE` sia stata definita o meno, viene definita la versione generica che corrisponde alla versione che utilizza codifica unicode o ascii.

Esempio: 
```
BOOL CreateDirectoryA(
	  LPCSTR lpPathName, //sequenza di char terminata da \0
	  LPSECURITY_ATTRIBUTES lpSecAtt);
BOOL CreateDirectoryW(
	  LPCWSTR lpPathName, //sequenza di wchar_t
	  LPSECURITY_ATTRIBUTES lpSecAtt);

#ifdef UNICODE
#define CreateDirectory CreateDirectoryW 
#else
#define CreateDirectory CreateDirectoryA
#endif // !UNICODE
```

1. Caratteri in **Linux**

GCC codifica i caratteri non ASCII con la codifica UTF-8, **all'interno della stessa stringa**, che è comunque terminata da `\0`, questo può creare alcuni problemi con funzioni che utilizzano l'associazione caratteri=byte come `strln()`. utilizzare invece la funzione `mbtowcs()` (multi-byte to wide-char string).

In linux il tipo `wchar_t` esiste ma ha lunghezza di 4B e può ospitare qualsiasi carattere UNICODE. Sequenze costanti di `wchar_t` sono precedute da `L`.

```
wchar_t* s = L"Ω€®™åß∂ƒ∞∆ªøπº¬∑∫µ"; //definizione preceduta da "L"
```


## Lo standard **POSIX**: Portable Operating System Interface

Definisce un **modello concettuale**, un insieme di **API a basso livello** e un **set di comandi shell** volti a **rendere portabili gli applicativi tra i diversi sistemi operativi** minimizzandone le differenze.

Linux e MacOS sono pienamente conformi allo standard POSIX, Windows ed Android solo in parte.

Le assunzioni principali riguardano:

* La struttura del file system
* L'uso di handle/fd per fare riferimento all'interno di un programma ai descrittori dei file ed in generale per l'accesso alle strutture dati fornite dal SO.
* L'uso di un sottoinsieme di caratteri portabili
* La gestione degli errori
* L'aritmetica e la rappresentazione dei numeri
* L'utilizzo di thread e processi

La `stdlib` di C/C++ è stata definita sulla base delle assunzioni dello standard POSIX.


# **Il Modello di Esecuzione**

Modello di esecuzione di un linguaggio di programmazione: 

	Insieme dei comportamenti attuati dall'elaboratore a fronte dei costrutti di alto livello del inguaggio.

Tale modello spesso non corrisponde a quello di un elaboratore reale: è compito del compilatore trasformare il codice scritto dal programmatore in codice eseguibile dall'elaboratore, attraverso l'utilizzo delle funzionalità offerte dalla runtime library del dispositivo.

Le **librerie di esecuzione** (runtime libs) supportano le astrazioni del linguaggio di programmazione, forniscono un'interfaccia uniforme tra i diversi SO per le funzioni ad esse demandate (i.e. stack, file, malloc). Sono costituite da due tipi id funzioni:

* alcune, invisibili al programmatore, sono inserite in fase di compilazione per supportare l'esecuzione (i.e. gestione dello stack)
* altre offrono funzionalità standard (i.e. malloc)

## Modello di esecuzione dei linguaggi C/C++

* **Isolamento**: i programmi sono pensati come se fossero gli unici utilizzatori dell'elaboratore.

* I programmi assumono di poter accedere a qualsiasi indirizzo di memoria.

* Non ci sono limiti sulle istruzioni che un programma può eseguire, sul tempo richiesto e sulla memoria necessaria all'esecuzione.

* Il programma è costituito da un flusso di esecuzione il cui punto di partenza è predefinito: il `main()` in C, i costruttori delle variabili globali in C++.

	Flusso C++: costruttori var.glob. -> main -> ... -> return main -> distruttori var.glob. 

* Lo **stack** permette di gestire le **chiamate annidate tra funzioni**. Supporta la ricorsione e l'uso di **variabili locali**. In c++ gestisce anche la gestione strutturata delle eccezioni (**stack unwinding**).

* Flussi paralleli di esecuzione (Thread) sono dotati del proprio stack.

La libreria di esecuzione non può da sola garantire la corretta implementazione di tutte le astrazioni del modello di esecuzione, in particolare quelle legate all'isolamento ed alle sue conseguenze: se più programmi sono in esecuzione, il malfunzionamento di uno non deve avere conseguenze sugli altri.

Per questo motivo **i programmi C++ sono isolati, dal SO, nel contesto di esecuzione di un processo**. Il processo viene modellato da un'opportuna struttura dati interna al sistema operativo che contiene tutte le informazioni utili per la sua gestione: memoria allocata (puntatori ed offset), PID, ecc. 

### Creazione di un processo

1. **creazione dello spazio di indirizzamento** (virtuale): sezione per il codice, dati, stack ed heap.
2. **caricamento dell'eseguibile** (formato ELF in linux ed PE2 in windows) in memoria, nella sezione per il codice e per i dati dello spazio di indirizzamento.
3. **caricamento delle librerie**: esse vengono ricorsivamente mappate nello spazio di indirizzamento.
4. **avvio dell'esecuzione**, la funzione di avvio: configura la piattaforma di esecuzione (inizializza stack, registri, strutture dati per la gestione delle eccezioni), invoca i costruttori degli oggetti globali, invoca il main, ..., invoca i distruttori degli oggetti globali, rilascia l'intero spazio di indirizzamento e tutte le risorse esse collegate (chiamata ad `exit()`). In linux, è la funzione `_start()`. In windows, quattro alternative: se l'applicazione è grafica o meno, se ansi o unicode.

### Modalità di esecuzione di un processo

SO necessita del supporto in hardware per il passaggio tra due modalità di esecuzione:

* **modalità utente**: permette l'esecuzione di un sottoinsieme delle istruzioni offerte dall'ISA del processore.
* **modalità supervisore**: accesso illimitato a tutte le funzionalità del sistema.

Per consentire ai programmi utente l'accesso controllato a funzionalità accessibili solo in modlaità superuser, sono state introdotte le SysCall. La chiamata ad una SysCall comporta il passaggio dell'esecuzione al SO in modalità superuser (innalzamento di privilegio): il costo è molto elevato, nell'ordine di 500 clock cycles.


# **Allocazione della Memoria**

Quando un processo viene creato, il suo spazio di indirizzamento viene popolato con diverse aree di memoria, ciascuna dotata di propri criteri di accesso.

* **codice eseguibile**, contiene le istruzioni in codice macchina, accesso r+x.
* **costanti**, accesso r.
* **variabili globali**, accesso r+w. Hanno indirizzo fisso, determinato dal compilatore e dal linker.
* **stack**, contiene indirizzi e valori di ritorno, parametri e variabili locali (hanno indirizzo relativo allo stack), accesso r+w.
* **heap**, insieme di blocchi di memoria disponibili per l'allocazione dinamica, variabili dinamiche sono accesibili solo tramite puntatori, la libreria di esecuzione fornisce meccanismi per l'allocazione ed il rilascio di tali variabili. L'indirizzo di una variabile allocata dinamicamente viene deciso a runtime.

## Allocazione dinamica

* in C, malloc, calloc, realloc
* in C++, viene definito il nuovo costrutto `new`: alloca nello heap un blocco di dimensioni opportune, invoca il costruttore della classe sul blocco, restituisce il puntaotre all'oggetto inizializzato.

### Rischi nell'uso dei puntatori

* **Dangling pointer**: accesso ad un indirizzo di memoria quando il ciclo di vita della variabile ivi allocata è terminato. La memoria inidirizzata può essere inutilizzata, in uso ad altre parti del programma o non mappata.
* **Memory Leakage**: non rilasciare la memoria allocata dinamicamente non più in uso.
* **Wild pointer**: accesso all'area di memoria indirizzata da un puntatore non inizializzato (contiene valori casuali).


### Allocazione dinamica in Linux

Durante l'inizializzazione del processo, nella fase relativa alla creazione dello spazio di indirizzamento, viene creato un blocco di memoria a disposizione dello heap: le funzioni di allocazione cercheranno di utilizzare la memoria disponibile per soddisfare le richieste, se nessuno dei blocchi liberi dovesse essere abbastanza grande, verrà richiesta al SO altra memoria.

La struct `mm_struct` contiene la descrizione della memoria, alcuni dei campi:

* start_stack
* mmamp_base: puntatore all'inizio della zona per il memory mapping dei file
* start_brk: puntatore all'inizio dello heap.
* brk: puntatore all'ultimo blocco allocato dello heap.


### Allocazione in Windows

Un processo può gestire più heap, di base ce ne è uno che è usato da malloc, per chiedere la creazione di nuovi Heap esistono le funzioni di libreria:

* `HANDLE HeapCreate()`
* `BOOL HeapDestroy()`
* `void* HeapAlloc(HANDLE h, DWORD options, SIZE_T s)`
* `BOOL HeapFree(HANDLE h, DWORD options, void* ptr)`



# **Introduzione al C++**

https://cppinsights.io/

Principi del C++: 

* **Astrazione a costo nullo**: ogni astrazione rispetto all'uso di funzionalità a basso livello è implementata senza penalizzare le prestazioni.

* **Espessività**: i tipi/classi definiti dal'utente (UDF) devono possedere lo stesso livello di espressività offerto dai tipi base del linguaggio.

* **Sostituzione**: è possibilire usare un UDT ovunque sia possibile usare un tipo base

Paradigmi supportati:

* programmazione ad oggetti: Incapsulamento, composizione, ereditarietà, polimorfismo.
* programmazione strutturata
* programmazione generica
* programmazione funzionale

Libreria standard limitata a sole 150 classi implementate come generiche.

Non esiste il garbage collector.

Innovazioni nel C++ moderno (ultimo standard C++17, C++20 in arrivo):

* Smart pointer: shared, unique, weak pointers.
* contenitori generici dotati di algoritmi standard, i.e. Vector, std::array
* Migliore gestione delle eccezioni
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
r = r+1; //de-referenziato automaticamente

int&& s; //Rvalue reference
```

Si può dichiarare un riferimento solo a partire da una variabile esistnete. Lo standard non definisce l'implementazione di un riferimento, nella maggior parte dei casi il compilatore lo codifica come un puntatore inizializzato che viene automaticamente de-referenziato.

* struct
 
* union

# **Classi in C++**

```
class ResultCode {
	private:
		int code;
	public:
		//implementazione in line
	 	int get_code(){return this->code;} 
		
		//implementazione separata
		char* get_description(); 
};

char* ResultCode::get_description(){/*implementazione*/}

// `<NomeClasse>::` è chiamato scope operator.
```

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

Gli **attributi definiti come `static` sono sempre allocati nella memoria globale** e non nella stessa area in cui è memorizzato l'oggetto.

Incapsulamento:

* **private**: valore di default, se non specificato. Accessibile solo dalla classe.
* **protected**: accessibile solo dalla classe e dalle sue sotto classi
* **public**: accessibile da tutti.
 
Accessibilità:

* **const**: vieta la modifica dell'oggetto/attributo.

Un oggetto può essere allocato in:

* memoria globale
* stack (local store)
* heap (free store)
* all'interno di un altro oggetto, che a sua volta è allocato in una delle aree precedenti.


### Costruzione

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

### Copia

Mediante il **costruttore di copia**, è possibile istanziare un oggetto a partire da un'altra istanza della stessa classe, di fatto clonando un oggetto:

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
CBuffer::CBuffer(const CBuffer& source){
	this->ptr = new char[size];
	memcpy(this->ptr, source.ptr, size);
}

//uso il costruttore di copia:
CBuffer buf1{50};
CBuffer buf2{buf1}; //copia, riceve come parametro un oggetto della stessa classe
```

Riceve un **riferimento costante**: si deve occupare di duplicare le singole variabili istanza dell'oggetto sorgente nell'oggetto corrente (this), ma NON deve modificare l'oggetto sorgente (ecco perchè `const`).

Per un UDT, se non è fornita l'implementazione, è generato dal compilatore, ivocando ricorsivamente la costruzione di copia di ogni campo presente all'interno dell'istanza originale.

Si può vietare la creazione di un duplicato dichiarando privato il costruttore di copia e richiedendone l'eliminazione nella dichiarazione della classe:

```
private CBuffer(const CBuffer& source) = delete;
```


### Movimento

Alcune delle variabili locali di un oggetto possono essere travasate in un altro oggetto **prima della distruzione**. 

Bisogna definire il **costruttore di movimento**:

```
ClassName(ClassName&& source); //il doppio && indica un candidato al movimento del tipo che lo precede, viene detto RValue reference
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

A differenza del costruttore di copia, **non è generato automaticamente dal compilatore per un UDT**. 

Se implementato, ogni volta che occorre copiare un valore, il compilatore valuta se effettuare la copia o il movimento.

Se si applica il movimento, il compilatore genera le seguenti pseudo chiamate:

```
Obj_MoveConstructor(dst, src);
Obj_Destructor(src);
```

Occorre quindi fare in modo che la chiamata al distruttore non elimini le risorse spostate! Ecco perchè occore impostare i puntatori a `nullptr`, altrimenti l'area di memoria puntata verrebbe deallocata.


La funzione `std::move()` permette di trasformare un oggetto generico in un riferimento rvalue, così da poterlo utilizzare nella costruzione o assegnazione per movimento.


### Distruttore

Ha il compito di rilasciare le risorse contenute in un oggetto. Se un oggetto non possiede risorse esterne o variabili allocate dinamicamente, non occorre dichiararlo. Non va mai chiamato esplicitamente, la chiamata sarà inserita nel programma dal compilatore quando opportuno.

Utile dichiararlo quando un oggetto incapsula puntatori a memoria dinamica o ad altre risorse del sistema. In questo modo le delete delle risorse create sono legate al ciclo di vita dell'oggetto:

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

Le variabili globali sono costruite prima dell'esecuzione del `main()` e sono distrutte dopo la sua terminazione.

Le variabili locali sono costruite all'ingreso in un blocco di codice (scope) definito tra graffe, e distrutte all'uscita.

Le variabili allocate dinamicamente sono allocate e deallocate nello Heap esplicitamente attraverso l'uso di `new` e `delete`.


## Allocazione dinamica

Dopo la chiamata a `delete`, è bene impostare il puntatore a NULL.

```
// allocazione dinamica di array di oggetti
ClassName* ptr = new[10] ClassName;
...
// deallocazione di array di oggetti
delete[] ptr; // vengono chiamati i distruttori di tutti gli oggetti
ptr = nullptr; //evita dangling pointer
```

Sebbene C++ consente l'utilizzo dei puntatori nativi, in associazione alle operazioni di `new` e `delete` **è bene non usarli**, utilizzando i tipi ad alto livello offerti dal C++: Array e Smart Pointers.


## Oggetti composti

in C++, un oggetto può contenere più oggetti in due differenti modi:

* come parte integrante della propria struttura dati: conviene se gli oggetti contenuti sono in quantità fissa e non troppo grandi (non si ottiene una struttura dati allocata troppo grande)

* facendo riferimento ad essi tramite puntatori: nel costruttore vengono allocati dinamicamente gli oggetti, nel distruttore devono essere deallocati esplicitamente.


## Passaggio di parametri

Può avvenire:

* **Per valore**: si passa una copia duplicata (nello stack), si basa sul costruttore di copia: è un'operazione onerosa. Da usare per tipi base od oggetti semplici.

```
void func(Buffer b){

	std::cout << buf.size;
}
```

* **Per indirizzo**: si passa il puntatore all'originale, può essere modificato. La funzione chiamata non può fare assunzioni sul tempo di vita dell'indirizzo.


```
void func(Buffer* b){

	std::cout << *buf.size;
}
```

* **Per riferimento**: si passa l'originale, sia lettura che scrittura. Sintatticamente ricorda un passaggio per valore, semanticamente corrisponde ad un passaggio per indirizzo che però **non può** essere mai NULL. 

```
void func(Buffer& b){

	std::cout << buf.size;
}
```

* Per **riferimento costante**: si passa l'originale, solo in lettura. Si ottiene precedendo la keyword `const` alla variabile passata.

```
void func(const Buffer& b){
	std::cout << buf.size;
	//buf.size = 0; //exception!
}
```

* Per **[riferimento RValue](https://smartbear.com/blog/develop/c11-tutorial-introducing-the-move-constructor-and/)**: ottimizzazione utilizzata per il passaggio di parametri in librerie. Permette al compilatore di muovere i dati contenuti all'interno del valore per costruirne, a minor prezzo, una copia. Se definito, usa il costruttore di movimento, altrimenti usa quello di copia. Per i tipi elementari è equivalente ad una copia.

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
return make_tuple<int,std::string>(status, myStr); //make_tuple è una funzione generica, accetta n parametri
```

## Copia ed Assegnazione

Se l'oggetto a sinistra dell'assegnazione esiste già, si tratta di una assegnazione. 

Se invece l'oggetto non esiste, allora si tratta di una copia: viene invocato il costruttore di copia.

```
Cpoint A, B;

A = B; //assegnazione, A esiste, viene invocata assegnazione
Cpoint C = B; //C non esiste, viene invocato costruttore di copia
Cpoint D(B); //D non esiste, viene invocata copia
```

Durante l'assegnazione di un oggetto, se devono essere assegnati attributi che sono puntatori, quello che fa il compilatore è copiare il puntatore sorgente in quello destinazione, così i due puntatori punteranno allo stesso oggetto/area di memoria (male) e l'area puntata dal puntatore destinazione non sarà più accesibile (male, memory leakage).

Per questo motivo, occorre sempre **ridefinire l'operatore di assegnazione per la classe, se essa contiene puntatori o riferimenti a memoria allocata dinamicamente**:

```
CBuffer& operator=(const CBuffer& source){

	if (this != &source){ //protezione

		//fa il lavoro del distruttore
		delete[] this->ptr;
		this->ptr = null; //in caso di eccezione della successiva new, il distruttore farebbe un'altra delete
		
		//assegna
		this->size = source.size;
		this->ptr = new char[size];
		memcpy(this->ptr, source.ptr, size);
	}

	return *this;
}
```

**L'operatore di assegnazione di sovrappone al distruttore**, **deve** rilasciare tutte le risorse possedute e tutte le aree di memoria allocate dinamicamente dell'oggetto a cui si assegna il nuovo valore (this).

In ogni caso, **costruttore di copia ed operatore di assegnazione devono essere semanticamente equivalenti.**

**Regola dei tre**: se una classe dispone di una qualunque di queste funzioni membro: *costruttore di copia, operatore di assegnazione, distruttore*, allora **deve** implementare le altre due. In mancanza di ciò, il compilatore fornirà la propria implementazione, **non generando alcun errore o warning**, ma che sarà nella maggior parte dei casi errata.


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

### Il paradigma Copy&Swap

Permette di **risolvere le problematiche di copia ed assegnazione**, che sono soprattutto legate al verificarsi di eccezioni, ad esempio:

```
intArray& operator=(const intArray& that) {

  if (this!=&that) { //(1)
    delete mArray;
    mArray=NULL; //(2)
    mSize=that.mSize;
    mArray = new int[mSize]; //(3)
    std::copy(that.mArray, that.mArray+mSize, mArray);
  }

  return *this;
}
```

1. se si omette (1) si potrebbe assegnare un oggetto a se stesso.
2. se si omette (2) in caso di eccezione il distruttore rilascerebbe nuovamente la memoria allocata.
3. l'allocazione di memoria di (3) potrebbe lanciare un eccezione se non ci fosse abbastanza spazio disponibile nello heap, lasciando l'oggetto in uno stato corrotto.

Con il paradigma copy&swap:

1. si introduce la funzione `swap(this, that)` definendola come `friend` della classe (funzione che non è metodo della classe ma può accedere ai membri privati), si occupa di scambiare il contenuto delle risorse tra due istanze. Si appoggia sulla funzione `std::swap` che permette di scambiare riferimenti elementari.

2. Si definisce l'operatore di assegnazione con un parametro di tipo valore, i.e. `intArray& operator=(intArray that)`, in modo tale che la logica dell'operatore di copia provveda a fare un duplicato del parametro that, in caso di eccezione, this non viene modificato (l'eccessione si verificherebbe prima di effettuare lo swap, durante il tentativo di creare la copia that).

```
//definita fuori dalla classe, il prototipo va però aggiunto nella classe
friend void swap(intArray& a, intArray& b) {
  std::swap(a.mSize, b.mSize);
  std::swap(a.mArray, b.mArray);
}
...

// operatore di assegnazione con copy&swap
intArray& operator=(intArray that){
  //that è passato per valore, copiato o mosso
  swap(*this, that);
  return *this;
}

// costruttore di movimento con copy&swap
intArray(intArray&& that): mSize(0), mArray(NULL) {
  swap(*this, that);
}
```


# **Ereditarietà in C++**

Molto differente rispetto a java: **una classe può ereditare da una o più classi**. Non esiste una classe radice (Object per java).

I metodi della classe di base possono essere invocati usando l'operatore di scope `::`, i.e. `ClasseBase::NomeMetodo()`.

Si può ereditare sia in modo pubblico che privato:

* ereditarietà pubblica: `class Derived: public Base{}` rappresenta la relazione `is-a`
* ereditarietà privata: `class Derived: Base{}` rappresenta la relazione `has-a`

```
class Base {
	public:
		Base();
 		void baz();
		~Base() {}
};

class Der : public Base { //Der is-a Base

	public:
		//nota chiamata a costruttore classe base
		Der():Base(){ 
			Base::baz();
   		}
  		~Der() {}
};

Der *obj = new Der();
obj->baz();
delete obj;
```

# **Polimorfismo in C++**

Se B estende A, allora è possibile assegnare ad una variabile `A&` un oggetto B: sfruttabile solo quando si ha a che fare con puntatori, perchè la memoria allocata per A è sempre minore di quella allocata per B, non ci entrerebbe se allocato staticamente.

Override di metodi: per default, il compilatore utilizza l'implementazione definita nella classe a cui ritiene appartenga l'oggetto (basandosi sul tipo del riferimento e non sul dato puntato). Per forzare un metodo come polimorfico, occorre utilizzare la keyword `virtual`. In questo caso verrà chiamata l'implementazione corretta (quella del dato puntato). **Solo le funzioni dichiarate `virtual` sono polimorfiche**.
  
I **metodi virtuali astratti sono dichiarati "=0"**. 

Una **classe astratta** contiene almeno una funzione virtuale astratta. Una classe che contiene solo funzioni virtuali è equivalente ad una **interfaccia** java. Una classe astratta, che cioè contiene almeno un metodo puramente virtuale (dichiarato `=0` e non implementato) **non può essere istanziata**.

Anche **i distruttori non sono polimorfici** per default. È opportuno che tutte le classi che posseggono risorse ed espongono tali risorse con funzioni virtuali, abbiano il distruttore dichiarato virtuale. Se una classe con metodi virtuali non ha un distruttore virtuale, è possibile che venga chiamato il distruttore errato.

```
class Base {
	public:
		Base() {}
		virtual void foo() { … }
		virtual void bar() = 0;
		virtual ~Base() { … }
};

class Der : public Base {
	public:
		Der() : Base() { … }
		void foo(){       
  			Base::foo();
		}
 		void bar() { … }
  		~Der() { … }
};
```

Gli oggetti C++ non posseggono alcun campo che definisce il tipo della classe, per chiamare il metodo corretto, viene utilizzato il meccanismo della `V-table.`

I metodi virtual sono aggiunti nella `V-table` che è la tabella dei metodi virtuali. **Ogni classe che ha almeno un metodo virtuale ha una sua Virtual Table, creata in fase di compilazione**.  Contiene tante righe quanti sono i metodi virtuali degli oggetti creati di questa classe. 

Ogni volta che viene creato un oggetto di una classe che contiene almeno la dichiarazione o implentazione di un metodo virtuale, nella memoria allocata per l'oggetto si aggiunge il puntatore alla **V-table della classe**. 

Ogni entry della v-table contiene il puntatore all'implementazione più derivata possibile del corrispondente metodo virtuale. Se un metodo virtuale non è stato specializzato nella clasee derivata, la entry della virtual table punterà all'implementazione della classe base.

Questo significa che la chiamata ad un metodo virtuale costa un pò di più.

```
//lavoro con puntatori, posso utilizzare puntatore a classe base.
Base* b1 = new Der1(); //Base definisce f come virtual, Der1 lo implementa
Base* b2 = new Der2(); //Base definisce f come virtual, Der2 lo implementa (diverso da quello di Der1)

b1->f(); //il compilatore accede alla vtable di b1 da cui legge il puntatore ad f per Der1
```


## Typecast

### `static_cast<T>(p)`

Utile per salire nell'albero di ereditarietà (verso la classe base, upcast). Converte il valore di p **come noto al compilatore** rendendolo di tipo T, in caso di puntatori, quindi, non viene verificato il tipo dell'oggetto puntato, ma il tipo del puntatore.

```
Derivata *d=new Derivata();
Base1 *b1;
Base2 *b2;

b1= static_cast<Base1 *>(d);
b2= static_cast<Base2 *>(d);

/*
Attenzione! Anche se b1==d non è possibile scrivere:
b2=static_cast<base2*>(b1)
*/
```

Se si effettua un downcast (dalla classe base verso la classe derivata) il compilatore non segnala alcun errore, ma a runtime potrebbero verificarsi dei problemi.

Inoltre, nell'esempio precedente, anche se b1==d, fare il cast da b1 a b2 produrrebbe un errore (`b2 = static_cast<Base2*>(b1)`).


### `dynamic_cast<T>(p)`
 
Utile per scendere nell'albero di ereditarietà. **Effettua controllo di compatibilità a runtime** assicurando cast sicuri tra tipi di classi. Applicato ad un puntatore, ritorna 0 se il cast non è valido. Applicato ad un riferimento, genera un'eccezione. Permette di effettuare il downcast da una classe virtuale di base ad una derivata.

### `reinterpret_cast<T>(p)`

Analogo al cast c-style. Consente al compilatore di violare il sistema die tipi, convertento il dato p in un dato di tipo T.

### `const_cast<T>(p)`

Elimina la caratteristica di costante da p.


# **Gestione delle eccezioni nel C++**

Due tipologie di errori:

* **errori attesi**: occorre verificare se siano o meno presenti, gestendoli, se necessario propagandoli all'indietro.
* **errori inattesi**: possono verificarsi ovunque, è improbabile che siano gestibili nel posto in cui si sono verificati.

Le eccezioni permettono di trasferire il flusso di esecuzione da un punto in cui si verifica un errore, ad un punto precedente, in cui è possibile gestirlo. Meccanismo basato sullo **stack unwindig**.

Una eccezione è lanciata dalla clausola `throw new MyExc()`. Quando il sistema incontra throw, abbandona il normale flusso di elaborazione ed inizia la ricerca di una contromisura: un costrutto `catch` che gestisce l'eccezione generata.

```
try{

	if(something_went_wrong)
		throw new MyExc();

} catch(MyExc& exc) {
	std::cout << "something went wrong!" << std::endl;
}
```

La libreria standard offre `std::exception`, le sue sottoclassi possono essere estese per gestire eccezioni personalizzate, incapsulano una stringa accessibile dal metodo `what()` che serve per documentare l'errore (utilizzare lo stderr per stampare gli errori, `std::cerr`). NON usare questa stringa per discernere la tipologia di eccezione. Le clausole catch **utilizzano il tipo della classe per discernere le diverse eccezioni**: se la classe dell'eccezione coincide o deriva (is_a: estende pubblicamente un'altra eccezione) quella indicata, il codice corrispondente è eseguito e l'eccezione rientra.

**Stack unwinding**: quando si verifica un'eccezione, lo stack si contrae (srotola) fino al blocco try dove l'eccezione è catturata, questo comporta che **tutte le variabili locali presenti nello stack vengono distrutte attraverso la chiamata agli opportuni distruttori**. Se non viene individuato alcun blocco catch che gestisce l'eccezione, lo stack viene contratto completamente ed il programma termina.

In C++ esiste la notazione `catch(...)` per catturare qualunque tipo di eccezione.

chiamare `throw` dentro un blocco catch rilancia all'esterno l'eccezione catturata dal catch.

**Mai scrivere un blocco catch che non esegue nessuna strategia di riallineamento.**

Strategie di gestione:

* terminare il programma in modo ordinato, i.e. chiamando `exit(-1)`.
* ritentare l'esecuzione per n volte, evitando di entrare in un loop infinito, i.e. annegando il blocco try-catch in un `while(n)` e decrementando `n` nel catch.
* registrare un messaggio nel log e rilanciare l'eccezione.

**Pattern "Second Phase Construction"**: se potrebbe verificarsi un'eccezione nel costruttore, fare nel costruttore il minimo indispensabile (sicuri che non sorgano eccezioni) e poi fare l'inizializzazione in un metodo da chiamare, simile all' `onCreate()` di Android.

Il lancio di un'eccezione ha un costo abbastanza elevato, soprattutto se si tiene conto dell'esecuzione di tutti i distruttori.


## Resource Acquisition Is Initialization (RAII)

Uno dei paradigmi del C++: se devo utilizzare una risorsa, scrivo una classe in cui nel costruttore (inizializzazione) acquisisco la risorsa (i.e. apro file) e nel distruttore rilascio la risorsa. In questo modo il ciclo di vita dell'oggetto rispecchierà il ciclo di vita dell'uso della risorsa: quando il programma uscirà dallo scope in cui è stata inizializata la classe, il distruttore verrà chiamato e la risorsa rilasciata, lo stesso avverrà in caso di eccezione durante lo stack unwinding.


# **Funzioni ed operatori nel C++**

* **Operator overloading**:

per via del **principio di espressività** del C++, se è lecito effettuare operazioni (+,-,<<) tra tipi base, lo è anche tra gli oggetti UDT: per questi oggetti, è necessario ridefinire le operazioni base.

Non è possibile definire nuovi operatori, o cambiare le precedenze o l'arità (numero di argomenti dell'operatore) dell'operatore.

Operatori non ridefinibili:

* L'operatore ternario `? :`
* l'operatore di scope `::`
* l'operatore di accesso `.` 
* `sizeof`

Si può invece ridefinire:

1) subscript operator `[]`
2) **puntatore a funzione**: `<tipo-ritornato> (* myfunc) (<argomenti>)`, si può omettere l'asterisco

```
int f(int i, double d){
	//corpo
}

//definisco puntatore a funzione
int (*var)(int, double);
var = f; //inizializzo
var = &f; //identico al precedente
var(10, 3.14);
```

Si possono ridefinire gli **operatori di conversione**, valido sia per i tipi primitivi che gli UDT:

```
operator float() const {
	//..conversione del dato a float
}
```

## Oggetti funzionali o funtori

Istanza di una qualsiasi classe che abbia ridefinito la funzione membro `operator ()`

```
class FC{
	public:
		int operator() (int v){
			return v*2;
		}
}

FC fc;
int i = fc(5); // i = 10
```

Se voglio scrivere una funzione che può ricevere come parametro sia un oggetto funzionale che un puntatore a funzione, posso definirla generica: 

```
template <typename F>
void some_func(F& f) {
	f();
}
```

## Funzioni Lamba

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

Le  parentesi quadre `[]` identificano la notazione lambda.

Se si vogliono rendere disponibili delle variabili locali all'interno delle funzioni lambda, è possibile aggiungerli all'interno della notazione lambda (passati sia per riferimento che per valore) `[]`, i.e. `[var, &varRef]`. 

Se si usa la notazione `[&]` tutte le variabili definite nello scope in cui è definita la lambda vengono catturate per riferimento, eventuali cambiamenti effettuati dalla lambda su questi dati influenzeranno gli originali. **Nota che questi valori sono mantenuti, come per gli oggetti funzionali, come se fossero definiti static dentro la lambda**, questo perchè una funzione lambda viene trasformata dal compilatore in un oggetto funzionale!

Una funzione lambda che cattura dei valori (by value) viene detta **closure**, essa cattura al propio interno una copia dei valori catturati, rendendoli disponibili durante una successiva invocazione (come se fossero static), per default, i valori catturati sono immutabili. Si può aggiungere la keyword `mutable` dopo la lista dei parametri per renderli modificabili: Questo consente la creazione di funzioni dotate di stato, che modificano il proprio comportamento con il susseguirsi delle invocazioni.

```
{
	int k = 3
	auto f = [k] (int i) {return i*k}
}

// E' compilato in:
{
	int i;    

	//lambda compilata in oggetto funzionale
	class __lambda_6_13  {
		int i;
		public: inline int operator()(int v) const {
			return v + i;
		}    
		public: __lambda_6_13(int _i): i{_i}{}
	};

	__lambda_6_13 f = __lambda_6_13{i};
}
```

## Generalizzare l'accesso ad una funzione

La classe `std::function<R(Args...)>` permette di modellare una funzione che riceve argomenti di tipo Args e restituisce un valore di tipo R. A questo tipo possono essere assegnati oggetti funzionali, lambda, puntatori a funzione purchè la firma sia conforme a quella indicata.

L'implementazione sfrutta il polimorfismo, questo rende la chiamata alla funzione leggermente più costosa.

```
import <functional>

std::function<float(float,float)> test_fun;

test_fun = std::fmaxf;
test_fun = std::multiplies<float>();
int x = 1;
test_fun = [x](float a, float b) {return a*x*b;} //lambda 
test_fun = [x](auto a, auto b){return a*x*b;} //lambda generica
```

## I modificatori const e static

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


# **Programmazione generica**

### Funzioni Generiche

Si può definire una funzione in modo che operi su un tipo di dato non ancora precisato, queste funzioni vengono chiamate **template**:

```
template <typename T>
const T& max(const T& t1, const T& t2){
	return (t1 < t2 ? t2 : t1);
}

int i = max(19,29); // T-> int
double d = max<double>(2, 3.14); // forza scelta di T a double
```

La funzione sopra sul tipo generico T a patto che supporti:

* l'operatore "<"
* il construttore di copia, per derivare da una variabile temporanea a partire dal dato costante

Ogni volta che si usa la funzione, il compilatore determina quale tipo effettivo assegnare a "T". **I parametri sono passati come `const` proprio perchè devono essere valutabili in fase di compilazione**.

### Classi generiche 

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

In presenza di template, il processo di compilazione viene diviso in due fasi:

* **definizione del template**: viene creato un albero sintatticoo astratto corrispondente al codice generico.
* **istanziazione del template**: quando all'interno di altre parti del codice si trova un riferimento al template che indica un valore concreto per i tipi/valori generici, questo viene istanziato specializzando l'AST con i dati concreti, e poi si procede a generare il codice corrispondente.

E' possibile **specializzare un template** nel caso in cui per un determinato tipo/classe alcuni degli operatori utilizzati dal template non siano definiti per il tipo/classe:

```
class Person {
  std::string name;
public:
  Person(std::string n): name(n) {}
  std::string name() { return name; } 
};

//specializzo il template Accum per poter lavorare con il tipo Person dato che non ha definito l'operatore +=.
template <>
class Accum<Person> {
  int total;
public:
  Accum(int start = 0): total(start) {}
  int operator+=(const Person &) {return ++total;}
  int value() { return total;}
};
```

**Vantaggi dei template**:

* aumentano le possibilità espressive senza introdurre overhead durante l'esecuzione (a differenza del polimorfismo).
* risparmiano tempo di sviluppo
* garantiscono codice conforme al sistema dei tipi
* flessibilità a livello sintattico e semantico

**Problematiche dei template**:

* vanno verificate le assunzioni sui tipi effettivamente usati, eventuali casi particolari gestibili mediante la specializzazione.
* le violazioni sul tipo utilizzato generano errori di compilazione difficilmente interpretabili per via dell'espansione del template.


# **Smart Pointers**

La ridefinizione degli operatori permette di dare una semantica agli operatori `*` ed `->` anche a dati che non sono (solo) puntatori: ciò permette di creare oggetti che si utilizzano come puntatori ma offrono molto più funzionalità.

```
// sfrutta paradigma RAII per rilasciare memoria durante distruzione

template <class T> 
class smart_ptr  { 
private:
	T* ptr;
	smart_ptr(const smart_ptr<T>&);
	smart_ptr<T>& operator=(const smart_ptr<T>&);
public: 
	explicit smart_ptr(T* p = 0) : ptr(p) {}
	~smart_ptr() { delete ptr; } 
	T& operator*() { return *ptr; } 
	T* operator->() { return ptr; } 
}; 
```

I template per gli smart pointer sono disponibili tramite `#include <memory>`.

* **`std::unique_ptr<BaseType>`**

**Impedisce la copia e l'assegnazione**, ma **permette il movimento esplicito** utilizzando la funzione `std::move()`. Implementa il concetto di **proprietà esclusiva**. Se utilizzato per puntatore ad array, bisogna utilizzare `<BaseType[]>`. 

Ha gli stessi tempi di esecuzione e la stessa occupazione di memoria di un puntatore nativo. Usi tipici sono per garantire la distruzione in caso di eccezioni (RAII), acquisire o passare la proprietà di un oggetto con un ciclo di vita dinamico tra funzioni, gestire in modo sicuro oggetti polimorfici.


* **`std::shared_ptr<BaseType>`**

Permette **proprietà condivisa** con un meccanismo di **conteggio dei riferimenti**: molti oggetti possono referenziare il blocco di memoria puntato, quando l'ultimo viene distrutto, il blocco viene rilasciato. Utilizza più risorse di un puntatore nativo. 


Il costruttore `shared_ptr<T>(T* native_ptr)` permette di costruire uno shared ptr che incapsula il puntatore nativo. Il distruttore decrementa il contatore dei riferimenti e libera la memoria posseduta quando il contatore raggiunge zero. La classe ridefinisce l'operatore di assegnazione `=`, si può utilizzare per **assegnare il puntatore condiviso ad un'altro shared ptr, incrementando il contatore dei riferimenti**.

La funzione `std::make_shared<BaseType>(Args...)` viene usata per creare un oggetto di tipo BaseType, passando al suo costruttore gli argomenti `Args...`, e ritornarne uno shared pointer all'oggetto.

Ogni shared_ptr mantiene due puntatori:

* uno all'oggetto puntato
* e l'altro ad un **blocco di controllo**

Il blocco di controllo è allocato dinamicamente e mantiene il distruttore, l'allocatore, il contatore di shared_ptr che condividono la proprietà dell'oggetto, il contatore di weak_ptr che fanno riferimento all'oggetto.

**Cast tra shared_ptr**, sono disponibili le funzioni:

```
std::shared_ptr<T> static_pointer_cast<T>(shared_ptr<U>)
std::shared_ptr<T> dynamic_pointer_cast<T>(shared_ptr<U>) //usare per scendere in albero ereditarietà (i.e. da classe puntatore ad oggetto classe base voglio quello della classe derivata, se non possibile ritorna nullptr)
std::shared_ptr<T> const_pointer_cast<T>(shared_ptr<U>)
std::shared_ptr<T> reinterpret_pointer_cast<T>(shared_ptr<U>)
```

Permettono di creare uno shared_ptr a una classe base e farne il cast a uno shared_ptr di classe derivata.

In alcuni casi si possono formare dipendenze cicliche che non permettono il rilascio della memoria, i.e. se A->B e B->A. Occorre che il programmatore eviti la creazione di cicli utilizzando i weak_ptr.

* **`std::weak_ptr<BaseType>`**

Permette di osservare il contenuto di uno shared_ptr senza partecipare al conteggi dei riferimenti, modella il **possesso temporaneo**. Offre i metodi `lock()` ed `expired()` per la sua gestione.

* `lock()` restituisce uno shared_ptr che permette l'accesso al blocco puntato
* `expired()` permette di verificare la validità del riferimento.

```
std::weak_ptr<int> gw;

void f(){
	if (auto spt = gw.lock()) //spt partecipa al conteggio, ma verrà distrutto
		std::cout<<"gw:"<< *spt << "\n";
	else 
		std::cout<< "gw e' scaduto\n";
}

int main() {
	{ 
		auto sp = std::make_shared<int>(42);
		gw = sp; //gw guarda l'area puntata da sp
		f();
	}  // sp viene distrutto, gw scade
	f();
}
```

Per tutti i puntatori, il metodo `reset()` permette di rilasciare il puntatore senza eliminare l'oggetto pointer, in modo tale da riutilizzarlo, l'area di memoria puntata viene rilasciata.


# **Input/Output in C++**

Oltre alle funzioni della libreria `stdio.h` come printf/scanf, il C++ offre un'alternativa per la gestione dei flussi di input/output, basata su una gerarchia di classi per la scrittura/lettura di stream:

`ios` è la classe base, virtuale, contiene attributi e metodi comuni a tutti gli stream. 

`istream` ed `ostream` derivano da `ios` e definiscono metodi per input ed output su stream generici, ridefiniscono anche gli operatori `<<` e `>>`. Consentono accesso indiretto allo stream tramite il buffer interno.

`iostream` esegue operazioni di lettura e scrittura.

## Operazioni sui file

Le classi base sono `ifstream` ed `ofstream` per l'accesso rispettivamente in lettura e scrittura, derivano indirettamente da `ios` e contengono metodi per la creazione dello stream (`open()`, `close()`).

`fstream` eredita da entrambe e permette accesso sia in lettura che scrittura.

Le operazioni di IO **non sollevano eccezioni**: va testato esplicitamente il risultato di ogni operazione effettuata. Le funzioni `eof()`, `fail()`, `bad()`, `good()` chiamate sull'oggetto stream permettono di discernere la condizione che si è verificata in seguito all'ultima operazione di IO:

* `eof()`: true se è stato incontrato EOF.
* `fail()`: true se è avvenuto un errore di formato, ma che non ha causato la perdita di dati
* `bad()`: true se c'è stato un errore che ha causato una perdita di dati
* `good()`: true se non c'è stato alcun errore 

In uno stato bad, tutte le successive operazioni di lettura/scrittura falliscono silenziosamente. Il metodo `clear()` invocato sullo stream ripristina lo stato del flusso: chiamare questo metodo dopo aver sistemato lo stream in seguito ad un fail/bad.



# **Standard Template Library (STL)**

Libreria basata su tre componenti: **contenitori**, **algoritmi** ed **iteratori**. La libreria è implementata attraverso l'uso di template, rendola quindi generica (polimorfismo a compile-time).

* container: strutture dati.
* algoritmi: applicabili ai container, operano su un range di elementi.
* iteratori: puntatori generalizzati che connettono container e algoritmi.

## Container

Tre tipologie:

* **Sequenziali**: organizzano linearmente una collezzione di oggetti, e.g.: Array, vector, deque, list, forward_list.

* **Associativi**: contenitori non lineari, organizzati in coppie chiave-valore, accesso agli oggetti tramite chiave. 
  Ordinati: set, map, multiset, multimap. 
	Non ordinati: unordered_set, unordered_map, unordered_multiset, unordered_multimap.

* **Container adapter**: sono contenitori sequenziali specializzati,e.g.: stack (LIFO), queue (FIFO), priority_queue. **NON supportano gli iteratori**.

In C++ per ogni tipo UDT utilizzato nei contenitori associativi non ordinati va specializzato il template `hash<T>`, classe generica di cui specializzare la ridefinizione dell'operatore `()`. 

## Contenitori Sequenziali

### std::array

Usa internamente un array a dimensione fissa allocato staticamente. Non fornisce metodi per rimuovere gli elementi.

### std::vector

usa internamente un array allocato dinamicamente.

Elementi accesibili non solo con iteratori ma anche con puntatori tradizionali + offset (memoria è allocata contiguamente). Può espandersi e contrarsi a seconda del bisogno. `capacity` e `size` ritornano rispettivamente lo spazio allocato per il vettore e la dimensione del vettore.

* std::string equivale a `std::vector<char>`

std::string non fa parte dei contenitori della STL! 	
  

## Contenitori associativi

I dati sono accessibili non per posizione, ma per chiave. Supportano iteratori bidirezionali (elemento successivo - elemento precedente). 

* **set, multiset, map, multimap**: mantengono gli oggetti **ordinati per chiave**, la chiave deve supportare l'operatore `<`, copiabile e movibile. Tempo di accesso `O(log(n))`. 
  
* **unordered_set, unordered_multiset, unordered_map, unordered_multimap**: oggetti **indicizzati per hash**, tempo di accesso costate. Non sono presenti elementi duplicati (set). La chiave deve essere equal comparable, copiabile o movibile, e **disponibile come valore hash**. Il valore deve avere un costruttore di default e deve essere copiabile e movibile.

multimap e multiset di entrambi i tipi ammettono chiavi duplicate.

Per le mappe, `pair<KeyType,ValueType> std::make_pair<KeyType, ValueType>(key, value)` permette di creare una `pair` da inserire: pair è un caso particolare di tupla, una coppia di elementi eterogenei, si accede agli elementi con `pair.first` e `pair.second`.


 ## Interfaccia comune per i Container

 Nonostante esistano diversi tipi di container, essi hanno molto in comune, molte operazioni non dipendono dal tipo di container:

 * creazione, cancellazione, accesso e swap di elementi.
 * accessibilità attraverso iterator.
 * tranne std::array, ogni container ha un allocatore che lavora in background, volendo, si può fornire un allocator personalizzato invece di quello di default. L'allocator permette di espandere e contrarre dinamicamente il container.

Ad esempio, per qualunque tipo di container si possono invocare i metodi:

* `clear()`  ne cancella tutti gli elementi.
* `size()` ritorna il numero di elementi
* `empty` verifica che sia vuoto
* `begin()` ed `end()` forniscono coppia di iteratori che puntano all'inizio/fine del container.
* `swap(cont2)` scambia gli elementi dei due container.

I container supportano il confronto: **vengono confrontate le chiavi**.

```
vector<int> v1{1,2,3,4};
vector<int> v2{1,2,3,4};
count << (v1==v2) <<endl; //true
std::swap(v1,v2) // oppure v1.swap(v2)
for (auto v: v1) std::cout << v << " ";
```

## Iteratori

Sono il collante tra contenitori ed algoritmi. Supportano gli operatori:

* `*`, ritorna l'elemento alla posizione corrente dell'iterator
* `==`, `!=`, confronta due posizioni
* `=`, assegna un nuovo valore all'iterator

Sono implementati come una generalizzazione dei puntatori, soffrono dei stessi problemi: non ne viene controllata la validità prima di accedere all'area di memoria puntata.

```
class c;
vector<C> v(10);
vector<C>::iterator iter = v.begin();
vector<C>::iterator end = v.end();

for(; iter!=end; iter++){ C elem = *iter;}
```

La classe dell'iteratore definisce le operazioni necessarie per lavorare con il contenitore utilizzando l'iteratore: il compilatore sceglie l'implementazione corretta.

**Per ogni contenitore esiste un tipo di iteratore** che implementa la corretta semantica delle operazioni.

Quando l'iteratore lavora sulla collezione, la collezione non può essere modificata.

Tipologie di iteratori:

* forward iterator
* bidirectional iterator
* random access iterator

Per ottenere un iteratore oltre a `container.begin()` e `container.end()` si possono usare:

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

* `std::stack`: offre i metodi `push()` e `pop()` per inserire/rimuovere elementi in cima, `top()` ritorna un riferimento all'oggetto in cima. Supportano gli operatori di confronto. complessità O(1).
* `std::queue`: push e pop per inserimento/rimozione, front e back per accesso ad inizio e fine coda. Complessità O(1).
* `std::priority_queue`: un coda a priorità, la priorità è gestita attraverso l'operatore di confronto `std::less`.

Non possono essere usati con gli algoritmi della STL, supportano un'interfaccia ridotta rispetto agli altri container. Non supportano gli iteratori, conoscono la loro dimensione.


## Algoritmi della STL

STL fornisce algoritmi per lavorare sui container ed i loro elementi: sono implementati come funzioni template e sono indipendenti dal tipo di elementi del container. L'algoritmo viene applicato in loop ad un intervallo di dati.

* in `<numeric>` si trovano algoritmi numerici come accumulate (che non lavora solo sui numeri), inner_product, adjacent_difference...

* in `<algorithm>` sono raccolti algoritmi per ricerca e ricerca binaria, trasformazione dei dati, partizionamento, ordinamento, merge, operazioni insiemistiche, operazioni su heap, comparazioni lessicografiche...

**Algoritmi non modificanti**: applicano funzioni agli elementi, modificandoli, senza che però venga modificata la struttura del container: 

* all_of, any_of, none_of: verificano se un predicato sia vero per tutti, qualcuno o nessuno degli elementi del container.
* for_each, for_each_n, applicano una funzione ad un range di elementi.
* find, find_if, find_if_not: trovano il primo elemento che soddisfa un criterio.

**Algoritmi modificanti**: algoritmi che possono modificare la struttura di un container:

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

Per default l'ordinamento è ascendente.

* sort, stable_sort
* partial_sort
* is_sorted
* is_sorted_until

**Algoritmi per ricerca binaria**:

* lower_bound
* upper_bound
* binary_search
* equal_range

## Execution policy

La maggior parte degli algoritmi della STL accetta come primo parametro una delle execution policy definite in `<execution>`, permettono l'esecuzione sequenziale, parallela o parallela con vettorizzazione.

* `std::execution::seq` esecuzione sequenziale
* `std::execution::par` esecuzione parallela con multithreading (genera un numero di thread pari a quelli del processore)
* `std::execution::par_unseq` esecuzione vettorizzata con SIMD (per GPU: single instruction (algorithm), multiple data)

Questo policy non implementano alcun controllo su data race o deadlock.



# **Librerie**

Compilazione e linking:

* il programma è suddiviso in un insieme di file sorgenti `.h` e `.cpp`
* il **compilatore** compila i file sorgenti producendo dei moduli oggetto che contengono riferimenti pendenti alle funzioni/variabili degli altri moduli o delle librerie
* il **linker** unisce più moduli oggetto in un eseguibile finale, risolvendo i simboli esterni e sostituendoli con gli opportuni indirizzi.

Le **librerie sono insiemi di moduli** oggetto archiviati in un unico file. Le funzioni e le variabili esportate dalla libreria sono dichiarate in un file header, i cui simboli sono preceduti dal modificatore `extern`.

Processo di compilazione in dettaglio:

* analisi lessicale (pre-processor)
* analisi sintattica, produce un Abstract Syntax Tree 
* analisi semantica, decora l'AST
* istanziazione dei template
* compilazione dei sorgenti .h e .cpp per generare i **moduli oggetto**.
* il linker unisce più moduli oggetto, comprese le librerie, in un eseguibile finale. Se è necessario utilizzare librerie esterne, va aggiunto il flag `-l<nomelibreria>`, i.e. per la libreria matematica: `-lm`. Oltre al nome della libreria va anche data la lista di directory nelle quali cercare le librerie con l'opzione `-L<>`, questa opzione fa ovverride del contenuto della variabile d'ambiente `LIBPATH`.



## Caricamento delle librerie

L'uso di una libreria richiede **due fasi**:

1. identificazione dei moduli necessari e loro caricamento in memoria
2. aggiornamento degli indirizzi delle chiamate alle funzioni per puntare correttamente ai moduli caricati

Le due fasi possono essere eseguite:

* dal **linker** durante il collegamento: **static linking**.
* dal **loader** durante il caricamento del programma, in questo caso si caricano le librerie dinamiche (condivise) con estensione su win/*nix: `.dll` / `.so`
* dal **programma stesso** durante l'esecuzione, chiamato **dynamic linking**.


## Tassonomia delle librerie

### Librerie a collegamento statico

Contengono funzionalità collegate staticamente al codice binario del programma in fase di compilazione. **Una libreria statica è un file archivio che contiene un insieme di moduli oggetto**, è creata dall'`archiver`. In linux hanno estensione `.a`, i.e.: `libm.a`. In windows hanno estensione `.lib`.

Il linker identifica in quali moduli della libreria si trovano le funzioni richiamate nel programma, e li carica nell'eseguibile. Terminata questa fase, i moduli ed i simboli della libreria statica risultano annegati nel codice binario del programma compilato.

**Vantaggi**: non ci sono dubbi su versione adottata della libreria, portabilità.

**Svantaggi**: stesse librerie presenti in processi differenti occupano memoria inutulmente, applicazione va ricompilata ad ogni modifica della libreria.

### Librerie a collegamento dinamico

Il file eseguibile non contiene i moduli della libreria, sono caricati successivamente (dal loader) nello spazio di indirizzamento del processo. 

All'atto della creazione del processo, il loader mappa nello spazio di indirizzamento virtuale del nuovo processo tutte le librerie condivise, risolvendo i simboli corrispondenti presenti nel programma.

In linux il dynamic linker è la libreria condivisa `ld.so`, non richiede il caricamento di altre librerie, altrimenti non funzionerebbe. In windows il loader è parte del kernel.

E' possibile postporre la fase di caricamento delle librerie dal loading all'esecuzione, l'applicazione in questo caso si avvia in assenza di qualsiasi liberia e ne fa richiesta quando necessario (incontra la prima chiamata ad una funzione di libreria).

### Dynamic Linking in Linux

Linux espone le `Dynamic Loading API` per gestire il caricamento ritardato in fase di running, alcune delle funzioni della API sono:

* `dlopen()`, rende il file oggetto di una libreria accesibile al programma
* `dlsym()` recupera l'indirizzo di un simbolo (variabile o funzione).
* `dlerror()` ritorna l'ultimo errore occorso.
* `dlclose()` chiude il file oggetto aperto.

### Dynamic Linking in Windows

Le librerie sono fornite in formato `PE2` con estensione `.dll`: contengono una funzione di ingresso `DllMain()`, oltre a funzioni, variabili globali, costanti, risorse.

La funzione `LoadLibrary()` carica la libreria indicata e la mappa nello spazio di indirizzamento del processo: ritorna la handle del modulo caricato.

Per esportare e importare funzioni e strutture dati dalla dll:

1. direttive `dllexport` e `dllimport`
2. creare un file module definition `.def`

* `__declspec(dllimport)` per importare simboli pubblici della DLL
* `__declspec(dllexport)` per esportare simboli pubblici dalla DLL e renderli accessibili


```
// SampleDLL.h
#ifdef EXPORTING_DLL
extern __declspec(dllexport) void HelloWorld();
#else
extern __declspec(dllimport) void HelloWorld();
#endif


// SampleDLL.c
#define EXPORTING_DLL
#include "sampleDLL.h"
BOOL APIENTRY DllMain(…)
void HelloWorld() {printf("Hello world");}


// Static_Dll_usage.c 
#include "sampleDLL.h"
void someMethod() { 	
	HelloWorld();
}


// Dynamic_Dll_usage.c
HINSTANCE hDLL = LoadLibrary(L"sampleDLL.dll");
if (hDLL != -1)
{
	DLLPROC Hw = (DLLPROC) GetProcAddress(hDLL,L"HelloWorld");  	
	if (Hw != NULL) (*Hw)();
	FreeLibrary(hDLL);
}
```


# **Programmazione concorrente**

Un programma concorrente dispone di due o più flussi di esecuzione contemporanei che vivono nello stesso spazio di indirizzamento. All'atto della creazione un processo dispone di un solo flusso di esecuzione, il thread principale.

Vantaggi dell'uso di più thread:

* computazione ed I/O possono sovrapporsi
* Più efficienti rispetto ad usare più processi (no overhead da IPC, meno dati replicati)
* sfruttano il simultaneus multi threading delle CPU moderne

Svantaggi:

* aumento della complessità del programma
* gestione della sincronizzazione prona ad errori se implementata male
* difficoltà di debug

La libreria C++ per l'uso dei thread astrae le implementazioni dei thread nativi in windows e linux, permettendo di scrivere un'implementazione multi piattaforma di un programma concorrente.

Problema dell'accesso a variabili condivise in memoria: la presenza di cache hw ed il possibile riordinamento delle istruzioni da parte delle CPU con esecuzione speculativa ed out-of-order rende non determiniscono il risultato dell'accesso in r/w alla stessa locazione di memoria da parte di due thread. occorre utilizzare costrutti di sincronizzazione per definire esplicitamente l'ordine di esecuzione. Fanno eccezione:

1. l'accesso a elementi distinti di uno stesso contenitore
2. l'uso dei character stream `cin`, `cout` e `cerr`.

## Thread e memoria

Ogni Thread dispone di:

* un proprio **stack** delle chiamate
* un proprio **stack pointer** e **program counter**
* un proprio **puntatore all'ultimo contesto per la gestione delle eccezioni** (puntatore al catch block).
* lo **stato del proprio "processore virtuale"**: valori dei registri, modalità di esecuzione, ecc...

I Thread dello stesso processo condividono:

* le **variabili globali**
* l'area in cui è memorizzato il codice (**code segment**)
* l'area delle **costanti**
* lo **heap**

## Thread nativi dei SO

I SO implementano la gestione dei thread ognuno in modo diverso, attraverso API dedicate.

### Thread in Windows

Si può creare un thread con la funzione `CreateThread()`, il cui prototipo è:

```
HANDLE WINAPI CreateThread(
  LPSECURITY_ATTRIBUTES	lpThreadAttributes, //contesto di sicurezza in cui il thread sarà eseguito
  SIZE_T 	dwStackSize, // dimensione dello stack
  LPTHREAD_START_ROUTINE	lpStartAddress, //funzione che dovrà eseguire
  LPVOID	lpParameter, //parametro passato alla funzione
  DWORD	dwCreationFlags, //se deve essere runnable o sospeso
  LPDWORD	lpThreadId // dove salvare il thread ID
);
```

ritorna un handle che permette di fare riferimento al thread nelle altre funzioni del SO. Windows crea un **oggetto kernel a cui associa tutti i parametri passati alla CreateThread**.

Con `HANDLE GetCurrentThread()` si può ottenere un riferimento opaco al thread in esecuzione.

Si può terminare un thread in Windows:

* aspettando che la funzione mandata in running dal thread ritorni
* invocando `ExitThread()`
* se un altro thread del processo invoca `TerminateThread()`

In C++, è opportuno evitare di chiamare `ExitThread()` o `ExitProcess()`, dato che i **distruttori degli oggetti allocati nello stack dei thread e quelli delle variabili globali NON verrebbero eseguiti**. Per evitare ciò, si può ricorrere all'utilizzo di una variabile condivisa che segnali l'avvenuta distruzione di tutte le strutture dati prima di permettere di procedere con l'uscita del thread/processo.

Quando un thread termina, l'oggetto kernel corrispondente passa nello stato `signaled`. Un thread/processo può rimanere in attesa che tale condizione si verifichi senza sprecare CPU, attraverso l'uso delle funzioni delle API windows:

* `WaitForSingleObject()`
* `WaitForMultipleObject()`


### Thread in Linux

La libreria `pthread` (POSIX Thread) è la libreria standard per la gestione dei thread in ambienti posix compliant.

Tipi e funzioni definite:

* `pthread_t`, identificatore univoco di un thread in un processo.
* `pthread_t pthread_self()`, identifica il thread corrente.
* `int pthread_equal(t1, t2)`, confronta due identificatori.

* `int pthread_create()`, permette di creare un thread
* `pthread_join()`, permette di attendere la terminazione di un thread ed ottenere un valore ritornato.



## Thread del C++11: libreria std::thread e sincronizzazione

In C++11 è stato reso il concetto di thread parte integrande del linguaggio, rendendo semplice la scrittura di programmi portabili tra piattaforma che offrono librerie di basso livello per la gestione dei thread implementativamente diverse tra loro.

Due approcci per la scrittura di programmi concorrenti in C++11:

* Uno di alto livello, basato su `std::async` e la promise `std::future`
  
* Uno di basso livello, che richiede l'uso esplicito di thread e costrutti di sincronizzazione, implementato dalla classe `std::thread`.


### La classe `std::thread`

modella un oggetto che rappresenta un singolo thread di esecuzione del sistema operativo (incapsula un riferimento al thread del SO).

L'esecuzione dell'oggetto **callable** (i.e. funzione, lambda, oggetto funzionale) argomento del costruttore del thread inizia **immediatamente** dopo la costruzione dell'oggetto thread. Al termine della funzione, il thread viene distrutto e cessa di esistere.

```
#include <thread>
void f() {
  std::cout << "Up & Running!" << std::endl;
}

int main() {
  std::thread t(f); // t creato ed inizia esecuzione di f
  t.join();  // blocca thread principale attendendo terminazione di f
}
```

Altri parametri oltre al callable vengono inoltrati all'oggetto callable attraverso la funzione `std::forward` che inoltra un riferimento al dato originale o un riferimento rvalue, in funzione del tipo di dato passato (lvalue nel primo caso, rvalue nel secondo). 

Per passare un dato come riferimento (i.e. per **ottenere valori di ritorno**, dato che la funzione callable deve essere void) e non come una copia temporanea occorre incapsularlo in un  oggetto di tipo `std::reference_wrapper<T>` tramite l'uso delle funzioni definite in `<functional>`: `std::ref()` ed `std::cref()`. 

Nota bene: i valori sono passati come **riferimento** e non come indirizzo: se passo un parametro `T` nella callable il tipo sarà `T&`.

**Problema**: l'oggetto passato tramite riferimento al thread deve avere un ciclo di vita maggiore di quello del thread.



Gestione del thread una volta creato:

1. il metodo `join()` della classe thread permette di far bloccare il chiamante in attesa della terminazione del thread su cui si fa join.

2. Il metodo `detach()` consente di far andare il thread senza che il thread principale abbia più conoscenza dello stato di avanzamento del thread sganciato.

3. travasare le informazioni contenute nell'oggetto thread in un altro oggetto attraverso l'operazione di movimento

Se nessuna delle operazioni precedenti viene effettuata, e si distrugge l'oggetto thread (i.e. uscita dallo scope dove è stato creato), **l'intero programma viene terminato**, invocando `std::terminate()`. 

Se il thread principale di un programma termina, tutti i thread secondari ancora esistenti **termineranno improvvisamente**: mai dimenticare di chiamare join o detach, anche in caso di eccezione, una soluzione semplice sfruttando il paradigma **RAII** consiste nell'utilizzo di una **classe che chiami join() nel distruttore**:

```
#include <thread>

class thread_guard {
  std::thread& t;

public:
  thread_guard(std::thread& t_): t(t_) {}

  ~thread_guard() {
    if(t.joinable()) t.join(); 
  }

  thread_guard(thread_guard const&)=delete;
  thread_guard& operator=(thread_guard const&)=delete;
};

void f() {
  std::thread t(...);
  thread_guard g(t); // ha lo stesso ciclo di vita di t
}
```

Se al thread viene passato come oggetto Callable una funzione membro di una classe, occorre utilizzare questa sintassi, aggiungendo l'oggetto `this`:

```
class MyClass
{
    void FunctA();
    void FunctB();

    void run()
    {
        std::thread t(&MyClass::FunctA, this);
        std::thread r(&MyClass::FunctB, this);
    }
};
```

### Costrutti di sincronizzazione

Definiti nell'header `<mutex>`.

1. **`std::mutex`**

	Gli oggetti di questa classe pemettono l'accesso controllato a porzioni di codice a un solo thread alla volta.

	Offre i metodi di sincronizzazione: `lock()` ed `unlock()`. Lock consente di bloccare il thread in attesa di un unlock: se non ben gestito il thread può rimanere bloccato per sempre (deadlock).

2. **`std::recursive_mutex`**

	Può essere acquisito più volte consecutivamente dallo stesso thread, dovrà essere rilasciato altrettante volte.

3. **`std::timed_mutex`**

	Aggiunge i metodi `try_lock_for()` e `try_lock_until()`, pone un limite di tempo all'attesa massima.

4. **`std::recursive_timed_mutex`**: sia ricorsivo che temporizzato.

Un mutex va sempre rilasciato: la classe generica `std::lock_guard<Lockable>` utilizza il paradigma RAII per garantire che un mutex venga sempre rilasciato. Il costruttore invoca il metodo `lock()` dell'oggetto passato come parametro, il distruttore invoca `unlock()`.

Per evitare che il thread si blocchi sul mutex, la classe mutex mette a disposizione il metodo `try_lock()`: restituisce un booleano per indicare se è stato possibile acquisirlo o meno. Se l'acquisizione ha avuto successo, il mutex può essere adottato da un oggetto lock_guard così da gestirne il rilascio al termine dell'utilizzo.

```
std::mutex m;

void someFunction() {

   // acquisizione del lock
  while (m.try_lock() == false) {
    do_some_work(); //while waiting, instead this is busy wait
  }
  std::lock_guard<std::mutex> lg(m, std::adopt_lock); //lg registra m al proprio interno, senza cercare di acquisirlo

  //do stuff

  //chiamata ai distruttori, lg rilascia m
}
```

`std::scoped_lock<Lockable>` è stato introdotto come evoluzione RAII di `std::lock`. E' equivalente a lock_guard ma è un template con un numero variabile di parametri di tipo Lockable accettati, il lock viene acquisito su tutti i Lockable passati al momento della creazione, e rilasciato alla distruzione. Se al lock dell'n-esimo mutex passato viene lanciata un'eccezione, tutti i lock già riusciti vengono rilasciati. **Risolve il problema del deadlock**.


`std::unique_lock<Locable>` estende il comportamento di lock_guard: offre gli stessi metodi lock e unlock, il costruttore offre numerose politiche di gestione selezionate in base al secondo parametro: 

* `adopt_lock` verifica che il thread possieda già il Lockable passato come parametro e lo adotta.

* `defer_lock` si limita a registrare il riferimento al Lockable senza cercare di acquisirlo.


Oltre ai quattro tipi di mutex visti sopra, ne esiste un quinto per la gestione della sincronizzazione per oggetti che possono essere accedduti in lettura in modo condiviso, ma scritti in modo esclusivo:  

5. **`std::shared_mutex`**
   
	 E' una primitiva di sincronizzazione che permette due forme di accesso: condiviso ed esclusivo. mentre la regione è posseduta in modo condiviso, eventuali richieste di accesso esclusive sono messe in attesa, richieste di accesso condiviso sono garantite subito. Utile per distinguere accessi in lettura, che possono essere concorrenti, da accessi in scrittura, che devono essere esclusivi.

	```
	class MyCounter
	{ public:
  	  int get() { //multiple thread can read
    	  std::shared_lock sl(sm);
      	  return counter;
    	}
    	void increment() { //only one can modify at a time
      	  std::unique_lock ul(sm);
       	  counter++;
    	}
    	void reset() {
      	  std::unique_lock ul(sm);
          counter = 0;
    	}
	private:
  	  std::shared_mutex sm;
      int counter = 0;
	};
	```


## Sincronizzazione ad alto livello in C++11

`std::async` ed `std::future` definite nell'header `<future>` permettono di implementare algoritmi paralleli in cui **ogni thread opera su sotto compiti indipendenti**:

* la computazione di uno non dipende da quella di un altro
* non vi è accesso in scrittura a dati condivisi


### `std::async()`

`std::async()` accetta un parametro Callable che restituisce un oggetto di tipo `<T>` ed eventuali parametri da passare all'oggetto chiamabile, restituisce un oggetto di tipo `std::future<T>`. 

Quando viene eseguita async, se possibie, invoca in un thread separato l'oggetto Callable con i relativi parametri. Altrimenti il task viene eseguito **in modo sincrono** al momento della chiamata a `wait()` o `get()` sull'oggetto future.

In ogni caso, `async()` **ritorna immediatamente**, senza attendere il completamento della funzione chiamata: di fatto, sgancia il thread, se può, e ritorna.

Si accede al risultato dell'esecuzione attraverso il metodo `get()` chiamato sull'oggetto future ritornato:

* se l'esecuzione è andata a buon fine ritorna il risultato

* se il thread secondario è terminato con un eccezione rilancia l'eccezione nel thread corrente

* se l'esecuzione è ancora in corso, **blocca il thread che chiama get** in attesa che il thread sganciato finisca

* se l'esecuzione non è ancora cominciata, ne forza l'avvio sincrono (nel thread corrente).

`std::async()` consente di precedere l'oggetto chiamabile con una politica di lancio:

* `std::launch::async` attiva un thread secondario, lancia eccezione `system_error` se il multithreading non è supportato o non ci sono risorse disponibili per crare nuovo thread (ogni thread consuma all'incirca 2MB di stack).

* `std::launch::deferred` comunica che l'oggetto chiamabile dovrà essere valutato solo se e quando qualcuno chiamerà `get()` o `wait()`, di fatto non genera alcun thread aggiuntivo, l'oggetto Callable verrà eseguito dal thread corrente.

Se la politica viene omessa, dapprima viene provata la politica async, se non è possibile, l'attività viene segnata come deferred.

```
#include <future>
#include <string>

std::string f1(std:string p1, double p2) { ... }
std::string f2(int p) { ... }

// calcola f1("mystring", 3.14) + f2(18)
int main() {
	std::future<std::string> future1;
	try{
		 future1 = std::async(std::launch::async, f1, "mystring", 3.14); //calcola su thread secondario
	}catch(system_error& exc){
		exit(0);
	}
  std::string res2 = f2(18);  //calcola sul thread principale
  std::string res1 = future1.get(); //blocca in attesa di risultato, se thread secondario non ha terminato
  std::string result = res1+res2;
}
```

### `std::future<T>`

Consente di accedere in modo sicuro e ordinato al risultato di un'operazione asincrona. Il tipo T rappresenta il tipo del risultato ritornato dall'oggetto Callable. Mantiene internamente uno stato condiviso con l'oggetto Callable passato ad async, quando viene invocato il metodo `get()` del future, lo stato condiviso viene rimosso e l'oggetto future entra in uno stato invalido: `get()` può essere **chiamato una sola volta**.

`wait()` consente di forzare l'avvio del task e attenderne la terminazione, senza prelevare il risultato: può essere chiamato più volte, se il task è già terminato, ritorna immediatamente. 

E' possibile attendere il completamento per un periodo specifico di tempo, **senza forzare l'avvio del task** se la politica è deferred, attraverso `wait_for()` e `wait_until()`: ricevono come parametro rispettivamente un valore di tipo `std::chrono::duration` e `std::chrono::time_point`. Ritornano:

* `std::future_status::deferred` se la funzione non è ancora partita
* `std::future_status::ready` se il risultato era già pronto o lo è diventato nel tempo di attesa.
* `std::future_status::timeout` se il tempo è scaduto, senza che il risultato sia diventato pronto.

**Quando un oggetto future viene distrutto, il distruttore attende la fine del task lanciato**: se la computazione è ancora attiva, questo può comportare un'attesa significativa. Le attività lanciate in questo modo non sono cancellabili se non avendo cura di condividere con la funzione chiamata una variabile che possa essere usata come criterio di terminazione.

```
int quickComputation(); //approssima il risultato con un'euristica 

int accurateComputation(); // trova la soluzione esatta, ma richiede tempo 
                        
std::future<int> f; // dichiarato all'esterno perché il suo ciclo di vita potrebbe estendersi più a lungo della funzione

int bestResultInTime()
{
  // definisce il tempo disponibile
  auto tp = std::chrono::system_clock::now() + std::chrono::minutes(1);
  
  // inizia entrambi i procedimenti
  f = std::async (std::launch::async, accurateComputation);
  int guess = quickComputation();
  
  // trova il risultato accurato, se disponibile in tempo
  std::future_status s = f.wait_until(tp);
  
  // ritorna il miglior risultato disponibile
  if (s == std::future_status::ready) {
    return f.get();
  } else {
    return guess; // attenzione: accurateComputation() continua!
  }
}
```

### `std::shared_future<T>`

Evita data race nell'accesso al risultato della `get()`: utile se bisogna valutare la terminazione dell'operazione asincrona in più thread. permette di chiamare `get()` più volte, produrrà sempre lo stesso risultato.

```
int task1(); // prima fase del calcolo
std::string task2(std::shared_future<int>); // seconda fase
double task3(std::shared_future<int>); // terza fase

int main() {

  std::shared_future<int> f1 = std::async(std::launch:async, task1).share(); //crea shared future a partire dal future
  std::future<std::string> f2 = std::async(task2, f1); //passo sf di f1 per ottenere il risultato con f1.get()
  std::future<double> f3 = std::async(task3, f1); // come sopra

  try { 
    std::string str = f2.get();
    double d = f3.get();
  } catch (std::exception& e) {
    //gestisci eventuali eccezioni
  }
}
```

### `std::promise<T>`

Fa da ponte tra il Callable eseguito in un nuovo thread da async e l'oggetto future restituito.

Rappresenta l'impegno, da parte del thread, a produrre prima o poi un oggetto di tipo T e di metterlo a disposizione, oppure di notificare un'eccezione che abbia impedito il calcolo dell'oggetto. Dato un oggetto promise, si può verificare che la promessa si è avverata richiedendo l'oggetto future corrispondente attraverso il metodo `get_future()`, e su questo chiamare `get()`, che blocca il thread in attesa che la promise si avveri ed il future restituisca il suo valore (o l'eccezione).

```
/*
* Implementazione di async home-made sfruttando thread e promise
*/

#include <future>

// f si impegna a fornire una promise
void f(std::promise<std::string> p) {

  try {
    //Calcolo il valore da restituire...
    std::string result = …;
    p.set_value(std::move(result));
  
  } catch (...) {
    p.set_exception(std::current_exception());
  }
} 

int main() {
  std::promise<std::string> p;
  std::future<std::string> fut = p.get_future();
  
  // creo un thread, forzando p a essere passata per movimento
  std::thread t(f, std::move(p));  
  t.detach();  

  // faccio altro...

  // accedo al risultato del thread
  std::string res = fut.get();

}
```

#### Quando usare thread e quando async

* con async si può impostare il tipo di esecuzione (async, deferred).
* con thread non c'è gestione built-in di eccezioni e valori di ritorno.
* con thread si possono ottenere risultati intermedi.

#### Uso dei thread detached

Sono pericolosi dato che diventa impossibile conoscere lo stato del thread distaccato.

Promise e future possono essere utilizzati per gestire un livello di sincronizzazione tra il thread principale ed i thread detached:

* il thread principale, invocando `wait()` o `get()` sul future resta bloccato in attesa del valore di ritorno. Ma il fatto che riceva il valore di ritorno non significa che il thread detached abbia terminato! i.e. potrebbe avere ancora del codice da eseguire, ad esempio le chiamate ai distruttori.

Per evitare questi problemi, la classe promise fornisce dei metodi spefici per evitare corse critiche tra la pubblicazione di un risultato nell'oggetto promise e la continuazione degli altri thread in attesa dello stesso:

* `set_value_at_thread_exit(T val)`
* `set_exception_at_thread_exit(std::exception_ptr p)`

#### Thread Pool di Packaged task

la classe `std::packaged_task<T(Args...)>` può essere usata per incapsulare una funzione che restituirà un tipo T, ricevendo una serie di argomenti. Quando il packaged_task viene eseguito, la funzione viene chiamata. **Un packaged task ha associato un future, quando l'esecuzione finirà, il future potrà restituire il risultato.**

I packaged task si prestano ad essere utilizzati per implementare **Thread Pool**: un numero limitato di thread a cui demandare l'esecuzione di compiti puntuali. 

Il numero di core disponibili si può ottenere attraverso `std::thread::hardware_concurrency()`. Per operazioni CPU-bound conviene avere un numero di thread pari al numero di core+1, per operazioni IO-bound, conviene moltiplicare per un fattore 10-50 il valore precedente.


#### Lazy evaluation

Situazione in cui è utile rimandare la creazione ed inizializzazione di strutture complesse fino a quando non devono essere effettivamente utilizzate. 

C++ offre la classe `std::once_flag` e la funzione `std::call_once(once_flag,Callable,...)`. La classe **registra in modo thread safe se è già avvenuta o sia in corso una chiamata a call_once**.

call_once esegue la funzione f una sola volta: **se dal flag passato non risultano chiamate, inizia l'invocazione, se risultano, blocca l'esecuzione in attesa del suo risultato**.

Con questo meccanismo è ad esempio possibile implementare una classe singleton thread safe.

```
#include <mutex>

class Singleton {
private:
  static Singleton *instance;
  static std::once_flag inited;

  Singleton() {...} //privato

public:
  static Singleton *getInstance() {
    std::call_once(inited, []() {
        instance = new Singleton();
      });
    return instance;
  } 
};
```


## Condition variable in C++11

Spesso un thread deve aspettare uno o più risultati intermedi prodotti da altri thread, per motivi di efficienza, l'attesa non deve consumare risorse. La coppia di classi promise/future offrono una soluzione limitata al problema, valida solo quando occorre notificare la disponibilità di un solo dato.

`std::condition_variable` modella una primitiva di sincronizzazione che permette l'attesa **condizionata** di uno o più thread, fino a che non si verifica una **notifica** da parte di un altro thread, **scade un timeout** o si verifica una **notifica spuria**. 

Richiede l'uso di uno `unique_lock` per garantire l'assenza di corse critiche nel momento del risveglio. 

* `wait(unique_lock)` per bloccare l'esecuzione del thread, mettendolo in stato di not_runnable nella queue del SO, senza che consumi CPU. Esiste un overload che ottiene come secondo parametro un Callable (lambda) per testare se la condizione di wait è valida o meno, in modo da evitare di andare in sleep se è inutile: è lecito fare il test perchè la wait si fa sempre avendo già preso il lock. La wait provede a rilasciare il lock prima di terminare.

* Un altro thread può infromare uno o tutti i thread attuamente in attesa che la condizione sia verificata attraverso i metodi `notify_one()` e `notify_all()`. E' bene che la notify venga fatta dal thread quando ancora possiede il mutex.

```
use namespace std;
mutex m; //protegge la cv
condition_variable cv;
int dato;

void produce() {
  ... //calcola un dato
  {
    lock_guard<mutex> lg(m);
    dato = ...;
    cv.notify_one(); //rilascia lock e sveglia thread
  }
  ...
}

void consume() {
  unique_lock<mutex> ul(m); //prende lock
  cv.wait(ul); //mette in attesa di condizione e rilascia lock
  //uso il dato
} 
```

La presenza di un unico lock fa si che se più thread ricevono la notifica, il risveglia sia progressivo, un thread per volta.

`wait_for()` e `wait_until()` permettono di limitare l'attesa nel tempo.


### Operazioni atomiche

Le moderne CPU supportano alcune istruzioni specializzate per permettere l'accesso atomico ad un singolo valore. 

La classe  `std::atomic<T>` offre la possibilità di accedere in modo atomico al tipo T, garantendo che gli accessi concorrenti alla variabile sono osservabili nell'ordine in cui avvengono, questo garantisce il meccanismo minimo di sincronizzazione. 

Lettura e scrittura di queste variabili sono implementate seguendo tecniche di memory fence basate su spin lock.

Le operazioni effettuate sulla variabile sono eseguite dalla CPU nell'ordine in cui sono scritte. 

```
std::atomic<boolean> done = false;

void task1() {
  //continua ad elaborare fino a che non viene detto di smettere
  while (! done.load() /*atomica*/ ) {
    process();
  }
}

void task2() {
  wait_for_some_condition();
  //segnala che il task1 deve finire
  done.store(true); //atomica
  //...
}

void main() {
  auto f1=std::async(task1);
  auto f2=std::async(task2);
}
```

### Interazione con il thread corrente

Il namespace `std::this_thread` offre un insieme di funzioni che permettono di interagire con il thread corrente.

* `get_id()` ritorna l'ID del thread corrente

* `sleep_for(duration)` sospende l'esecuzione del thread corrente per almeno il tempo indicato.

* `sleep_until(time_point)`

* `yield()` eseguito dal thread running, cheide allo scheduler di rischedulare il thread corrente al fondo della runing_queue, lasciando quindi la CPU agli altri thread (se ci sono). Per alcune architetture il compilatore mappa yield come`nop`.

**Non usare queste funzioni per gestire la sincronizzazione.**




# **Applicazioni basate su interfacce grafiche**

L'interazione è uno degli aspetti fondamentali da dover attenzionare quando si sviluppa un'applicazione, l'interazione deve risultare:

* Efficace
* Efficiente
* Facile da imparare
* Piacevole

Un'interfaccia utente deve risultare comprensibile e coerente, mettendo in risalto l'informazione principale.

## Pattern per la realizzazione di un'interfaccia grafica (UI)

In un programma in cui l'utente interagisce tramite UI, l'interazione non è sequenziale, ne guidata dal programma stesso: l'utente può scegliere con quale componente interagire in un dato momento, in un ordine non prevedibile.

Una UI è composta da da Widget (liste, textbox, buttons ecc) che si prestano ad essere modellati in oggetti, mettendoli in relazione l'un l'altro attraverso l'ereditarietà (i.e. button visto come una specializzazione di un textbox, aggiunge la possibilità di essere cliccato). Ogni framework grafico propone una propria gerarchia di classi e si basa su uno dei seguenti pattern di programmazione per la definizione dei widget:
 
* **Composite**: composizione di oggetti per ottenere oggetto complesso (strutturato ad albero). I widget sono organizzati in strutture ricorsive ad albero, per rappresentare la relazione "parte-tutto", i.e. classe Window è specializzata in Group che a sua volta è specializzata in Image, Label e Button. 

* **Template**: definisce la struttura generale di un algoritmo (in una classe "template"), demandando alcune operazioni alle sotto classi: queste possono adattare il proprio comportamento senza compromettere la struttura dell'algoritmo. I.e. FrameworkClass che ha come figlie ApplicationClass1 e ApplicationClass2 che fanno override di un metodo definito dalla classe template.
  
* **Strategy**: i widget demandano ad altri componenti la gestione di alcune funzionalità specifiche, i.e. LayoutManager in Android gestisce il Layout. Questo rende indipendenti gli algoritmi dalle classi che li utilizzano.

## Architettura applicativa per il supporto dell'interazione

L'architettura applicativa di un'applicazione basata su interfaccia grafica si basa su una **coda di eventi**, il main attende che si verifichi un evento (i.e. pressione tasto), lo estrae dalla coda e lo invia alle funzioni che si sono registrate come gestori dell'evento (callback). E' il sistema operativo ad inserire gli eventi nella coda.

I diversi componenti dell'applicazione esprimono a priori il proprio interesse a ricevere determinati tipi di eventi, registrando opportuno funzioni che potranno essere invocate in futuro (callback).

Nella maggior parte dei casi, non occorre utilizzare più di un thread, il thread principale gestisce la coda ed esegue le callback: se questi eseguono codice bloccante, potrebbero verificarsi dei problemi. Le callback vengono eseguite in modo sincrono dal thread principale, se una si blocca per un tempo lungo, il programma appare bloccate (non risponde all'interazione dell'utente).

Per evitare ciò si può spezzare l'esecuzione in più eventi, ed al termine di ogni callback inserire un nuovo evento in coda, oppure lanciare un nuovo thread, in quest'ultimo caso bisogna tenere conto del fatto che **solo il thread principale può agire sull'albero dei widget** (visual tree, **view**).

```
1. inizializza il framework
2. crea albero dei widget
3. regista le callback
4. esegui il ciclo degli eventi
```

Lo stato dell'applicazione è mantenuto dal **model**, e verrà aggiornato dalle callback, che hanno il ruolo di **controller**, modificando il modello. Le modifiche verranno rispecchiate dalla **view**. In questo modo viene implementato il pattern **MVC**.

## Il pattern observer

Per facilitare il compito della vista di mantenere aggiornate le proprie strututre dati al variare delle informazioni contenute nel modello, spesso si ricorre al pattern observer:

* Il modello mantiene una lista di viste interessate ai suoi cambiamenti.
* ogni volta che il modello cambia una parte del proprio stato, notifica tutte le viste al momento registrate del cambiamento avvenuto, in Qt, emette uno dei suoi segnali.
* La vista reagisce alla notifica aggiornando il proprio contenuto, in Qt, attraverso la chiamata ad uno dei suoi slot, che sono stati connessi ad uno dei segnali del modello.

In alcune situazione, può essere necessario ricorrere ad un oggetto intermediario, detto **adapter**, che si registri come osservatore del modello e, in base al suo contenuto, decida quali e quanti widget debbano dover far parte dell'albero delle viste (i.e. Adapter per la RecycleView di Android). Questo permette, in caso di viste contenenti molti widget, di limitare la struttura topologica dell'albero alle sole viste utili (quelle visibili all'utente).

In Qt, il pattern observer è implementato dai signal/slot e dal pattern model/view.


# **Il framework Qt**

Qt estende il C++ aggiungendo le seguenti funzionalità:

* meccanismo di comunicazione tra oggetti basato su **segnali e slot** che implementa il pattern observer/observable.
* un sistema di **properties** interrogabile ed estendibile
* **eventi e filtri** di eventi
* **internazionalizzazione** delle stringhe
* **timer a intervalli** che rendono possibile implementare task non bloccanti
* **organizzazione degli oggetti ad albero**, gerarchica e interrogabile
* **puntatori gestiti (QPointer)**, automaticamente impostati a 0 quando l'oggetto referenziato viene distrutto
* un meccanismo di **dynamic cast**

### QObject

`QObject` è la classe padre della maggior parte delle classi di Qt, compresi tutti i `QWidget`. Le responsabilità di questa classe sono di gestire la memoria, effettuare **l'identificazione del tipo a runtime** (introspezione) e gestire gli eventi. Non offre alcun meccanismo di garbage collection ma un sistema di trasferimento del possesso, analogamente agli smart pointer.

Ogni istanza di un `QObject` può ricevere come argomento del proprio costruttore il puntatore ad un genitore. Il figlio informa il genitore della propria esistenza, che provvederà ad aggiungerlo alla lista dei propri figli.

Gli oggetti che ereditano da `QObject` vengono normalmente **allocati nello heap**. Se viene loro assegnato un genitore, questi prende possesso dell'oggetto appena creato e ne chiama la delete quando necessario (può farlo perchè il padre ha il possesso - ha il puntatore - dei figli).

```
QLabel* label = new QLabel("42", parent_obj); 
//oppure: label.setParent(parent_obj)
```

Gli oggetti che non ereditano da `QObject` sono invece allocato nello stack, i.e. `QStringList sl;`.

Fanno eccezione `QFile`, `QApplication` ed i dialog che, pur ereditando da `QObject` vengono di solito allocati sullo stack.

Quando un `QObject` viene creato, ne viene impostata **l'affinità al thread** in cui tale operazione avviene, tutti i figli devono avere la stessa affinità del genitore.

Il **Meta Object Compiler (MOC)** di Qt compila tutti i QObject in classi C++, dopo si può effettuare la compilazione del risultato utilizzando il compilatore standard.

### QWidget

La classe `QWidget` estende la semantica della relazione genitore-figlio: un figlio diventa anche un widget figlio, cioè viene mostrato nello stesso sistema di coordinate del genitore e limitato graficamente dai confini del genitore.

Ad ogni oggetto si può associare un nome, ed utilizzare il nome per cercarlo all'interno dell'albero dei figli di un `QObject`:

```
// trova il figlio di tipo QPushButton chiamato button1
QPushButton *button = parent->findChild<QPushButton*>("button1");

// trova tutti i figli di tipo QPushButton
QList<QPushButton*> allButtons = parent.findChildren<QPushButton*>();

// trova tutti i figli di tipo QWidget e di nome "widgetname"
QList<QWidget*> widgets = parent.findChildren<QWidget*>("widgetname");
```

Ogni elemento dell'albero della view è un'istanza di QWidget, ognuno di essi ocucpa un'area rettangolare, che è ritagliata all'interno del rettangolo del widget genitore.

### QPointer

`QPointer` è uno smart pointer specializzato per le istanza di `QObject` e derivati, internamente utilizza un weak_pointer, e quando l'oggetto a cui punta viene distrutto, viene posto automaticamente a zero.

```
QObject *obj = new QObject;
QPointer<QObject> pObj(obj);
delete obj;
Q_ASSERT(pObj.isNull()); // true
```

`QSharedPointer` può puntare a qualsiasi tipo di dato: mantiene internamente un conteggio dei riferimenti, rilascia la memoria quando il contatore arriva a zero.


### QVariant

Simula il funzionamento di una union, in grado di contenere una molteplicità di dati diversi. Il metodo `type()` consente di conoscere il tipo di dato memorizzato.

```
QDataStream out(...);
QVariant v(123);  // v contiene un intero
int x = v.toInt();// x = 123
out << v;  // Scrive un tag e un valore
v = QVariant("hello"); // v contiene un QByteArray
v = QVariant(tr("hello")); // v contiene una QString
int y = v.toInt(); // y = 0
QString s = v.toString(); // s = tr("hello")
```

### QString

modella una sequenza di caratteri UTF-16, internamente è implementato attraverso un buffer allocato dinamicamente e condiviso con politica CoW.

### Qt collections

Qt fornisce un insieme di classi compatibili con quelle della STL di C++:

* QList
* QVector
* QLinkedList
* QQueue
* QStack
* QMap
* ...

Per scorrere gli elementi contenuti nei contenitori, sono offerti due tipi di iteratori, uno in stile STL, uno in stile Java: i primi sono più efficienti ma possono essere invalidati se si modifica il contenuto della collezione durante l'iterazione, i secondi mantengono una propria copia privata della collezione su cui poi iterano.

```
//Iteratori classici (stile STL)
QVector<QString> data {"uno","due","tre"};
for(auto it=data.begin(); it!= data.end(); it++) {
  std::cout<< "+ " << it->toStdString() << "\n";
}

//Iteratori Java-like
QVectorIterator<QString> it(data);
while (it.hasNext()) {
  std::cout << "- " << it.next().toStdString() << "\n";
}
```


### QApplication

Ogni applicazione Qt deve avere almeno un oggetto `QApplication` che gestisce la coda dei messaggi, il ciclo di vita, e l'albero dei widget. Deve essere creato prima di ogni altro componente grafico, tipicamente instanziato come variabile locale all'interno del main. Il metodo `exec()` esegue il ciclo dei messaggi.

```
#include <QApplication>
#include <QPushButton>

int main(int argc, char** argv) {
  QApplication app(argc, argv);  // inizializzazione di Qt

  QPushButton b ("Hello QT!"); // radice dell'albero
  b.show();  // mostra l'albero(!) delle viste

  return app.exec();  // ciclo dei messaggi
}
```

In Qt ogni QObject ha conoscenza del QThread in cui è stato creato.

L'albero di elementi della UI è composto da derivati della classe `QWidget`, ogni albero corrisponde ad una finestra, ed ad un `QThread`: **tutti gli elementi dell'albero devono appartenere allo stesso Qthread**.


### QMainWindow

`QMainWindow` semplifica la creazione di una GUI fornendo un layout standard in cui ospitare i widget di una QApplication.

```
#include <QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QMainWindow>

int main(int argc, char** argv) {
    QApplication app(argc,argv);
    QMainWindow mainWindow();

    QPushButton* button = new QPushButton("Run");

    mainWindow.setCentralWidget(button);

    QStatusBar* bar=new QStatusBar();
    bar->showMessage("Idle...");
    mainWindow.setStatusBar(bar);

    mainWindow.show();
    return app.exec();
}
```

La classe QWidget demanda alla classe `QLayout` la strategia di disposizione degli elementi del sotto albero della view (quello del QWidget e dei suoi figli) sullo schermo. QLayout negozia con i figli dimensioni e posizione. Sono disponibili specifiche specializzazioni di QLayout che supportano diversi algoritmi di ordinamento dei QWidget:

* QBoxLayout
* QFormLayout
* QGridLayout
* QstackedLayout
* ...

### QDialog

I dialog vengono creati combinando vari widget, possono essere allocati sullo stack, un dialog modale restituisce un flag (risposta: true/false).

## Il Model/View pattern

Alcuni tipi di widget sono specializzati per visualizzare collezioni di dati: liste, tabelle, alberi.

Il pattern Model/View prevede la fusione del controller nella view, in modo da ottenere una struttura più semplice da gestire.

* il modello nasconde i dati dalla view e dai delegate.
* la view usa il delegate per visualizzare e modificare i dati.


## Meta-Object System (MOS)

Qt mantiene all'interno degli oggetti del framework un **sistema di meta-informazioni** per classi e tipi che **permette l'estensione del linguaggio C++**:

* abilita i meccanismi di signal e slot per la comunicazione tra oggetti
* implementa le proprietà dinamiche
* consente di fare cast type-safe a runtime, senza il supporto del compilatore
* permette la serializzazione/deserializzazione a tipi arbitrari

Gli elementi che permettono di implementare questo sistema sono:

* la **classe QOBject**, da cui derivano tutti gli oggetti che fanno uso del MOS.
* la **macro Q_OBJECT**, inserita nella sezione privata della dichiarazione di una classe serve a segnalare che la classe necessita del supporto del MOS.
* il **Meta-Object Compiler**, che fornisce a tutte le sottoclassi di QObject in cui è presente la macro il codice necessario per l'implementazione delle funzionalità sopra citate.

Il MOC è il cuore di questo sistema, permette l'estensione dello standard del C++ con i concetti di signal, slot e properties, ed abilita i meccanismi di:

* **introspezione dei tipi**: consente di esaminare il tipo, i metodi, e le proprietà  di un oggetto in fase di esecuzione.
* **riflessione**: va un passo oltre l'introspezione, consentendo ad un oggetto di modificare durante l'esecuzione la propria struttura e il proprio comportamento.

Il MOC non modifica il codice sorgente, ma genera per le classi che contengono la macro Q_OBJECT, file C++ aggiuntivi con estensione `.moc`, questi sono responsabili della definizione del `QMetaObject` relativo alla classe, che contiene tutte le informazioni necessarie per supportare l'introspezione e la riflessione. Questi vengono poi compilati assieme al resto dei sorgenti, o inclusi a valle della definizione della classe.

La classe `QMetaObject` non è strettamente necessaria per la realizzazione di applicazioni, è utile per realizzare meta-applicazioni basate su scripting engine o designer di GUI. I metodi generalmnete più utilizzati sono:

* `className()`, ritorna il nome della classe
* `superclass()`, restituisce il meta-object della superclasse
* `method` e `methodCount()` forniscono informazioni riguardanti i meta-metodi della classe (i.e. signal e slot).
* `property()` e `propertyCount()` forniscono informazioni sulle property della classe.

La classe `QMetaType` è usata da Qt per rendere disponibili le informazioni sui tipi a runtime.


## Gestire l'interazione: Signal e slot

Gli oggetti posso interagire tra di loro, ad esempio per gestire l'aggiornamento della vista, attraverso un meccanismo basato su `signal` e `slot` che **implementano il pattern observer in Qt**.

Un `signal` è un particolare metodo dichiarato all'interno di una classe Qt, nella sezione `signals:` della classe, che **viene implementato dal MOC**, Meta Object Compiler, di Qt. E' possibile emettere un signal utilizzando la keyword `emit`.

Gli `slot` sono dei metodi che possono essere chiamati quando vengono connessi ad un signal, attraverso la funzione statica `connect()` offerta da QObject, la funzione è **sincrona**.

```
class Model: public QObject {
  Q_OBJECT // macro for the MOC
  int count, min, max;
public:
  Model(int v, int min=0, int max=100): 
     count(v), min(min), max(max) { }

public slots:
  void countUp() { 
	if (count<max) {
    	count++; 
		emit changed(count); 
	} 
  }
  
  void countDown() { 
    if (count>min) {
   	   	count--; 
		emit changed(count); 
	} 
  }

signals:
  void changed(int newValue);
};
```

Quando un oggetto intende far sapere al resto del sistema che al proprio interno si è verificato un cambiamento, può **emettere uno dei propri segnali**, attraverso l'operatore `emit signalName()`.

Se, viceversa, **un oggetto vuole reagire ad un cambiamento esterno in un altro oggetto**, esso può implementare tale comportamento in uno dei propri slot. Una opportuna funzione statica, la `QObject::connect()` permette di collegare dinamicamente signal e slot di oggetti differenti.

Ogni volta che un oggetto emetterà un segnale, tutti gli slot che sono stati collegati al segnale saranno eseguiti.

```
void main(int argc, char**argv) {
  Qapplication app(argc,argv);

  QPushButton* b = new QPushButton("+");
  Model* model = new Model(0,0,100);
  QLabel *l = new QLabel("0");
  
  QObject::connect(b, &QPushButton::clicked, model, &Model::countUp);
  QObject::connect(model, &Model::changed,
        [=](int v) { 
			l->setText(Qstring::number(v));
		}
  );

  b->show();
  app.exec();
}
```

Una nuova sintassi per signal e slot è stata introdotta nelle versioni recenti di Qt, basata sulle macro `SIGNAL()` e `SLOT()`.

vantaggi:

* check in fase di compilazione dell'esistenza di signal, slot e Q_OBJECT
* cast automatico dei tipi
* è possibile connettersi non solo a slot a ma a qualunque funzioni membro di QObject, a funzioni qualcnque ed ad espressioni lambda (funtori/oggetti funzionali).

svantaggi:

* gli argomenti di default negli slot non sono più supportati, non c'è disconnessione automatica da una funzione semplice (senza Q_OBJECT) quando il ricevente viene distrutto (va chiamata `disconnect()`).

```
QMetaObject::Connection m_conn;

// nuova connessione functor-based
m_conn = connect(sender, &Sender::valueChanged, receiver, &Receiver::updateValue);

// nuova connessione functor-based, funzione qualunque senza Q_OBJECT m_conn = connect(sender, &Sender::valueChanged, someFunction);

// nuova connessione functor-based, funzione lambda
m_conn = connect(sender, &Sender::valueChanged, 
	 [=](const QString &newValue) {
	     receiver->updateValue("senderValue", newValue);
	 }
); 

// nuova disconnect, simmetrica a connect functor-based
disconnect(sender, &Sender::valueChanged, receiver, &Receiver::updateValue); 

// nuova disconnect che fa uso di QMetaObject
QObject::disconnect(m_conn);
```

Quando si collegano due QObject attraverso signal/slot, non c'è bisogno che i due QObject appartengano allo stesso QThread.


## Gestione degli eventi

Gli eventi sono oggetti derivati dalla classe astratte `QEvent`: `QMouseEvent`, `QResizeEvent`, `QPaintEvent` ecc. Questi eventi possono essere scatenati sia da attività esterne (interazione dell'utente) o da cambiamenti avvenuti all'interno dell'applicazione.

Gli eventi sono inseriti all'interno di una coda, in Windows, la coda è gestita dal sitema operativo e non è accessibile dall'applicazione Qt.

Gli eventi possono essere generati da `sendEvent()` o `postEvent()` (meglio quest'ultimo che non è rientrante) e sono ricevuti dal metodo `event()` del QObject, che si occuperà di fare il dispatch verso il gestore effettivo (i.e. `KeyPress()`) sulla base del tipo dell'evento, che s può ottenere attraverso il metood `type()`.

Il loop degli eventi inizia con la chiamata ad `exec()` e termina con la chiamata a `quit()`.

`QCoreApplication` e `QGuiApplication` sono le classi che forniscono un loop degli eventi per le applicazioni Qt. Esiste una sola istanza di queste classi per una applicazione Qt.

Qt offre particolari metodi per la gestione di **eventi di ridisegno** ed **eventi periodici**.


## Qt build system

Qt mette a disposizione tre strumenti:

* MOC: meta-object compiler
* UIC: user interface compiler, traduce la definizione XML della UI in header C++
* RCC: resource compiler, genera codice C++ per incorporare risorse binarie nelle applicazioni Qt (i.e. immagini, icone). I nomi delle risorse su disco vanno elencati in un Resource Collection File con estensione `.qrc`.

Processo di compilazione di un programma Qt:

* `qmake -project`, crea un Qt project file `.pro`
* `qmake` legge il project file e produce in output un Makefile specifico per la piattaforma, genera le make rules per invocare MOC per gli header file contenenti la macro Q_OBJECT
* `make` compila il programma, utilizzando il Makefile, per la piattaforma specifica. Esegue anche MOC, UIC ed RCC. 

## Integrazione con il sistema sottostante

La classe `QSettings` permette di conservare le impostazioni dell'applicazione nella maniera nativa della piattaforma:

* Windows: `HKEY_CURRENT_USER\Software\organization\application`
* Linux: `$HOME/.config/organization/application.conf`

La classe `QSystemTrayIcon` permette di integrare la propria applicazione con la system tray e l'area di notifica della shell del sistema operativo.

La classe `QDesktopServices` permette di accedere a servizi come l'apertura di una pagina web in una webview o a locazioni standard del file system.

## Esecuzione in background

Qt fornice due API per ll supporto dell'elaborazione in background:

* `QThread` permette di creare e gestire thread.
* `QtConcurrent` consente di usare la programmazione concorrente a più alto livello, permette di ignorare i dettagli riguardanti la sincronizzazione.



# **Processi ed IPC**

La programmazzione multi processo porta con se il problema della **gestione della comunicazione tra i vari processi**.

L'uso dei thread permette di sfruttare i processori multithread, ma presentano lo svantaggio di avere uno **spazio di indirizzamento condiviso tra tutti i flussi di esecuzione**. Ci sono situazioni in cui la presenza di un singolo spazio di indirizzamento non è possibile o desiderabile: 

* **sicurezza**, i.e. chrome crea un processo per ogni tab.
* **scalabilità**, programmi eseguiti su macchine distinte che comunicano a livello di processo tra loro (i.e. Hadoop).
* **riuso**, riutilizzo di programmi già esistenti per la gestione di task comuni.

I processi creati a partire da un processo genitore devono essere in grado di operare e cooperare tra di loro indipendentemente dalla loro genesi.

Ad ogni processo è associato almeno un thread (primary thread): **un sistema multiprocesso è instrinsecamente concorrente**, solleva gli stessi problemi della programmazione multithread, rendendone più difficile la gestione per via della mancanza di uno spazio di indirizzamento condiviso (i.e. appoggiarsi su variabili globali protette da mutex è impossibile).


## Processi in Windows

In windows i processi costituiscono entità separate, senza relazioni di dipendenza esplicita tra essi.

`CreateProcess()` permette di creare un processo, il nuovo processo conosce qual'è il processo che l'ha creato, ma non ci sono dipendenze esplicite. Questa funzione:

* crea un nuovo **spazio di indirizzamento**
* **lo inizializza** con l'immagine di un programma in formato `.exe` scritto secondo la sintassi `PE2` (Portable Executable 2)
* **attiva il thread primario** utilizzando la runtime library del C++.

Questo nuovo processo figlio **può condividere** con il padre variabili d'ambiente, handle a file, semafori, pipe del processo padre, ma **non può condividere** handle a thread, processi, librerie dinamiche, e regioni di memoria.

## Processi in Linux

Si crea un processo con l'operazione `fork()`: crea un nuovo spazio di indirizzamento identico a quello del processo genitore, secondo la politica Copy-On-Write. Fork ritorna al processo padre il PID del figlio, al figlio ritorna 0, può ottenere il PID del padre attraverso la funzione `getppid()`.

La famiglia di funzioni `exec*()` sostituiscono l'attuale immagine di memoria (l'eseguibile) dello spazio di indirizzamento del processo che chiama la funzione. Permette ad esempio al figlio di eseguire un nuovo eseguibile.

```
int main ( const int argc, const char* const argv[] ) {
    pid_t ret = fork();
    switch (ret) {
	case -1:
	   puts( "parent: error: fork failed!" );break;
    case  0:
	   puts( "child: here (before execl)!" );
	   if (execl( "./ch.exe", "./ch.exe", 0 ) == -1) //exec
		perror( "child: execl failed:" );
	   puts( "child: here (after execl)! how the f*ck am I here!?" ); 
	   //non si dovrebbe arrivare qui
 	   break;
    default:
		printf( "par: child pid=%d \n", ret );
		break;
    }
 return 0;
}
```

### Fork e Thread in linux

nel caso di programmi concorrenti, l'esecuzione di `fork()` crea un problema:

* **il processo figlio conterrà un solo thread, anche se il padre avesse creato più di un thread.**

Cosa succede se mentre avviene la creazione di un processo, uno dei thread del padre modificano l'area di memoria indirizzata dal processo? Gli oggetti di sincronizzazione (mutex ecc..) presenti nel padre possono trovarsi in stati incongruenti.

La funzione `int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));` accetta  tre puntatori a funzione (handlers), permette di eseguire le chiamate a fork in modo thread-safe. In caso di chiamata ad una fork:

* prima dell'esecuzione di fork viene eseguita la **prepare**, dovrebbe fare in modo che i thread esistenti rilascino le risorse. Una delle possibili implementazioni è che tutti i mutex vengano acquisiti da prepare. 
* Avviene la creazione del processo.
* Viene eseguita dal padre la **parent**, fa il duale della prepare: rilascia tutti i mutex
* Nel processo figlio viene eseguita la **child**, fa il duale della prepare come per parent.

In questo modo sia il processo padre che figlio continueranno la loro esecuzione in uno stato congruo e predicibile (tutti i mutex liberi).

## IPC

Come conseguenza dell'avere due spazi di indirizzamento diversi, non è possibile condividere dati tra processi attraverso l'uso di variabili globali o zone di memoria condivise e protette da meccanismi di sincronizzazione. 

Ogni SO offre alcuni meccanismi per superare la barriera tra due processi in modo controllato, ed ogni SO implementa questi meccanismi in modi diversi. 

L'IPC permette:

* **sincronizzazione** delle attività tra processi differenti, informa i processi che qualcosa sia avvenuto nell'altro, alcuni costrutti di IPC sono bloccanti, permettendo di implementare l'attesa.
* **scambio di dati** tra processi.

Indipendentemente dal tipo di meccanismo adottato per l'implementazione dell'IPC, **occorre adattare le informazioni scambiate**, così da renderle comprensibili al destinatario: i.e., i puntatori non hanno significato se lo spazio di indirizzamento cambia.

**Rappresentazione esterna**: formato intermedio che permette la rappresentazione di strutture dati arbitrarie, **sostituendo i puntatori con riferimenti indipendenti dalla memoria**.

Due strade possibili per rappresentare esternamente il testo:

* formati basati su testo (i.e. xml, json, yaml)
* formati binari (i.e. XDR, HDF)

Il passaggio da rappresentazione interna a rappresentazione è detta **serializzazione**, o marshaling.

## Tipi di IPC

### Code di messaggi

Permettono **l'invio di messaggi asincroni** tra due o più processi, permette quindi scambio di dati ma anche sincronizzazione, dato che la ricezione e successiva lettura di un messaggio mette al corrente il processo che qualche cosa (che ha scatenato l'invio del messaggio) si è verificata nell'altro processo.

Permettono il trasferimento atomico di blocchi di byte, comportamento FIFO.

* serializzazione (rappresentazione interna -> rappresentazione esterna)
* push nella coda (attraverso API del SO)
* deserializzazione (rappresentazione esterna -> rappresentazione interna)

### Pipe

Permettono il trasferimento di sequenze di byte di dimensioni arbitrarie tra due processi:

* occorre inserire marcatori che consentano di delimitare i singoli messaggi, i.e. formato **TLV**.
* **comunicazione sincrona** (bloccante) 1-1, permette sincronizzazione.

### Memoria condivisa

permette di avere uno spazio di memoria indirizzato da due o più processi. Occorre sincronizzare l'accesso all'area condivisa, ad esempio utilizzando un Semaforo (permette sincronizzazione a livello di SO ta più processi).

### Semafori

* costrutto di sincronizzazione a basso livello, basato sulla manipolazione atomica di un valore intero.
* Il valore è gestito dal SO e non può mai diventare negativo
* Il valore è modificato utilizzando due funzioni atomiche (P()/up() incrementa il valore e V()/down() lo decrementa o si blocca in attesa di un incremento)

Un semaforo inizializzato ad 1 può essere visto come un mutex utilizzabile da più processi. **La differenza con il mutex è che il semaforo non ha conoscenza di chi sia il suo possessore.**

### Altro

File, Database, Socket, segnali unix.

## Identità e scelta dei canali di comunicazione

Il SO su richiesta dei singoli processi può creare canali di comunicazione tra quelli elencati sopra. Ad ogni canale è associato un **identificativo univoco a livello di sistema detto handle**. I processi che cooperano devono accordarsi sul tipo e l'identità del canale usato per la comunicazione

Fattori che influenzano la scelta del canale di comunicazione:

* **relazione e dipendenza tra i processi**: se ci sono dipendenze padre-figlio, una pipe ha senso. Se non ci sono dipendenze, non è possibile condividere handle.
* **comunicazione mono/bi-direzionale**? 
* **Numero di procesi coinvolti**
* Comunicazione tra **macchine diverse**? una sola soluzione: socket.


## IPC in Windows

Win32 offre meccanismi di sincronizzazione che permettono di bloccare un thread fino a quando non si sia verificata una condizione in un altro thread, sfruttando l'uso di **oggetti kernel condivisi**, il kernel permette un **accesso controllato al loro stato**, permettendo a thread appartenenti a processi differenti di modificarne lo stato.

Rispetto ai costrutti di sincronizzazione intra-processo, sono più generali ma meno efficienti: presentano tempi di blocco/sblocco maggiori,

Questa API espone due metodi:

* `WaitForSingleObject(handle h)`, accetta come parametro un handle ad un oggetto kernel
* `WaitForMultipleObjects()`, accetta come parametro una lista di oggetti kernel su cui aspettare, un flag che comunica varie politiche di attesa (i.e. `WAIT_ALL`)

```
/*primo processo*/
handle = LocateObject(...); //object è allocato in memoria kernel
wait(handle); //aspetta
closeHandle(handle);

/*secondo processo*/
handle = LocateObject(...);
signal(handle); //invia segnale che sblocca l'attesa del primo processo
closeHandle(handle);
```

La maggior parte degli oggetti kernel può trovarsi in due stati:

* **segnalato**
* **non segnalato**

Nel caso di processi e thread lo stato non segnalato indica che l'elaborazione è ancora in corso, una volta raggiunto lo stato segnalato, non è possibile tornare indietro.

Il kernel NT può gestire questi **oggetti kernel** per la IPC:

### Eventi

Imitano la semantica di una condition variable. La condizione è segnalata esplicitamente dal programmatore tramite opportuni metodi. Gli eventi di tipo manual-reset permettono ad un numero indefinito di thread in attesa di svegliarsi. Ci si collega ad un evento tramite `createEvent()` o `openEvent()`. Si modifica il loro stato attraverso `SetEvent()`, `ResetEvent()`, `PulseEvent()`, il loro comportamento varia in base al fatto che l'evento sia marcato come `AUTO_RESET` o un `MANUAL_RESET`.

### Semafori

Mantengono al proprio interno un contatore, stato segnalato se valore maggiore di zero, non segnalato se è a zero. Non può valere meno di zero, la `wait()` si blocca se il contatore è a zero.

### Mutex

Simile a quelli del C++ (recursive mutex) ma questi sono oggetti kernel per l'IPC, completamente diversi. Assicurano l'accesso in mutua esclusione ad una risorsa del sistema.

NOTA: più oggetti kernel possono essere utilizzati congiutamente per implementare politiche di sincronizzazione avanzate, i.e. utilizzare un evento ed un mutex per evitare deadlock nell'acquisizione di una risorsa (e.g. file mapping).

### Mailslot

Coda di messaggi asincrona per la comunicazion tra processi, anche tra macchine diverse, sulla stessa rete. 

	Si crea un mailslot server associandole un nome univoco:

	```
	HANDLE hSlot;
	LPTSTR SlotName = TEXT("\\\\.\\mailslot\\ms1");
	//crea un mailslot server
	hSlot = CreateMailslot(slotName, 0 /*no timeout*/, MALSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL /*default security*/);
	```
	Il mailslot client accoda i messagi ad una mailslot. `CreateFile()` apre la mailslite `WriteFile()` scrive il messaggio. Ogni write è interpretata come un messaggio diverso.

NOTA: è sempre necessario rilasciare il possesso dell'oggetto kernel attraverso la chiamata a `CloseHandle()` per evitare memory leak.

### Pipe

Anonymous (monodirezionali), named (possono essere bidirezionali).

### FileMapping

Adatto a condividere ampie zone di memoria. 

	`CreateFileMapping()` permette di creare un file mapping.
	`MapViewOfFIle()` restituisce un puntatore all'inizio del file mappato.
	`UnmapViewOfFile()` chiude il mapping.

	Tutti questi metodi NON gestiscono la sincronizzazione. E' necessario quindi utilizzarli congiutamente ad un mutex per garantire l'accesso esclusivo alla risorsa.

### Socket

Permettono comunicazione tra macchine dotate di sistemi operativi differenti, dato che opera a livello rete. Supportano solo il trasferimento di array di byte.

### Remote Procedure Call

Peste.


## IPC in Linux

Ciascuna struttura IPC è identificata nel SO da un **intero non negativo** (file descriptor): all'atto della creazione di un oggetto IPC, si fornisce una chiave, il SO converte questa chiave nell'ID associato (un file descriptor). Un processo può creare una nuova struttura IPC con chiave diversa da zero: tutti i processi che conoscono la chiave possono ottenere l'ID corrispondente.

La chiave può essere creata utilizzando la funzione `ftok()`, che genera la chiave a partire dal nome di un file (il suo contenuto è irrilevante) ed un byte che rappresenta il project-ID. 

L'ultimo processo che fa accesso ad una struttura dati IPC deve occuparsi della rimozione della stessa, altrimenti esso continuerà ad essere un oggetto kernel valido.


### Message queues

Permettono lo scambio di messaggi tra processi. I messaggi sono composti da un tipo e da un payload, il processo che riceve può specificare il tipo dei messaggi a cui è interessato.

```
struct message {
	long type;
	char messagetext[MESSAGESIZE];
}
```

Per creare una message queue esiste il metodo `int mesgget(key_t key, int msgflg)` che restituisce l'id della message queue associata alla chiave specificata.

Per inviare un messaggio, esiste il metodo `int msgsnd(int msqid, const void* msgp, size_t msgz, int msgflgs)`. 

Per leggere un messaggio `ssize_t msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg`.

Per controllare la coda, esiste la funzione `msgctl` che accetta vari parametri.

### Pipe

`pipe()` restituisce un array di dimensione due in cui il primo elemento rappresenta il punto di ingresso, il secondo il punto di uscita. Utile crearle prima della fork, poi padre e figlio utilizzano la pipe secondo il protocollo scelto dal programmatore.

Le named pipe sono create con `mkfifo()`.

### Shared memory

blocchi di memoria mappati nello spazio di indirizzamento di più processi.

`shmget()`, `shmctl()`, permettono di creare e controllare la zona condivisa.

Possibili operazioni:

* IPC_STAT
* IPC_SET
* IPC_RMID

`shmat()` permette di mappare nello spazio di indirizzamento del processo la zona di memoria condivisa, l'operazione duale è `shmdt()`.

`mmap()` permette di mappare un file in memoria, per condividere il contenuto del file tra processi in lettura/scrittura, semplificando le operazioni di tipo `fseek()`. `munmap()` rilascia la memoria mappata.

### Semafori

I semafori `System-V` vengono utilizzati per la sincronizzazione di thread di processi differenti, contrariamente all'utilizzo dei semafori `posix`. Sono costituiti da un array di contatori, se necessario, si può ottenere il lock contemporaneo.

Creazione: `semget()`

Modificare lo stato del semaforo: `semop()`

