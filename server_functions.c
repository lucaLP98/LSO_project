#include "server.h"

tid_list initThreadList(){
    return NULL;
}

tid_list insertThread(pthread_t new, tid_list l){
    tid_list tmp = (tid_list)malloc(sizeof(struct t_tid_list));

    tmp->tid = new;
    tmp->next = l;

    return tmp;
}

tid_list freeList(tid_list l){
    tid_list tmp;

    while(l != NULL){
        tmp = l;
        l=l->next;
        free(tmp);
    }

    return l;
}

tid_list deleteThread(pthread_t toDelete, tid_list l){
    tid_list prev, cur;
    int found;

    if(l != NULL){
        if(l->tid == toDelete){
            cur = l;
            l = l->next;
            free(cur);
        }else{
            found = 0;
            prev = l;
            cur = l->next;

            while(cur != NULL && found == 0){
                if(cur->tid == toDelete){
                    prev = cur->next;
                    free(cur);
                    found = 1;
                }else{
                    cur = cur->next;
                }
            }
        }
    }

    return l;
}

void createUsersTree(const char file_name[]){
    int file_descriptor;
    char buf[100];
    user userBuff;

    if((user_file_descriptor = open(file_name, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) == -1) 
        error("\n\n SERVER : Errore apertura file utenti...\n\n", -1);

    if(read(user_file_descriptor, buf, 100) == -1)
        error("\n\n SERVER: Errore lettura file utenti...\n\n", -2);

    users = initUsersTree();

    int i = 0, j;
    while(strlen(buf) > 1 && buf[i] != '\0'){
        j = 0;

        while(buf[i] != ' '){
            userBuff.username[j] = buf[i];
            i++; j++;
        }
        userBuff.username[j] = '\0';
        
        i += 1; j = 0;
        
        while(buf[i] != '\n' && buf[i] != ' ' && buf[i] != '\0'){
            userBuff.password[j] = buf[i];
            j++; i++;
        }
        userBuff.password[j] = '\0';
        users = insertNewUser(users, userBuff.username, userBuff.password); 

        cleanString(userBuff.username);
        cleanString(userBuff.password);
        i += 1;
    }
}

int insertPlaygroundDimension(){
    char tmp_buf[50];
    int tmp_dim;

    do{
        writeOnSTDOUT("\n - Inserire la dimensione del campo da gioco >>> ");
        read(STDIN_FILENO, tmp_buf, 50);
        tmp_dim = atoi(tmp_buf);
        if(tmp_dim <= 1)
            writeOnSTDOUT("\n\n ERRORE ! Valore troppo piccolo...\n\n Riprovare >>> ");
        else if(tmp_dim > MAX_MATRIX_DIM)
            writeOnSTDOUT("\n\n ERRORE ! Valore troppo grande...\n\n Riprovare >>> ");
    }while(tmp_dim <=1 || tmp_dim > MAX_MATRIX_DIM);
    
    return tmp_dim;
}

int insertNumberOfElements(int matrix_dimension, char msg[]){
    int number_of_elements;
    char buff[10];

    writeOnSTDOUT(msg);
    do{
        read(STDIN_FILENO, (void *)buff, 10);
        number_of_elements = atoi(buff);
        if(number_of_elements <= 0 || number_of_elements >= matrix_dimension*matrix_dimension)
            writeOnSTDOUT("\n ERRORE ! Valore non valido\n\n Riprovare >>> ");
    }while(number_of_elements <= 0 || number_of_elements >= matrix_dimension*matrix_dimension);

    return number_of_elements;
}

void closeServer(){
    //aggiornamento file di log con chiusura server
    dati_log buf_log;
    strcpy(buf_log.IP_address , "localhost");
    strcpy(buf_log.username, "server");
    strcpy(buf_log.operation, "close_server");
    writeOnLogFile(buf_log);

    /**
     * mi occupo di terminare il thread di gestione gioco
     * essendo quest'ultimo bloccato in attesa passiva della 
     * condition variable cond
     */
    pthread_mutex_lock(flag_gameover_mutex);
    gameover = 2;
    pthread_mutex_unlock(flag_gameover_mutex);
    pthread_mutex_lock(cond_mutex);
    pthread_cond_broadcast(cond);
    pthread_mutex_unlock(cond_mutex);

    //Distruzione mutex
    pthread_mutex_destroy(user_file_mutex);
    pthread_mutex_destroy(log_file_mutex);
    pthread_mutex_destroy(object_array_mutex);
    pthread_mutex_destroy(matrix_mutex);
    pthread_mutex_destroy(user_mutex);
    pthread_mutex_destroy(remaining_obj_mutex);
    pthread_mutex_destroy(flag_gameover_mutex);
    pthread_mutex_destroy(tid_list_mutex);
    pthread_mutex_destroy(cond_mutex);

    //distruzione condition variable
    pthread_cond_destroy(cond);

    //deallocazione memoria occupata dai mutex
    free(user_file_mutex);
    free(log_file_mutex);
    free(object_array_mutex);
    free(matrix_mutex);
    free(user_mutex);
    free(remaining_obj_mutex);
    free(flag_gameover_mutex);
    free(cond_mutex);
    free(tid_list_mutex);
    
    //deallocazione memoria occupata da condition variable
    free(cond);

    //deallocazione risorse server
    free(objects->arr);
    free(objects);
    free(users);
    freeList(list);

    //chiusura socket
    close(server_socket);

    //chiusura file di Log
    close(log_file_desriptor);

    //chiusura del file utenti
    close(user_file_descriptor);

    system("clear");
    writeOnSTDOUT("\n CHIUSURA SERVER AVVENUTA CON SUCCCESSO !");
    writeOnSTDOUT("\n\n ______________________________________________________________________\n Develop By Pastore Luca\n Matr. N86002599\n\n");
    exit(0);
}

void initLogFile(char *file_name){
    if((log_file_desriptor = open("log.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR |S_IWUSR )) == -1)
        error("\n\n SERVER : Errore apertura file utenti\n\n", -1);

    char msg1[] = "\nINDIRIZZO IP\t\t";
    char msg2[] = "UTENTE\t\t\t";
    char msg3[] = "OPERAZIONE\t\t";
    char msg4[] = "DATA\t\t\t";
    char msg5[] = "ORARIO\n";

    if( write(log_file_desriptor, msg1, strlen(msg1)) == -1)
        error("\n\n SERVER : Errore scrittura intestazione file di Log\n\n", -3);

    if( write(log_file_desriptor, msg2, strlen(msg2)) == -1)
        error("\n\n SERVER : Errore scrittura intestazione file di Log\n\n", -3);

    if( write(log_file_desriptor, msg3, strlen(msg3)) == -1)
        error("\n\n SERVER : Errore scrittura intestazione file di Log\n\n", -3);

    if( write(log_file_desriptor, msg4, strlen(msg4)) == -1)
        error("\n\n SERVER : Errore scrittura intestazione file di Log\n\n", -3);
    
    if( write(log_file_desriptor, msg5, strlen(msg5)) == -1)
        error("\n\n SERVER : Errore scrittura intestazione file di Log\n\n", -3);
}

void writeOnLogFile(dati_log dati){
    date current = currentDate();
    char date_str[50], time_str[50], tab[] = "\t\t";

    sprintf(date_str, "%d/%d/%d\t\t", current.day, current.month, current.year);
    sprintf(time_str, "%d:%d:%d\n", current.hours, current.minutes, current.seconds);

    pthread_mutex_lock(log_file_mutex); //inizio sezione critica per aggiornare file log

    if(write(log_file_desriptor, dati.IP_address, strlen(dati.IP_address)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);
    if(write(log_file_desriptor, tab, strlen(tab)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);

    if(write(log_file_desriptor, dati.username, strlen(dati.username)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);
    if(write(log_file_desriptor, "\t\t\t", 3) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);

    if(write(log_file_desriptor, dati.operation, strlen(dati.operation)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);
    if(write(log_file_desriptor, tab, strlen(tab)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);

    if(write(log_file_desriptor, date_str, strlen(date_str)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);
    
    if(write(log_file_desriptor, time_str, strlen(time_str)) < 0)
        error("\n\n SERVER : Errore scrittura su file di log\n\n", -3);

    pthread_mutex_unlock(log_file_mutex); //fine sezione critica
}

void initMutex(){
    //allocazione dinamica memoria per mutex
    user_file_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    log_file_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    matrix_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    object_array_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    user_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    remaining_obj_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    flag_gameover_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    cond_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); 
    tid_list_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); 

    //inizializzazione dei mutex
    if(pthread_mutex_init(user_file_mutex, NULL) != 0)  
        error("\n\n SERVER : Errore inizializzazione mutex associato a file utenti\n", -4);

    if(pthread_mutex_init(matrix_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex assocaito a matrice\n", -4);
    
    if(pthread_mutex_init(remaining_obj_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex assocaito agli ogetti rimanenti\n", -4);

    if(pthread_mutex_init(log_file_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex associato a file di log\n", -4);

    if(pthread_mutex_init(object_array_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex associato ad array degli oggetti\n", -4);
    
    if(pthread_mutex_init(user_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex associato ad ABR degli utenti\n", -4);

    if(pthread_mutex_init(flag_gameover_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex associato al flag di fine gioco\n", -4);
    
    if(pthread_mutex_init(cond_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex associato a condition variable\n", -4);
    
    if(pthread_mutex_init(tid_list_mutex, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione mutex associato a condition variable\n", -4);
   
    //inizializzazione condition variable
    cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    
    if(pthread_cond_init(cond, NULL) != 0)
        error("\n\n SERVER : Errore inizializzazione condition variable\n", -4);
}

void setGameParameters(){
    menuScreen();

    //Inserimento da tastiera della dimensione matrice del campo da gioco
    camp.dimension = insertPlaygroundDimension();

    //inserimento numero di oggetti e allocazione array
    pthread_mutex_lock(object_array_mutex);
    objects->number_of_object = insertNumberOfElements(camp.dimension, "\n - Inserire numero di oggetti da collocare >>> ");
    objects->arr = (object *)realloc(objects->arr, objects->number_of_object * sizeof(object));
    pthread_mutex_unlock(object_array_mutex);
    
    //imposto oggetti rimanenti
    pthread_mutex_lock(remaining_obj_mutex);
    remaining_objects = objects->number_of_object;
    pthread_mutex_unlock(remaining_obj_mutex);

    //inserimento numero massimo di oggetti che un client deve consegnare per vincere
    char buf[10];
    int tmp;
    writeOnSTDOUT("\n - Inserire massimo numero di oggetti da consegnare >>> ");
    do{
        read(STDOUT_FILENO, buf, sizeof(buf));
        tmp = atoi(buf);
        if(tmp <= 0 || tmp > objects->number_of_object)
            writeOnSTDOUT("\n ERRORE ! Valore non valido\n\n Riprovare >>> ");
    }while(tmp <= 0 || tmp > objects->number_of_object);
    max_number_of_object = tmp;

    //inserimento numero di ostacoli
    number_of_obstacles = insertNumberOfElements(camp.dimension, "\n - Inserire numero di ostacoli da collocare >>> ");
}

void generatePlayground(){
    int i, j, row, column;

    //riempio il campo di caselle vuote indicate dall'intero -1
    for(i=0;i<camp.dimension;i++){
        for(j=0;j<camp.dimension;j++){
            camp.matrix[i][j].user_mov = -1;
            camp.matrix[i][j].content = -1;
        }
    }

    //colloco gli ostacoli sul campo in posizione random, indicati dall'intero -2
    for(i=0;i<number_of_obstacles;i++){
        row = rand()%camp.dimension;
        column = rand()%camp.dimension;
        camp.matrix[row][column].content = -2;
    }

    //colloco gli oggetti e le locazioni sul campo in posizioni random
    //gli oggetti saranno indicati da un intero intero >= 0
    //le locazioni saranno indicate dall'intero -3
    for(i=0;i<objects->number_of_object;i++){
        //posiziono oggetto
        do{
            row = rand()%camp.dimension;
            column = rand()%camp.dimension;
        }while(camp.matrix[row][column].content != -1);
        camp.matrix[row][column].content = i;

        //posiziono locazione di destinazione dell'oggetto appena collocato
        do{
            row = rand()%camp.dimension;
            column = rand()%camp.dimension;
        }while(camp.matrix[row][column].content != -1);
        camp.matrix[row][column].content = -3;

        objects->arr[i].destination_location.row = row;
        objects->arr[i].destination_location.column = column;
        objects->arr[i].status = NOT_FOUND;
    }
}

void initGameSession(){
    
    memset(user_winner, '\0', 50);

    //genero il campo da gioco
    pthread_mutex_lock(matrix_mutex);
    generatePlayground();
    pthread_mutex_unlock(matrix_mutex);

    dati_log log_buf;
    strcpy(log_buf.IP_address , "localhost");
    strcpy(log_buf.username, "server");
    strcpy(log_buf.operation, "start_session");
    writeOnLogFile(log_buf);

    gameover = 0;
}

date currentDate(){
    date current;
    struct tm *timePtr;
    time_t calTime = time(NULL);
    timePtr = localtime(&calTime);

    if(timePtr == NULL) error("\n\n SERVER : Errore determinazione data corrente\n\n", -9);      

    current.day = timePtr->tm_mday;
    current.month = timePtr->tm_mon + 1;
    current.year = (timePtr->tm_year - 100) + 2000; //numero di anni dal 1900
    current.hours = timePtr->tm_hour;
    current.minutes = timePtr->tm_min;
    current.seconds = timePtr->tm_sec;

    return current;
}

void printMatrix_server(){
    int i, j;

    printf("\n");
    pthread_mutex_lock(matrix_mutex);
    for(i=0;i<camp.dimension;i++){
        for(j=0;j<camp.dimension;j++){
            if(camp.matrix[i][j].user_mov <= -4){
                printf("\t%5c", 'U');
            }else{
                switch(camp.matrix[i][j].content){
                    case -1: printf("\t%5c", '-'); break; //casella vuota
                    case -2: printf("\t%5c", 'X'); break; //ostacolo
                    case -3: printf("\t%5c", 'L'); break; //locazione
                    default: printf("\t%5d", camp.matrix[i][j].content); break; //oggetti
                }
            }
        }
        printf("\n\n");
    }
    pthread_mutex_unlock(matrix_mutex);

    writeOnSTDOUT(" Listato utenti attualmente connessi :\n\n");
    pthread_mutex_lock(user_mutex);
    inOrderViewUsers(users);
    pthread_mutex_unlock(user_mutex);

    writeOnSTDOUT("\n\n\n Premere CTRL-C per interrompere l'esecuzione in qualsiasi momento\n\n");
}

void *gestisciClient(void *arg){
    client_data data = (client_data)arg;
    char client_access_mode, mode, login_done = 'f', buf[100], ip_buf[100], info_user[500];
    user client_user_buf;
    usersTree client_user;
    dati_log log_buf;
    int client_ID, move_ok = 0;
    coordinates temp_coord;

    cleanString(info_user);

    //comunico al client se la sessione di gioco è iniziata o meno
    do{
        if(gameover != 0)
            if(send(data->sock_descriptor, &gameover, sizeof(gameover), 0) == -1) error("\n\n SERVER : Errore invio dati\n\n", -10);
    }while(gameover != 0);
    if(send(data->sock_descriptor, &gameover, sizeof(gameover), 0) == -1) error("\n\n SERVER : Errore invio dati\n\n", -10);

    //GESTIONE LOGIN E REGISTRAZIONE CLIENT
    while(login_done == 'f'){
        //ricevo la modalita' di accesso da parte del client
        if(recv(data->sock_descriptor, (void *)&client_access_mode, sizeof(client_access_mode), 0) == -1) error("\n\n SERVER : Errore ricezione dati\n\n", -11);
        
        switch(client_access_mode){
            case 'a': /***** client effettua il login *****/    
                //ricezione credenziali utente
                if(recv(data->sock_descriptor, (void *)&client_user_buf, sizeof(client_user_buf), 0) == -1) error("\n\n SERVER : Errore ricezione dati\n\n", -11);

                //gestione caduta/chiusura client durante fase di login
                if(client_user_buf.status == -1){
                    close(data->sock_descriptor);
                    pthread_exit(NULL);
                }

                client_user_buf.username[strlen(client_user_buf.username)-1] = '\0';
                client_user_buf.password[strlen(client_user_buf.password)-1] = '\0';

                pthread_mutex_lock(user_mutex); //inizio sezione critica per accesso in lettura ABR utenti
                client_user = searchUsers(users, client_user_buf.username);
                pthread_mutex_unlock(user_mutex); //fine sezione critica

                //controllo che l'utente cercato sia registrato al sistema
                if(client_user != NULL){
                    //controllo che l'utente non sia gia' loggato su un altro client
                    if(client_user->User.status == OFFLINE){
                        //controllo correttezza della password
                        if(strcmp(client_user_buf.password, client_user->User.password) == 0){
                            //password corretta, accesso consentito
                            client_user->User.status = ONLINE; //imposto stato online per utente

                            //comunico avvenuto login al client
                            login_done = 't';
                            if(send(data->sock_descriptor, &login_done, sizeof(login_done), 0) == -1) error("\n\n SERVER : Errore invio dati\n\n", -10);

                            //aggiorno file di log
                            strcpy(log_buf.operation, "login_client");
                            strcpy(log_buf.username, client_user_buf.username);
                            inet_ntop(AF_INET, &data->client_IP.sin_addr, log_buf.IP_address, sizeof(log_buf.IP_address));
                            writeOnLogFile(log_buf);
                            cleanString(log_buf.operation);

                            ///assegno ID a client per mostrarlo su matrice
                            client_ID = data->sock_descriptor * -1;
                            client_user->User.userID = client_ID;
                            //comunico ID al client
                            sleep(1);
                            if(send(data->sock_descriptor, &client_ID, sizeof(client_ID), 0) == -1) error("\n\n SERVER : Errore invio dati\n\n", -10);
                        }else{
                            //password errata, accesso negato
                            strcpy(buf, "\n PASSWORD ERRATA !!!...\n Riprovare\n");
                            send(data->sock_descriptor, buf, 100, 0);
                            cleanString(buf);
                        }
                    }else{
                        //l'utemte ha gia' effettuato il login ed e' attualmente in partita su un altro client
                        strcpy(buf, "\n UTENTE GIA' IN GIOCO DA UN ALTRO CLIENT...\n Riprovare\n");
                        send(data->sock_descriptor, buf, 100, 0);
                        cleanString(buf);
                    }
                }else{
                    //utente non esistente, username errato
                    strcpy(buf, "\n UTENTE NON ESISTENTE...\n Riprovare\n");
                    send(data->sock_descriptor, buf, 100, 0);
                    cleanString(buf);
                }
            break; /***** fine login *****/

            case 'b': /***** client si registra al sistema *****/
                if(recv(data->sock_descriptor, (void *)&client_user_buf, sizeof(client_user_buf), 0) == -1) error("\n\n SERVER : Errore ricezione dati\n\n", -11);

                if(client_user_buf.status == -1){
                    close(data->sock_descriptor);
                    pthread_exit(NULL);
                }

                client_user_buf.username[strlen(client_user_buf.username)-1] = '\0';
                client_user_buf.password[strlen(client_user_buf.password)-1] = '\0';

                //verifico che l'utente non sia gia' iscritto al sistema
                pthread_mutex_lock(user_mutex); //inizio sezione critica per accesso ad ABR utenti
                client_user = searchUsers(users, client_user_buf.username);
                pthread_mutex_unlock(user_mutex); //fine sezione critica
            
                if(client_user == NULL){
                    //aggiungere utente a file 
                    pthread_mutex_lock(user_file_mutex); //inizio sezione critica per aggiungere utente a file utenti       
                    write(user_file_descriptor, client_user_buf.username, strlen(client_user_buf.username));
                    write(user_file_descriptor, " ", 1);
                    write(user_file_descriptor, client_user_buf.password, strlen(client_user_buf.password));
                    write(user_file_descriptor, "\n", 1);
                    pthread_mutex_unlock(user_file_mutex); //fine sezione critica

                    //aggiorno file di log
                    strcpy(log_buf.operation, "registration");
                    strcpy(log_buf.username, client_user_buf.username);
                    inet_ntop(AF_INET, &data->client_IP.sin_addr, log_buf.IP_address, sizeof(log_buf.IP_address));
                    writeOnLogFile(log_buf);
                    cleanString(log_buf.operation);

                    //aggiungere utente ad albero
                    pthread_mutex_lock(user_mutex); //inizio sezione critica per aggiungere nuovo utente ad ABR utenti
                    insertNewUser(users, client_user_buf.username, client_user_buf.password);
                    pthread_mutex_unlock(user_mutex); //fine sezione critica

                    //avviso il client dell'avvenuta registrazione
                    strcpy(buf, "\n REGISTRAZIONE AVVENUTA CON SUCCESSO\n");

                    cleanString(client_user_buf.username);
                    cleanString(client_user_buf.password);
                }else{
                    //avviso il client che esiste gia' un utente con username selezionato
                    strcpy(buf, "\n UTENTE GIA' ISCRITTO AL SISTEMA...\n Riprovare\n");
                }

                if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati\n\n", -10);
            break; /***** fine registrazione *****/

            case 'c': //il client ha deciso di interrompere la comunicazione senza aver effettuato il login
                pthread_mutex_lock(tid_list_mutex);
                list = deleteThread(pthread_self(), list);
                pthread_mutex_unlock(tid_list_mutex);
                close(data->sock_descriptor);
                pthread_exit(NULL);
            break;
        }
    }

    //LOGIN AVVENUTO CON SUCCESSO

    //colloco l'utente sul campo da gioco
    pthread_mutex_lock(matrix_mutex);
    do{
        //generazione random coordinate
        temp_coord.row = rand()%camp.dimension;
        temp_coord.column = rand()%camp.dimension;
    //mi assicuro che l'utente sia collocato in una xasella non occupata da latri utenti o da un oggetti/locazione/ostacolo    
    }while(camp.matrix[temp_coord.row ][temp_coord.column].content != -1 || camp.matrix[temp_coord.row ][temp_coord.column].user_mov != -1);
    camp.matrix[temp_coord.row ][temp_coord.column].user_mov = client_ID;
    pthread_mutex_unlock(matrix_mutex);

    //aggiorno coordinate utente
    client_user->User.current_position = temp_coord;

    //comunico ad utente posizione
    if(send(data->sock_descriptor, &temp_coord, sizeof(temp_coord), 0) == -1) error("\n\n SERVER: ERRORE INVIO CAMPO DA GIOCO\n\n", -10);
    
    //comunico ad utente matrice
    pthread_mutex_lock(matrix_mutex);
    if(send(data->sock_descriptor, &camp, sizeof(playground), 0) == -1) error("\n\n SERVER: ERRORE INVIO CAMPO DA GIOCO\n\n", -10);
    pthread_mutex_unlock(matrix_mutex);

    //INIZIA LA PARTITA DEL CLIENT
    do{
        system("clear");
        writeOnSTDOUT("\n\n\t ********** CAMPO DA GIOCO **********\n\n");
        printMatrix_server();
        
        //ricevo mossa utente
        if(recv(data->sock_descriptor, &mode, sizeof(mode), 0) == -1) error("\n\n SERVER : Errore ricezione mossa\n\n", -11);

        switch(mode){
            //Visualizzare utenti connessi e loro stato nel gioco
            case 'A' : case 'a' :
                pthread_mutex_lock(user_mutex);
                usersInfoIntoString(users, info_user);
                if(send(data->sock_descriptor, info_user, 500, 0) == -1) error("\n\n SERVER: Errore invio dati\n\n", -10);
                pthread_mutex_unlock(user_mutex);
                cleanString(info_user);
            break;

            //Visualizzare il numero di oggetti rimanenti (compresi quelli attualmente posseduti da utenti)
            case 'B' : case 'b' :
                pthread_mutex_lock(remaining_obj_mutex);
                if(send(data->sock_descriptor, &remaining_objects, sizeof(remaining_objects), 0) == -1)
                    error("\n\n SERVER : Errore invio dati\n\n", -10);
                pthread_mutex_unlock(remaining_obj_mutex);
            break;

            //Logout e uscita dal gioco
            case 'C' : case 'c' :
                pthread_mutex_lock(matrix_mutex);
                if(client_user->User.object != -1){
                    //se l'utente che effettua il logaut porta con se un oggetto, posiziono l'oggetto di nuovo sul campo
                    if(camp.matrix[client_user->User.current_position.row][client_user->User.current_position.column].content == -1)
                        //se l'ultima posizione dell'utente che ha effettuato il logout è vuota poso l'oggetto
                        camp.matrix[client_user->User.current_position.row][client_user->User.current_position.column].content = client_user->User.object;
                    else{
                        //se l'ultima posizione dell'utente che ha effettuato il logout contiene una locazione o un altro oggetto, allora
                        //genero delle coordinate random dove andro a posizionare l'oggetto 
                        coordinates tmp;
                        do{
                            tmp.row = rand()%camp.dimension;
                            tmp.column = rand()%camp.dimension;
                        }while(camp.matrix[tmp.row][tmp.column].content != -1);
                        camp.matrix[tmp.row][tmp.column].content = client_user->User.object;
                    }
                }
                camp.matrix[client_user->User.current_position.row][client_user->User.current_position.column].user_mov = -1;
                pthread_mutex_unlock(matrix_mutex);

                client_user->User.status = OFFLINE;
                client_user->User.userID = 0;
                client_user->User.object = -1;

                pthread_mutex_lock(tid_list_mutex);
                list = deleteThread(pthread_self(), list);
                pthread_mutex_unlock(tid_list_mutex);

                //aggiorno file di log
                strcpy(log_buf.operation, "logout_client");
                writeOnLogFile(log_buf);
                cleanString(log_buf.operation);

                //chiusura socket e uscita dal thread
                close(data->sock_descriptor);
                system("clear");
                writeOnSTDOUT("\n\n\t ********** CAMPO DA GIOCO **********\n\n");
                printMatrix_server();
                pthread_exit(NULL);
            break;

            //gestione movimento sulla mappa
            default:
                switch(mode){
                    //movimento di una casella verso l'alto
                    case 'N' : case 'n' :
                        temp_coord.row = client_user->User.current_position.row - 1;
                        temp_coord.column = client_user->User.current_position.column;

                        //controllo che la mossa non superi il limite superiore del campo
                        if(temp_coord.row < 0){
                            strcpy(buf, "ERRORE ! Superati i limiti del campo !\n Mossa Nulla\n\n");
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                            move_ok = 0;
                        }else
                            move_ok = 1;
                    break;

                    //movimento di una casella verso il basso
                    case 'S' : case 's' :
                        temp_coord.row = client_user->User.current_position.row + 1;
                        temp_coord.column = client_user->User.current_position.column;

                        //controllo che la mossa non superi il limite inferiore del campo
                        if(temp_coord.row >= camp.dimension ){
                            strcpy(buf, "ERRORE ! Superati i limiti del campo !\n Mossa Nulla\n\n");
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                            move_ok = 0;
                        }else
                            move_ok = 1;
                    break;

                    //movimento di una casella verso sinistra
                    case 'O' : case 'o' :
                        temp_coord.row = client_user->User.current_position.row;
                        temp_coord.column = client_user->User.current_position.column - 1;

                        //controllo che la mossa non superi il limite sinistro del campo
                        if(temp_coord.column < 0 ){
                            strcpy(buf, "ERRORE ! Superati i limiti del campo !\n Mossa Nulla\n\n");
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                            move_ok = 0;
                        }else
                            move_ok = 1;
                    break;

                    //movimento di una casella verso destra
                    case 'E' : case 'e' :
                        temp_coord.row = client_user->User.current_position.row;
                        temp_coord.column = client_user->User.current_position.column + 1;

                        //controllo che la mossa non superi il limite destro del campo
                        if(temp_coord.column >= camp.dimension ){
                            strcpy(buf, "ERRORE ! Superati i limiti del campo !\n Mossa Nulla\n\n");
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                            move_ok = 0;
                        }else
                            move_ok = 1;
                    break;
                }

                //a questo punto la mossa non va oltre i limiti del campo
                if(move_ok == 1){
                    pthread_mutex_lock(matrix_mutex);
                    //controllo che la mossa non porti alla collisione con un oggetto
                    if(camp.matrix[temp_coord.row][temp_coord.column].content == -2){
                        strcpy(buf, "OPS ! Hai incontrato un ostacolo !\n Mossa Nulla\n\n");
                        if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                    
                    //controllo che la mossa non porti alla collisione con un altro utente
                    }else if(camp.matrix[temp_coord.row][temp_coord.column].user_mov != -1){
                        strcpy(buf, "ATTENZIONE ! Casella occupata da un altro utente !\n Mossa Nulla\n\n");
                        if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);

                    }else{ //a questo punto la mossa e' andata a buon fine
                        camp.matrix[client_user->User.current_position.row][client_user->User.current_position.column].user_mov = -1;
                        client_user->User.current_position = temp_coord;
                        camp.matrix[temp_coord.row][temp_coord.column].user_mov = client_user->User.userID;
                       
                        //il movimento porta l'utente su un oggetto
                        if(camp.matrix[temp_coord.row][temp_coord.column].content >= 0 && client_user->User.object == -1){
                            //l'utente non porta con se un altro oggetto (ogni utente puo' portare un oggetto alla volta)

                            //l'utente prende l'oggetto
                            client_user->User.object = camp.matrix[temp_coord.row][temp_coord.column].content;

                            //comunico all'utente che ha raccolto un oggetto
                            strcpy(buf, "HAI RACCOLTO UN OGGETTO");
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                            cleanString(buf);

                            //comunico all'utente la locazione di destinazione dell'oggetto
                            pthread_mutex_lock(object_array_mutex);
                            sprintf(buf, "Locazione di destinazione : R. %d - C. %d", objects->arr[camp.matrix[temp_coord.row][temp_coord.column].content].destination_location.row, objects->arr[camp.matrix[temp_coord.row][temp_coord.column].content].destination_location.column);
                            pthread_mutex_unlock(object_array_mutex);
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                                
                            //aggiorno file di log
                            strcpy(log_buf.operation, "object_taken");
                            writeOnLogFile(log_buf);
                            cleanString(log_buf.operation);

                            //la casella della matrice che conteneva l'oggetto diventa vuota
                            camp.matrix[temp_coord.row][temp_coord.column].content = -1;
                            
                        //il movimento porta l'utente su una locazione
                        }else if(camp.matrix[temp_coord.row][temp_coord.column].content == -3 && client_user->User.object != -1){
                            pthread_mutex_lock(object_array_mutex);
                            if(objects->arr[client_user->User.object].destination_location.row == temp_coord.row && objects->arr[client_user->User.object].destination_location.column == temp_coord.column){
                                //l'utente e' arrivato sulla locazione di destinazione e l'oggetto viene consegnato
                                objects->arr[client_user->User.object].status = FOUND;
                                client_user->User.object = -1;
                                client_user->User.found_objects += 1;
                                strcpy(buf, "DEPOSITATO !");
                                if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                                cleanString(buf);

                                //aggiorno file di log
                                strcpy(log_buf.operation, "obje_deposited");
                                writeOnLogFile(log_buf);
                                cleanString(log_buf.operation);

                                //aggiorno numero di oggetti rimanenti
                                pthread_mutex_lock(remaining_obj_mutex);
                                remaining_objects -= 1;
                                pthread_mutex_unlock(remaining_obj_mutex);

                                /**
                                 * risveglio thread che si occupa della gestione della sessione di gioco.
                                 * 
                                 * se la condizione è vera, allora la partita è finita:
                                 * il thread risvegliato dichiarera il vincitore e iniziera' 
                                 * una nuova sessione
                                 */
                                pthread_mutex_lock(remaining_obj_mutex);
                                if(remaining_objects == 0 || client_user->User.found_objects == max_number_of_object){
                                    pthread_mutex_lock(cond_mutex);
                                    pthread_cond_broadcast(cond);
                                    pthread_mutex_unlock(cond_mutex);

                                    pthread_mutex_lock(flag_gameover_mutex);
                                    gameover = 1;
                                    pthread_mutex_unlock(flag_gameover_mutex);
                                } 
                                pthread_mutex_unlock(remaining_obj_mutex);
                            }else{
                                strcpy(buf, "NORMALE");
                                if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                            }
                            pthread_mutex_unlock(object_array_mutex);
                        }else{
                            strcpy(buf, "NORMALE");
                            if(send(data->sock_descriptor, buf, 100, 0) == -1) error("\n\n SERVER : Errore invio dati !!\n\n", -10);
                        }
                    }   
                    pthread_mutex_unlock(matrix_mutex);
                }
                cleanString(buf);
            break;
        }
        //comunico al client il flag che indica periodicamente se la partita e' finita o meno
        pthread_mutex_lock(flag_gameover_mutex);
        if(send(data->sock_descriptor, &gameover, sizeof(gameover), 0) == -1) error("\n\n SERVER: Errore invio dati\n\n", -10);
        pthread_mutex_unlock(flag_gameover_mutex);

        //comunico matrice aggiornata al client
        if(gameover == 0){
            pthread_mutex_lock(matrix_mutex);
            if(send(data->sock_descriptor, &camp, sizeof(playground), 0) == -1) error("\n\n SERVER: ERRORE INVIO CAMPO DA GIOCO\n\n", -10);
            pthread_mutex_unlock(matrix_mutex);
        }
    }while(gameover == 0);

    //comunico il vincitore al client
    while(user_winner[0] == '\0');
    if(send(data->sock_descriptor, user_winner, 50, 0) == -1) error("\n\n SERVER: Errore invio vincitore\n\n", -10);

    //aggiorno file di log
    cleanString(log_buf.operation);
    if(strcmp(client_user->User.username, user_winner) == 0){
        strcpy(log_buf.operation, "Winner\t");
    }else{
        strcpy(log_buf.operation, "Lose\t");
    }
    writeOnLogFile(log_buf);

    cleanString(log_buf.operation);
    strcpy(log_buf.operation, "logout_client");
    writeOnLogFile(log_buf);

    close(data->sock_descriptor);
    pthread_exit(NULL);
}

/**
 * Funzione che cerca l'utente che ha consegnato piu' oggetti
 * e salva il suo username nella variabile condivisa user_winner
 */
void searchWinner(usersTree t, int* obj){
    if(!emptyUsersTree(t)){
        searchWinner(t->sx, obj);
        if(t->User.status == ONLINE && t->User.found_objects > *obj){
            cleanString(user_winner);
            strcpy(user_winner, t->User.username);
            *obj = t->User.found_objects;
        }
        searchWinner(t->dx, obj);
    }
}

/**
 * Funzione che cerca analizza l'ABR degli utenti in cerca di un utente che ha
 * consegnato max_number_of_object oggetti
 */
void searchWinner2(usersTree t, int *exit){
    if(!emptyUsersTree(t)){
        searchWinner2(t->sx, exit);
        if(t->User.status == ONLINE && *exit == 0 && t->User.found_objects == max_number_of_object){
            cleanString(user_winner);
            strcpy(user_winner, t->User.username);
            *exit = 1;
        }
        searchWinner2(t->dx, exit);
    }
}

void azzeraPunteggiUtenti(usersTree t){
    if(!emptyUsersTree(t)){
        azzeraPunteggiUtenti(t->sx);
        t->User.found_objects = 0;
        t->User.object = -1;
        t->User.current_position.row = -1;
        t->User.current_position.column = -1;
        t->User.status = OFFLINE;
        azzeraPunteggiUtenti(t->dx);
    }
}

void *gameManagement(void *arg){
    int exit = 0, tmp;
    dati_log log_buf;

    while(1){
        pthread_mutex_lock(cond_mutex);
        pthread_cond_wait(cond, cond_mutex);

        if(gameover == 2) pthread_exit(NULL);

        pthread_mutex_lock(remaining_obj_mutex);
        if(remaining_objects == 0){
            tmp = -1;
            pthread_mutex_lock(user_mutex);
            searchWinner(users, &tmp);
            pthread_mutex_unlock(user_mutex);
            exit = 1;
        } 
        pthread_mutex_unlock(remaining_obj_mutex);

        pthread_mutex_lock(user_mutex);
        if(exit == 0) searchWinner2(users, &exit);
        pthread_mutex_unlock(user_mutex);

        pthread_mutex_lock(user_mutex);
        azzeraPunteggiUtenti(users);
        pthread_mutex_unlock(user_mutex);

        writeOnSTDOUT("\n\n *************** IL VINCITORE E' ");
        writeOnSTDOUT(user_winner);
        writeOnSTDOUT(" ***************\n\n");

        pthread_mutex_unlock(cond_mutex);

        strcpy(log_buf.IP_address, "localhost");
        strcpy(log_buf.username, "server");
        strcpy(log_buf.operation, "end_session"),
        writeOnLogFile(log_buf);

        writeOnSTDOUT("\n\n In attesa che i client ricevano il risultato della partita ...\n");
        pthread_mutex_lock(tid_list_mutex);
        tid_list cur = list;
        while(cur != NULL){
            pthread_join(cur->tid, NULL);
            cur = cur->next;
        }
        pthread_mutex_unlock(tid_list_mutex);

        //inizio una nuova sessione 
        system("clear");
        pthread_mutex_lock(tid_list_mutex);
        list = freeList(list);
        pthread_mutex_unlock(tid_list_mutex);
        initGameSession();
        exit = 0;

        writeOnSTDOUT("\n\n\t ********** CAMPO DA GIOCO **********\n\n");
        printMatrix_server();
    }
}
