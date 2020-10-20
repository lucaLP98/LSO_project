# LSO_project
Progetto di Laboratorio di Sistemi Operativi. Sviluppato da Pastore Luca per l'Università degli Studi di Napoli Federico II.
Sviluppo di un'applicazione client-server in ambienete UNIX con server concorrente.

<h1> SPECIFICHE </H1>
<H2>DESCRIZIONE SINTETICA</H2>
Realizzare un sistema client-server che consenta a più utenti di prendere e portare oggetti da una locazione di partenza ad una destinazione indicata. Scopo del gioco è consegnare più oggetti alla destinazione.
Si utilizzi il linguaggio C su piattaforma UNIX. I processi dovranno comunicare tramite socket TCP. Corredarel’implementazione di adeguata documentazione.
<BR><BR>
<H2>DESCRIZIONE DETTAGLIATA</H2>
Il server manterrà una rappresentazione dell’ambiente in cui verranno posizionati degli oggetti, delle locazioni e degli ostacoli. L’ambiente sarà rappresentato da una matrice in cui gli utenti si potranno spostare di un passo alla volta nelle quattro direzioni: S, N, E, O oppure prendere o depositare oggetti (es. con azioni P, D). Il server posizionerà nella matrice locazioni, oggetti ed ostacoli in posizioni random. Ogni oggetto avrà associata una locazione in cui portarlo (1, 2, 3, etc.). Ogni utente, una volta connesso al server, verrà posizionato in una posizione random della matrice. All’inizio del gioco gli ostacoli sulla mappa saranno nascosta per l’utente, saranno invece visibili le posizioni degli altri utenti, degli oggetti e le possibili locazioni di consegna. Gli utenti potranno inserirsi nel gioco anche a gioco già iniziato. Dopo ogni passo l’utente riceverà l’informazione sull’effetto proprio movimento: se lo spostamento porterà ad una collisione con un ostacolo oppure con un altro utente, il movimento avrà effetto nullo. In corrispondenza di un oggetto l’utente potrà prendere tale oggetto e leggere la locazione di destinazione. Dovrà quindi portarlo in tale locazione muovendosi sulla mappa per poi posarlo. Quando uno degli utenti avrà consegnato un numero massimo di pacchi, o quando tutti gli oggetti saranno consegnati, il server notificherà agli utenti la fine della sessione e ne genererà una nuova.<BR>
Per accedere al servizio ogni utente dovrà prima registrarsi al sito indicando password e nickname.<BR>
Non c'è un limite a priori al numero di utenti che si possono collegare con il server. Il client consentirà all'utente di collegarsi ad un server di comunicazione, indicando tramite riga di comandoil nome o l'indirizzo IP di tale server e la porta da utilizzare. Una volta collegato ad un server l'utente potrà: registrarsi come nuovo utente o accedere al servizio come utente registrato. Il servizio permetterà all’utente di: spostarsi di una posizione, disconnettersi, vedere la lista degli utenti collegati, vedere gli ostacoli incontrati e la posizione degli altri utenti.<BR>
Il server dovrà supportare tutte le funzionalità descritte nella sezione relativa al client. All'avvio del server, sarà possibile specificare tramite riga di comando la porta TCP sulla quale mettersi in ascolto. Il server sarà di tipo concorrente, ovvero e in grado di servire più client simultanemente. <BR>
Durante il suo regolare funzionamento, il server effettuerà logging delle attivita principali in un file apposito. Ad esempio, memorizzando data e ora di connessione dei client, il loro nome simbolico (se disponibile, altrimenti l'indirizzo IP), data e ora della consegna dei pacchi.

<BR><BR>
<H2>REGOLE GENERALI</H2>
Il server ed il client vanno realizzati in linguaggio C su piattaforma UNIX/Linux. Le comunicazioni tra client e server si svolgono tramite socket TCP. Oltre alle system call UNIX, i programmi possono utilizzare solo la libreria standard del C. E’ sconsigliato l'uso di primitive non coperte dal corso (ad es., code di messaggi) al posto di quelle studiate.
  
<BR><BR>
<H1> DOCUMENTAZIONE </H1>
La documentazione del progetto, comprensiva di un manuale d'utilizzo è visionabile al seguente <a href="https://drive.google.com/file/d/1rjInPeK5flEHrQcl25K1kE5CGI6tG1Ei/view?usp=sharing">LINK</a><br><br>
La documentazione è divisa nei seguenti capitoli:<br>
  <ul>
    <li>Guida all'installazione</li>
    <li>Guida all'utilizzo del server</li>
    <li>Guida all'utilizzo del client</li>
    <li>Codici di errore restituiti in presenza di situazioni anomale</li>
    <li>Protocollo di comunicazione client-server</li>
    <li>Dettagli implementativi</li>
  </ul>
