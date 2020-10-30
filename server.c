/*
 * File : server.c
 * 
 * Autore: Pastore Luca
 * Universita' : Universita' degli studi di Napoli Federico II
 * Corso : Laboratorio di Sistemi Operativi
 * Anno accademico : 2019-20
 */

//SEZIONE HEADER FILE
#include "server.h"

//MAIN PROGRAM
int main(int argc, char *argv[]){
    //controllo che il numero di parametri inseriti sia giusto
    if(argc != 3){
        writeOnSTDOUT("\n\n ERRORE ! Numero di parametri non valido...\n\n");
        exit(-14);
    }

    srand(time(NULL)); system("clear");
    
    //sezione variabili locali
    struct sockaddr_in server_addr, client_addr;
    int server_port_number = atoi(argv[2]); //porta sulla quale il server si pone in ascolto
    int client_addr_dim, new_socket;
    char *IP_address_server = argv[1]; //indirizzo sul quale il server si pone in ascolto
    client_data data = NULL; //dati da passare al thread di gestione del client
    pthread_t client_thread; //variabile che conterra' il TID del thread che gestira il client

    //impostazione handler per segnali durante la comunicazione con i client
    if(signal(SIGINT, closeServer) == SIG_ERR) error("\n\n SERVER : ERRORE INSTALLAZIONE SEGNALE\n\n", -12); //imposto handler per catturare segnale SIGINT (interruzione da tastiera ctrl-c)
    if(signal(SIGTSTP, closeServer) == SIG_ERR) error("\n\n SERVER : ERRORE INSTALLAZIONE SEGNALE\n\n", -12); //imposto handler per catturare segnale SIGTSTP (interruzione da tastiera ctrl-z)
    if(signal(SIGHUP, closeServer) == SIG_ERR) error("\n\n SERVER : ERRORE INSTALLAZIONE SEGNALE\n\n", -12); //imposto handler per catturare segnale SIGHUP (disconnessione del terminale)
    if(signal(SIGQUIT, closeServer) == SIG_ERR) error("\n\n SERVER : ERRORE INSTALLAZIONE SEGNALE\n\n", -12); //imposto handler per catturare segnale SIGQUIT (quit da tastiera ctrl-\)
    if(signal(SIGTERM, closeServer) == SIG_ERR) error("\n\n SERVER : ERRORE INSTALLAZIONE SEGNALE\n\n", -12); //imposto handler per catturare segnale SIGTERM (TERMINAZIONE GENERATA DAL COMANDO KILL DA TERMINALE)
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) error("\n\n SERVER : ERRORE INSTALLAZIONE SEGNALE\n\n", -12); //imposto handler per catturare segnale SIGPIPE (Chiusura della socket lato client)
    
    //Lettura file utenti e inseriemento dati in ABR dedicato
    createUsersTree("utenti.txt"); 

    //allocazione memoria per l'array di oggetti
    objects = (object_array)malloc(sizeof(struct T_object_array));

    //allocazione e inizializzazione mutex
    initMutex();

    //inizializzo file di log
    dati_log log_buf;
    initLogFile("log.txt");
    strcpy(log_buf.IP_address , "localhost");
    strcpy(log_buf.username, "server");
    strcpy(log_buf.operation, "start_server");
    writeOnLogFile(log_buf);
    
    //setto i parametri di gioco
    setGameParameters();

    //inizializzo sessione di gioco
    initGameSession();

    writeOnSTDOUT("\n\n Parametri partita impostati correttamente !\n\n In attesa di connessioni client\n");
    pausa();
    system("clear");

    //INSTAURAZIONE CONNESSIONE MEDIANTE SOCKET TCP
    //inizializzazione parametri
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_number);
    inet_aton(IP_address_server, &(server_addr.sin_addr));
    memset(&(server_addr.sin_zero), '\0', 8);
    
    //instauro connessione
    if((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 )
        error("\n\n SERVER : Errore nella creazione della socket\n\n", -5);

    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("\n\n SERVER : Errore bind()\n\n", -6);
    
    if(listen(server_socket, INT_MAX) < 0)
        error("\n\n SERVER : Errore LISTEN()\n\n", -7);    

    //stampo campo da gioco
    writeOnSTDOUT("\n\n\t ********** CAMPO DA GIOCO **********\n\n");
    printMatrix_server();

    /**
     * avvio thread di gestione gioco
     * 
     * Tale thread determina la fine di una sessione e il relativo vinvitore
     * e ne avvia un'altra
     */
    pthread_t tid_game; //TID del thread di gestione sessione di gioco
    pthread_create(&tid_game, NULL, gameManagement, NULL);

    //Inizializzo lista che conterrà i tid dei thread che gestiscono le connessioni con i client attivi
    list = initThreadList();
    
    //comunicazione con i client
    while((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_dim)) != -1){
        data = (client_data)malloc(sizeof(client_data_str));
        data->sock_descriptor = new_socket;
        data->client_IP = client_addr;
        if(pthread_create(&client_thread, NULL, gestisciClient, (void *)data) != 0)
            error("\n\n Errore creazione thread di gestione client\n\n", new_socket);
        
        list = insertThread(client_thread, list);
    }

    return 0;
}
