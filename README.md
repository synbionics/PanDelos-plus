# ParPanDelos

---

## Ottimizzazioni

### K-mers

Dato che più k-mer di un gene sono hanno in comune una sola stringa è possibile mantenere mantenere un solo riferimento alla stringa all'interno di un Handler (`KmersHandler`) di Kmer (`Kmer`).


---

## Strutture dati

### Kmer

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

### KmersHandler

```js
object KmersHandler {
    
    // mantine la stringa presa in esame per questo gene
    alphabet_

    // mantiene il valore del k per il calcolo dei mer
    k_
    
    // container che mantiene l'insieme di tutti i Kmer all'interno di un singolo gene
    // ha una struttura chiave(intero)-valore(Kmer*) 
    dictionary_
}
```

La struttura di un Kmer si occupa di tenere traccia di:
- `alphabet_`, ovvero il la sequenza del gene
- `k_` ovvero la dimensione della sottostringa per il calcolo dei Kmers
- `dictionary_`, ovvero un container che si occupa di mantere tutti i kmers trovati

`dictionary_` viene implementato sfruttando la una coppia chiave(intero)-valore(Kmer*) avendo così la possibilità di tenere traccia dei k-mers ad un costo contenuto:
- chiave: costo di un unsigned long
- Kmer: costo di un puntatore ad un oggetto Kmer

La chiave permette di essere univoca per ogni Kmer dato che esiste un solo k-mer che inizia ad indice `i` nella stringa `alphabet_` e ha lunghezza `k_`. Questa univocità sfrutta il ragionamento sopra descritto nella struttura del Kmer.

### Gene

```js
object Gene {
    // identificativo del genoma
    geneId_

    // mantine la stringa presa in esame per questo gene
    alphabet_

    // mantiene l'insieme dei kmers
    kmers_
}
```
La struttura di un Gene si occupa di tenere traccia di:
- `alphabet_`, ovvero il la sequenza del gene
- `geneId_`, ovvero l'identificativo di un gene
- `kmers_`, ovvero l'insieme dei kmers del gene

`kmers_` va a rappresentare un KmersHandler.
`geneId_` identifica il gene mediante un identificativo di tipo `unsigned long`. Più geni possono presentare lo stesso identificativo, ma data la loro appartenenza ad un genoma, due geni con lo stesso identificativo appartengono a genomi differenti.


### Genome
```js
// identificativo del genoma
genomeId_

// container che mantiene la lista di geni appartenenti al
// genoma 
genes_
```

`genes_` viene implementato sfruttando un'array o un vettore, in questo modo l'indice corrisponde con l'id del gene.

### Homology detector

```js
object HomologyDetector {
    // tiene traccia della dimensione dei kmers
    k_
}
```
L'oggetto HomologyDetector non è stata pensata dal punto di vista di "rappresentazione di dati", ma più per fornire un insieme di funzionalità che hanno come scopo comune il calcolo di omologie. 
---

## Algoritmi

### Kmer
L'oggetto Kmer non presenta attualmente funzionalità interessanti

### KmersHandler

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


```js
procedur getSet()

    set // container che si occupa di mantenere i kmers come sottostringhe

    for each kmer in dictionary_ do
        set.add(alphabet_.substring(kmer), kmer.index())
    done
end procedure
```

La funzione si occupa di fornire un set di tipo chiave valore dove la chiave indica la sottostringa dell'alfabeto corrispondente al kmer e il valore indica l'indice del kmer nel container di kmers.



```js
procedur makeUnion(set1, set2)

    kmersUnion // container che si occupa di mantenere l'unione dei set

    // aggiunta degli elementi contenuti in set1 non presenti in set2
    for each kmer in set1 do
        elem = set2.find(kmer)
        // se set2 non contiene l'elemento presente in set1, l'elemento viene aggiunto
        if elem == NIL then
            kmersUnion.add(kmer)
        end if
    end
    // aggiunge tutti gli elementi presenti in set2 (compresi quelli comuni con set1 esclusi precedentemente)
    for each kmer in set2 do
        kmersUnion.add(kmer)
    end
end procedure
```
Questo metodo si occupa di fare l'unione di 2 set dati


### Gene e Genome

Le varie funzioni messe a disposizione da `Gene` e `Genome` permettono di far "interagire" le "classi esterne" con il KmersHandler.

### Homology detector

```js
procedure calculateSimilarity(gene1, gene2)
    // creazione e calcolo dei vari kmers per gene 1 e gene 2
    
    // creazione di set di tipo chiave-valore dove la chiave indica la sottostringa del kmer
    // e il valore indica l'indice del kmer all'interno del contenitore che si occupa di memorizzare
    // i kmers
    set1 = gene1.getKmers().getSet()
    set2 = gene2.getKmers().getSet()
    
    // calcola l'unione insiemistica dei due set ottenuti sopra
    kmersUnion = makeUnion(set1, set2)

    num = den = 0
    // calcolo del valore di similarità
    for each kmerSubstring in kmersUnion do

        elem1 = set1.find(kmerSubstring)
        elem2 = set2.find(kmerSubstring)

        if elem1 != NIL and elem2 != NIL then
        
            num = num + min(elem1.multiplicity, elem2.multiplicity)
            den = den + max(elem1.multiplicity, elem2.multiplicity)
        else
            if elem1 == NIL and elem2 != NIL then
                den = den + elem2.miltiplicity
            else
                den = den + elem1.miltiplicity
            end if

        end
    
    end for

    simil = num/den

    return simil

end procedure
```
---

## Note importati 


### Generali
È possibile utilizzare degli smart pointers per la gestione automatica della memoria dinamica. Un'unique utilizza 8 byte, uno shared utilizza 16 byte.


### Kmer
Possibili ottimizzazioni, non è strettamente necessario tenere la variabile `multiplicity_` perché si ha la possibiltà di ottenerla guardando la dimensione di `begins_` 



### KmersHandler

È stata utilizzata una reference, avendo accesso diretto all'oggetto non dovendo deferenziare il puntatore, all'alfabeto che sarà successivamente memorizzato all'interno di un gene per ottimizzare le prestazioni

Sarebbe possibile ordinare in ordine alfabetico i kmers trovati così da ridurre il costo computazionale nella fase di ricerca della similarità.


### Indici dei Geni, genomi

Basandosi su questa tabella è possibile diminuire l'impatto sulla memoria dovuto agli indici dei geni-genomi e dei vari indici dei kmers. ![limits](https://cplusplus.com/reference/climits/)
Mediante l'utilizzo di un `unsigned short` si può arrivare ad un numero massimo di ID differenti pari a $65535 = 2^{16}-1$ con un utilizzo di memoria pari ad 2 byte. 

> Nota
> Un `unsigned int` occupa 4 byte e può arrivare ad un numero massimo di ID differenti pari a $65535 = 2^{16}-1$
> Un `unsigned long` occupa 8 byte e può arrivare ad un numero massimo di ID differenti pari a $4294967295 = 2^{32}-1$

### Stringhe

Alcune stringhe possono essere sostituite con `const char*` 

### Multi-thread

Per il multithreading si potrebbe sfruttare una threadpool. Le varie operazioni da eseguire vengono messe su una coda ad accesso condiviso (locks) ed estratte, e svolte.