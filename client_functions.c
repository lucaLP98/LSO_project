#include "client.h"

obstaclesList initObtsaclesList(){
    return NULL;
}

obstaclesList addNewVisitedObtacle(obstaclesList list, coordinates obst_coordinate){
    obstaclesList newNode = (obstaclesList)malloc(sizeof(obstacles_list));

    newNode->obst_coordinate = obst_coordinate;
    newNode->next = list;

    return newNode;
}

int searchVisitedObstacles(obstaclesList list, coordinates obst_coordinate){
    if(list == NULL)
        return 0;

    if((list->obst_coordinate.row == obst_coordinate.row) && (list->obst_coordinate.column == obst_coordinate.column))
        return 1;

    return searchVisitedObstacles(list->next, obst_coordinate);    
}


char selectClientAccessMode(){
    char tmp[20];

    writeOnSTDOUT("\n Selezionare un'opzione valida e premere INVIO\n\n  > a. Login\n  > b. Registrazione\n  > c. Esci\n\n >>> ");
    do{
        read(STDOUT_FILENO, tmp, sizeof(tmp));
        if(tmp[0] != 'a' && tmp[0] != 'b' && tmp[0] != 'c')
            writeOnSTDOUT("\n ERRORE ! Opzione non valida...\n\n Riprovare >>> ");
    }while(tmp[0] != 'a' && tmp[0] != 'b' && tmp[0] != 'c');

    return tmp[0];
}

void printMatrix_client(playground camp, obstaclesList visited_obst, int userID, char msg[]){
    int i, j;
    coordinates tmp;

    printf("\n\t%s\n\n", msg);
    for(i=0;i<camp.dimension;i++){
        for(j=0;j<camp.dimension;j++){

            if(camp.matrix[i][j].user_mov <= -4){
                (camp.matrix[i][j].user_mov  == userID) ? printf("\t   TU") : printf("\t%5c", 'U');
            }else{
                switch(camp.matrix[i][j].content){
                    case -1: printf("\t%5c", '-'); break; //casella vuota 
                    case -2:
                        tmp.row = i; tmp.column = j;
                        (searchVisitedObstacles(visited_obst, tmp) == 1) ? printf("\t%5c", 'X') : printf("\t%5c", '-');
                    break;
                    case -3: printf("\t%5c", 'L'); break; //locazione
                    default: printf("\t%5d", camp.matrix[i][j].content); break; //oggetti
                }
            }        
        }
        printf("\n\n");
    }
}

char clientMenu(){
    char tmp[10];

    writeOnSTDOUT("\n\n\t\t********** MENU UTENTE **********\n");
    writeOnSTDOUT("\n > Utilizare i comandi N - S - O - E per muoversi nella mappa di una casella\n");
    writeOnSTDOUT("\n Oppure selezionare una scelta dal seguente menu': ");
    writeOnSTDOUT("\n\n  < A > Stato utenti in gioco\n  < B > Numero oggetti rimanenti\n  < C > Logout\n\n >>> ");
    do{
        read(STDIN_FILENO, tmp, 10);

        if(tmp[0]!= 'N' && tmp[0]!= 'n' && tmp[0]!= 'S' && tmp[0]!= 's' && tmp[0]!= 'O' && tmp[0]!= 'o' && tmp[0]!= 'E' && tmp[0]!= 'e' && 
           tmp[0]!= 'A' && tmp[0]!= 'a' && tmp[0]!= 'b' && tmp[0]!= 'B' && tmp[0]!= 'c' && tmp[0]!= 'C') 
           writeOnSTDOUT("\n\n ERRORE ! Scelta non valida\n\n Riprovare >>> ");
    }while(tmp[0]!= 'N' && tmp[0]!= 'n' && tmp[0]!= 'S' && tmp[0]!= 's' && tmp[0]!= 'O' && tmp[0]!= 'o' && tmp[0]!= 'E' && tmp[0]!= 'e' && 
           tmp[0]!= 'A' && tmp[0]!= 'a' && tmp[0]!= 'b' && tmp[0]!= 'B' && tmp[0]!= 'c' && tmp[0]!= 'C');

    return tmp[0];
}

void clientExit(){
    close(client_socket);
    system("clear"),
    writeOnSTDOUT("\n GRAZIE PER AVER GIOCATO CON NOI !!!\n\n _____________________________________________________________________________\n Develop By Luca Pastore\n Matr. N86002599\n\n");
    exit(0);
}

void sigHandlerMenu(){
    char buf[] = "c";
    if(send(client_socket, buf, strlen(buf), 0) == -1)    error("\n\n CLIENT: Errore invio dati\n\n", -10);

    clientExit();
}

void sigHandlerCloseServer(){
    close(client_socket);
    system("clear"),
    writeOnSTDOUT("\n IL SERVER HA INTERROTTO LA COMUNICAZIONE, SESSIONE FINITA...\n\n _____________________________________________________________________________\n Develop By Luca Pastore\n\n");
    exit(0);
}

void sigHandlerLogReg(){
    user tmp;
    tmp.status = -1;
    if(send(client_socket, &tmp, sizeof(tmp), 0) == -1)    error("\n\n CLIENT: Errore invio dati\n\n", -10);

    clientExit();
}