# Note

---


## Analisi contesto

Ricerca di omologie tra i geni di genomi -> 1 genoma ha tanti geni
I geni vengono analizzati utilizzando dei k-meri con jaccard generalizzato o qualosa con jaccard 


---

## Ottimizzazioni

### K-mers

Dato che più k-mer di un gene sono hanno in comune una sola stringa è possibile mantenere mantenere un solo riferimento alla stringa all'interno di un Handler (`KmersHandler`) di Kmer (`Kmer`).

#### Strutture dati

##### Kmer

```js
object Kmer {
    // container che mantiene l'insieme di indici che corrispondono
    // all'inizio dei vari k-mer comuni nella sequenza
    begins_
    
    // contatore che mantiene il numero di volte che questo k-mer è ripetuto
    // all'interno nella sequenza
    multiplicity_
}
```
La struttuara di un Kmer si occupa di tenere traccia di:
- `begins_`, ovvero un container di possibili indici che identificano una sottostringa all'interno dell'alfabeto. Gli indici tenuti in questione hanno tipo `unsigned long`.
- `multiplicity_`, ovvero il numero di volte che un kmer si ripete all'interno di una sequenza. Questo contatore è di tipo `unsigned long`

Un oggetto di tipo Kmer deve essere creato se e soltanto se si va ad incontrare una sottostringa non già incontrata, in caso contrario deve essere aggiunto l'indice di inizio della sotto stringa nel container `begins_` e deve essere incrementato il contatore di `multiplicity_`.



> Nota
> Se il container utilizzato è ad esempio un vettore si ha la possibilità di fare aggiunte "sul fondo". In questo modo si riesce a mantenere il primo indice, che identifica la prima occorrenza del k-mer all'interno dell'alfabeto, come l'elemento presente nella prima cella del container`begins_[0]`

##### KmerHandler

```js
object KmersHandler {
    
    // mantine la stringa prese in esame per questo gene
    alphabet_

    // mantiene il valore del k per il calcolo dei mer
    k_
    
    // container che mantiene l'insieme di tutti i Kmer all'interno di un singolo gene
    // ha una struttura chiave(intero)-valore(Kmer*) 
    dictionary_
}
```

La struttuara di un Kmer si occupa di tenere traccia di:
- `alphabet_`, ovvero il la sequenza del gene
- `k_` ovvero la dimensione della sottostringa per il calcolo dei Kmers
- `dictionary_`, ovvero un container che si occupa di mantere tutti i kmers trovati

`dictionary_` viene implementato sfruttando la una coppia chiave(intero)-valore(Kmer*) avendo così la possibilità di tenere traccia dei k-mers ad un costo contenuto:
- chiave: costo di un unsigned long
- Kmer: costo di un puntatore ad un oggetto Kmer

La chiave permette di essere univoca per ogni Kmer dato che esiste un solo k-mer che inizia ad indice `i` nella stringa `alphabet_` e ha lunghezza `k_`. Questa univocità sfrutta il ragionamento sopra descritto nella struttura del Kmer.

#### Algoritmi

##### Kmer
L'oggetto Kmer non presenta attualmente funzionalità interessanti

##### KmersHandler

```js
procedure calculateKmers()

    // container per memorizzare i kMers calcolati nella funzione
    currentKmers = empityMap

    // numero totale di sottostringhe trovabili
    kmersNum = alphabet_.length() - k_ + 1


    for i = 0 to kmersNum do

        // ottenimento di una sottostringa di lunghezza k
        // avendo come primo carattere quello che si trova
        // ad indice i
        ss = alphabet_.substring(i, k_)

        iter = currentKmers.find(ss)

        // se non è stato trovato nessun Kmer corrispondente
        // alla stringa ss
        if iter not found then
            // creazione di un nuovo oggetto kmer relativo alla posizione i
            tmpKmer = Kmer(i)
            // creazione di una coppia composta da
            // sottostringa ss e dal Kmer appena creato 
            currentKmers.insert(pair(ss, tmpKmer))

            // inserimento di una coppia composta da indice i
            // e Kmer appena creato
            dictionary_.insert(pair(i, tmpKmer))
        else then
            // se è stato trovato un kMer che viene identificato dalla sottostringa
            // viene aggiunto l'indice di inizio della sottostringa alla lista degli
            // indici del Kmer (vedi sopra nella sezione relativa alla struttura dati)
            iter.Kmer.addIndex(i)
        end if
end procedure
```
La funzione si occupa di calcolare e di tenere traccia di tutti i k-mers individuabili all'interno della stringa. La variabile tmpKpmer che fa riferimento ad un oggetto Kmer è codiviso sia dalla coppia creata per il container `currentKmers` sia per il container `dictionary_`(vedi sopra nella parte di strutture dati per maggiori informazioni) così da ottimizzare sia l'utilizzo di memoria sia le prestazioni.

Il container sono stati realizzati attraverso una `unordered_map` così da riuscire a sfruttare l'hashing delle sottostrighe per riuscire ad individuare rapidamente il rispettivo oggetto di tipo Kmer

---

## Note importati 

### Kmer
Possibili ottimizzazioni da valutare ed implementare, non è strettamente necessario tenere la variabile `multiplicity_` perché si ha la possibiltà di ricalcolarla guardando la dimensione di `begins_`


### Genomi-geni






---

## Link utili

![Progetto originale](https://github.com/InfOmics/PanDelos)
![File reader](https://github.com/vbonnici/ArcMatch/blob/main/include/FileReader.h)
![Articolo](https://bmcbioinformatics.biomedcentral.com/articles/10.1186/s12859-018-2417-6)
![K-mer wikipedia](https://en.wikipedia.org/wiki/K-mer)


---

## Informazioni utili

Il termine genome si utilizza per indicare sia una sequenza di DNA, G, sia il corrispondente insieme di geni

---

## Note importati 

### Kmer
Possibili ottimizzazioni da valutare ed implementare, non è strettamente necessario tenere la variabile `multiplicity_` perché si ha la possibiltà di ricalcolarla guardando la dimensione di `begins_`