# Programmazione di Sistema

Questa repository contiene codice prodotto per la sola parte curata dal Prof. Giovanni Malnati.


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




