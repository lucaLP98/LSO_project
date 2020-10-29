/*
 * File : client.c
 * 
 * Autore: Pastore Luca
 * Universita' : Universita' degli studi di Napoli Federico II
 * Corso : Laboratorio di Sistemi Operativi
 * Anno accademico : 2019-20
 */
#include "client.h"

int main(int argc, char *argv[]){
    //controllo che il numero di parametri inseriti sia giusto
    if (argc != 3){
        writeOnSTDOUT("\n\n ERRORE ! Numero di parametri non valido...\n\n");
        exit(-14);
    }

    struct sockaddr_in client_addr; //indirizzo del client
    int client_port_number = atoi(argv[2]); //porta di connessione del client
    char *IP_address = argv[1]; //indirizzo a cui il client intende connettersi

    int my_ID; //ID assegnato al client dal server utilizzato durante il gioco
    char access_mode; //carattere che indica la modalita' di accesso del client: login o registrazione
    char mode; //carattere che rappresenta la mossa scelta dal client durante la fase di gioco  
    char login_done = 'f', buf[100], psw_verify_buf[50], info_users[500], msg[100], winner[50];
    user client_user;
    obstaclesList visited_obstacles = initObtsaclesList();
    playground camp;
    coordinates position, move;
    int remaining_obj, gameover;
    memset(msg, '\0', 50);
    memset(winner, '\0', 50);
    memset(buf, '\0', 100);
    memset(info_users, '\0', 500);

    //inizializzo parametri connessione client
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(client_port_number);
    inet_aton(IP_address, &client_addr.sin_addr);

    //creo socket client
    client_socket = socket(PF_INET, SOCK_STREAM, 0);

    //instauro la connessione con il server
    if( connect(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
        error("\n\n CLIENT : Errore connessione con il server (funzione connect)\n\n", -8);

    if(signal(SIGINT, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGQUIT, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGHUP, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGTSTP, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGPIPE, sigHandlerCloseServer) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGTERM, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);

    //ricevo flag dal server che mi indica se la sessine di gioco è iniziata o meno
    writeOnSTDOUT("\n\n LA SESSIONE DI GIOCO STA PER INIZIARE ...\n\n");
    do{
        if(recv(client_socket, &gameover, sizeof(gameover), 0) == -1) error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
    }while(gameover != 0);

    //INIZIO COMUNICAZIONE CON IL SERVER
    system("clear");
    while(login_done == 'f'){
        system("clear"); menuScreen();
        access_mode = selectClientAccessMode(); //seleziona modalita' di accesso 

        //invio modalità d'accesso al server
        if(send(client_socket, (void *)&access_mode, sizeof(access_mode), 0) == -1) error("\n\n CLIENT : Errore invio dati\n\n", -10); 
    
        cleanString(buf);
        cleanString(client_user.username);
        cleanString(client_user.password);

        switch(access_mode){
            case 'a': //effettuo il login 
                //installazione segnali durante fase di Login
                if(signal(SIGINT, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
                if(signal(SIGQUIT, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
                if(signal(SIGTSTP, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
                if(signal(SIGHUP, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
                if(signal(SIGTERM, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);

                system("clear"); logScreen();

                //inserimento username e password
                writeOnSTDOUT("\n - USERNAME >>> ");
                read(STDIN_FILENO, client_user.username, 50);
                writeOnSTDOUT("\n - PASSWORD >>> ");
                read(STDIN_FILENO, client_user.password, 50);
                client_user.status = 0;
            
                //invio username e password a server
                if(send(client_socket, (void *)&client_user, sizeof(client_user), 0) == -1) error("\n\n CLIENT : Errore invio dati\n\n", -10);
            
                //salvo la risposta del server
                if(recv(client_socket, buf, 100, 0) == -1) error("\n\n CLIENT : Errore ricezione dati\n\n", -11);

                if(buf[0] != 't')   writeOnSTDOUT(buf);
                else{    
                    login_done = 't';
                    writeOnSTDOUT("\n\n Login avvenuto con successo !!!\n");

                    //ricevo ID dal server
                    if(recv(client_socket, &my_ID, sizeof(my_ID), 0) == -1) error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
                    client_user.status = ONLINE;
                }
                cleanString(buf);
                pausa();
            break;

            case 'b': //effettuo registrazione
                system("clear"); regScreen();

                //installazione segnali durante fase di registrazione
                if(signal(SIGINT, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
                if(signal(SIGQUIT, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
                if(signal(SIGHUP, sigHandlerLogReg) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);

                client_user.status = 0;

                //inserimento username e password
                writeOnSTDOUT("\n - Inserire un nuovo USERNAME >>> ");
                read(STDIN_FILENO, client_user.username, 50);

                do{ //verifico corretteza password
                    cleanString(client_user.password);
                    cleanString(psw_verify_buf);
                    writeOnSTDOUT("\n - Inserire una nuova PASSWORD >>> ");
                    read(STDIN_FILENO, client_user.password, 50);
                    writeOnSTDOUT("\n - Ripetere la appena inserita PASSWORD >>> ");
                    read(STDIN_FILENO, psw_verify_buf, 50);

                    if(strcmp(psw_verify_buf, client_user.password) != 0)
                        writeOnSTDOUT("\n\n ATTENZIONE ! Le password non combaciano\n Riprovare ...\n");
                }while(strcmp(psw_verify_buf, client_user.password) != 0);

                //invio username e password a server
                if(send(client_socket, (void *)&client_user, sizeof(client_user), 0) == -1) error("\n\n CLIENT : Errore invio dati\n\n", -10);
            
                //salvo la risposta del server
                if(recv(client_socket, buf, 100, 0) == -1) error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
                writeOnSTDOUT(buf);
                cleanString(buf);
                pausa();
            break;

            case 'c': clientExit(); break; //client esce senza effettura login
        }
    }
    client_user.username[strlen(client_user.username)-1] = '\0'; //Elimino il '\n' alla fine dell'username per stamparlo a video
    client_user.found_objects = 0;

    //imposto segnali per la fase di gioco
    if(signal(SIGINT, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGQUIT, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGTSTP, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12); 
    if(signal(SIGHUP, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);
    if(signal(SIGTERM, sigHandlerMenu) == SIG_ERR) error("\n\n CLIENT : Errore installazione segnale\n\n", -12);

    //ricevo coordinate assegnate
    if(recv(client_socket, &position, sizeof(position), 0) == -1)  error("\n\n CLIENT : ERRORE RICEZIONE CAMPO\n\n", -11);

    //ricevo matrice da server
    if(recv(client_socket, &camp, sizeof(playground), 0) == -1)  error("\n\n CLIENT : ERRORE RICEZIONE CAMPO\n\n", -11);

    //INIZIA IL GIOCO
    do{
        system("clear");
        writeOnSTDOUT("\n\n\t**** Utente : ");
        writeOnSTDOUT(client_user.username);
        writeOnSTDOUT(" - Oggetti trovati : ");
        sprintf(buf, "%d ****\n\n", client_user.found_objects);
        writeOnSTDOUT(buf);

        //stampo campo da gioco aggiornato
        printMatrix_client(camp, visited_obstacles, my_ID, msg);

        //seleziono mossa client
        mode = clientMenu();

        //comunico mossa al server
        if(send(client_socket, &mode, sizeof(mode), 0) == -1)   error("\n\n CLIENT : Errore invio mossa\n\n", -10);

        switch(mode){
            //Visualizzare utenti connessi e loro stato nel gioco
            case 'A' : case 'a' :
                if(recv(client_socket, info_users, 500, 0) == -1)  error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
                
                writeOnSTDOUT("\n\n ********** Informazioni utenti attualmente in gioco **********\n\n");
                writeOnSTDOUT(info_users);
                pausa();
            break;

            //Visualizzare il numero di oggetti rimanenti (compresi quelli attualmente posseduti da utenti)
            case 'B' : case 'b' :
                if(recv(client_socket, &remaining_obj, sizeof(remaining_obj), 0) == -1)  error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
                
                if(remaining_obj > 0){
                    sprintf(buf, "%d", remaining_obj);
                    writeOnSTDOUT("\n\n\t\t*********************** RIMANGONO ");
                    writeOnSTDOUT(buf);
                    writeOnSTDOUT(" OGGETTI DA TROVARE ***********************\n\n");
                    cleanString(buf);
                    pausa();
                }
            break;

            //Logout e uscita dal gioco
            case 'C' : case 'c' : clientExit(); break;

            //gestione movimento
            default:
                cleanString(buf);

                //determino coordinate movimento (utilizzate per salvare posizioni degli eventuali ostacoli incrontrati)
                switch(mode){
                    case 'N' : case 'n' :
                        move.row = position.row - 1;
                        move.column = position.column;
                    break;

                    case 'S' : case 's' :
                        move.row = position.row + 1;
                        move.column = position.column;
                    break;

                    case 'O' : case 'o' :
                        move.row = position.row ;
                        move.column = position.column - 1;
                    break;

                    case 'E' : case 'e' :
                        move.row = position.row;
                        move.column = position.column + 1;
                    break;
                }

                //ricevo messaggio server su esito movimento
                if(recv(client_socket, buf, 100, 0) == -1)  error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
                
                if(buf[0] == 'O'){ //il movimento ha incontrato un ostacolo
                    writeOnSTDOUT("\n\n ");
                    writeOnSTDOUT(buf);
                    visited_obstacles = addNewVisitedObtacle(visited_obstacles, move);
                    pausa();
                }else if(buf[0] == 'A' || buf[0] == 'E'){ //il movimento ha incontrato un altor utente oppure un bordo
                    writeOnSTDOUT("\n\n ");
                    writeOnSTDOUT(buf);
                    pausa();
                }else if(buf[0] == 'H'){ //e' stato raccolto un oggetto
                    writeOnSTDOUT("\n\n ********* ");
                    writeOnSTDOUT(buf);
                    writeOnSTDOUT(" *********\n");

                    //ricevo le coordinate della locazione di destinazione dell'oggetto raccolto
                    if(recv(client_socket, msg, 100, 0) == -1)  error("\n\n CLIENT : Errore ricezione dati\n\n", -11);
                    
                    position = move;
                    pausa();
                }else if(buf[0] == 'D'){ //l'oggetto e' stato depositato
                    cleanString(msg);

                    writeOnSTDOUT("\n\n ********* ");
                    writeOnSTDOUT(buf);
                    writeOnSTDOUT(" *********\n");

                    client_user.found_objects += 1;

                    position = move;
                    pausa();
                }else{
                    position = move;
                    for(int i=0;i<5000000;i++); //ciclo utilizzato per rallentare l'esecuzione del client per meno di un secondo, in modo da sincronizzare la ricezione della matrice aggiornata
                }

                cleanString(buf);
            break;
        }

        //ricevo flag di fine partita
        if(gameover == 0)
            if(recv(client_socket, &gameover, sizeof(gameover), 0) == -1)  error("\n\n CLIENT : ERRORE RICEZIONE DATI\n\n", -11);

        //ricevo matrice aggiornata
        if(gameover == 0){
            if(recv(client_socket, &camp, sizeof(playground), 0) == -1)  error("\n\n CLIENT : ERRORE RICEZIONE CAMPO\n\n", -11);
        }
        

    }while(gameover == 0);

    //ricevo username del vincitore
    if(recv(client_socket, winner, 50, 0) == -1)  error("\n\n CLIENT : ERRORE RICEZIONE VINCITORE\n\n", -11);

    system("clear");
    if(strcmp(winner, client_user.username) == 0){   
        writeOnSTDOUT("\n\n ******************** SEI IL VINCITORE ********************\n\n");
    }else{
        writeOnSTDOUT("\n\n ******************** IL VINCITORE E' : ");
        writeOnSTDOUT(winner);
        writeOnSTDOUT(" ********************\n\n");
    }

    close(client_socket);
    writeOnSTDOUT("\n GRAZIE PER AVER GIOCATO CON NOI !!!\n\n _____________________________________________________________________________\n Develop By Luca Pastore\n\n");

    return 0;
}
