#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <bits/signum.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "user_tree.h"
#include "utilities.h"
#include "playground.h"

/**
 * Lista di ostacoli gia incontrati
 */
typedef struct T_obstacles_list{
    coordinates obst_coordinate;
    struct T_obstacles_list *next;
}obstacles_list;
typedef obstacles_list* obstaclesList;

//VARIABILI GLOBALI
int client_socket; //socket utilizzata dal client

/**
 * Funzione usata per inizializzare una lista di ostacoli
 */
obstaclesList initObtsaclesList();

/**
 * Funzione che permette di aggiungere un nuovo ostacolo visitato alla lista (in testa)
 * e restituisce la nuova testa della lista
 */
obstaclesList addNewVisitedObtacle(obstaclesList list, coordinates obst_coordinate);

/**
 * Funzione utilizzata per cercare un  ostacolo visitato nella lista
 */
int searchVisitedObstacles(obstaclesList list, coordinates obst_coordinate);

/*
 * Funzione utilizzata dal client per decidere il metodo di accesso:
 * a --> login
 * b --> registrazione
 * c --> esci senza effettuare login
 */
char selectClientAccessMode();

/*
 * Funzione che permette di stampare una matrice quadrata di interi lato client
 */
void printMatrix_client(playground camp, obstaclesList visited_obst, int userID, char msg[]);

/**
 * Funzione che permette la scelta da parte dell'utente della mossa da svolgere nel gioco
 */
char clientMenu();

/**
 * Funzione utilizzata per chiudere il client e la connessione
 */
void clientExit();

/**
 * HAndler per catturare segnale emanato da chiusura server
 */
void sigHandlerCloseServer();

/**
 * Handler per catturare segnali durante il primo menu di gioco
 */
void sigHandlerMenu();

/**
 * Handler per catturare i segnali durante le fasi di Login e Registrazione
 */
void sigHandlerLogReg();

#endif 