#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <bits/signum.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "user_tree.h"
#include "playground.h"
#include "utilities.h"

//costanti numeriche che indicano lo stato di un oggetto
#define FOUND 1
#define NOT_FOUND 0

//struttura che contiene informazioni su un oggetto 
typedef struct T_object{
    coordinates destination_location;
    int status;
} object;

/*
 * struttura array che mantiene informazioni sugli oggetti presenti nel gioco
 * ogni oggetto sarà rappresentato da un ID intero (>= 0). Tale ID non sarà 
 * altro che l'indice dell'array
 */
struct T_object_array{
    object *arr;
    int number_of_object;
};
typedef struct T_object_array* object_array; 

/*
 * Struttura che contiene i dati di log
 */
typedef struct T_dati_log{
    char IP_address[100];
    char username[50];
    char operation[100];
}dati_log;

/*
 * Struttura che permette di salvare data e ora
 */
typedef struct T_date{
    int day;
    int month;
    int year;
    int hours;
    int minutes;
    int seconds;
}date;

/*
 * Struttura utilizzata per passare dati al thread di gestione del client
 */
typedef struct T_client_data{
    int sock_descriptor;
    struct sockaddr_in client_IP;
}client_data_str;
typedef client_data_str* client_data;

/**
 * Struttura che mantiene i tid dei thread attivi
 */
struct t_tid_list{
    pthread_t tid;
    struct t_tid_list *next;
};
typedef struct t_tid_list *tid_list;

//SEZIONE VARIABILI GLOBALI
usersTree users; //utenti iscritti al sistema prelevati da file
playground camp; //campo da gioco
object_array objects; //arrady degli oggetti
int server_socket; //ID della socket del server
int log_file_desriptor; //file descriptor del file utilizzato per le informazioni di log
int user_file_descriptor; //file descriptor del file utenti
int remaining_objects; //variabile che indica il numero di oggetti ancora da trovare
int max_number_of_object; //variabile che indica il massimo numero di oggetti che un client deve consegnare per vincere
int number_of_obstacles; //variabile che conterra' il numero di ostacoli totali presenti sul campo da gioco
char user_winner[50]; //conterra' l'username dell'utente vincitore
int gameover; //flag che indica la fine di una sessione di gioco
tid_list list; //lista che contiene i tid dei thread attivi

//SEZIONE MUTEX
pthread_mutex_t* user_file_mutex; //mutex associato al file utenti
pthread_mutex_t* log_file_mutex; //mutex associato al file di log
pthread_mutex_t* matrix_mutex; //mutex associato alla matrice del campo da gioco
pthread_mutex_t* remaining_obj_mutex; //mutex associato al numero di oggetti rimanenti
pthread_mutex_t* user_mutex; // mutex associato all'ABR degli utenti
pthread_mutex_t* object_array_mutex; //mutex associato all'array degli oggetti
pthread_mutex_t* flag_gameover_mutex; //mutex associato all'array degli oggetti
pthread_mutex_t* tid_list_mutex; //mutex associato alla lista dei tid dei thread attivi
pthread_mutex_t* cond_mutex; //mutex associato alla condition variable per la gestione della sessione di gioco

//SEZIONE CONDITION VARIABLE
pthread_cond_t* cond; //condition variable utilizzata dal thread che gestisce la sessione di gioco

/*
 * Questa funzione permette di leggere il file degli utenti e di inserirli
 * in un albero binario di ricerca.
 * Esce con codice -1 se si verifica un errore nell'apertura del file
 * -2 se si verifica un errore nella lettura del file.
 */
void createUsersTree(const char file_name[]);

/*
 * Funzione utilizzata per allocare e inizializzare i mutex
 */
void initMutex();

/*
 * Funzione utilizzata per inizializzare il file di log
 */
void initLogFile(char *file_name);

/*
 * Funzione che permette di inserire la grandezza del campo da gioco
 */
int insertPlaygroundDimension();

/*
 * Funzione utilizzata per inserie il numero di elementi da collocare
 * (elementi = oggetti || locazioni || ostacoli)
 */
int insertNumberOfElements(int matrix_dimension, char msg[]);

/*
 * Funzione che permette di settare parametri del gioco:
 *  - dimensione campo da gioco
 *  - numero di ostacoli
 *  - numero di oggetti e locazioni
 */
void setGameParameters();

/*
 * Funzione usata per generare il campo da gioco
 */
void generatePlayground();

/**
 * Funzione utilizzata per inizializzare una sessione di gioco
 */
void initGameSession();

/*
 * Handler utilizzato per chiudere il server catturando il segnale SIGINT
 */
void closeServer();

/*
 * Funzione che restituisce la data e l'ora corrente
 */
date currentDate();

/*
 * Funzione utilizzata per scrivere su file di log
 */
void writeOnLogFile(dati_log dati);

/*
 * Funzione che permette di stampare una matrice quadrata di interi lato server
 */
void printMatrix_server();

/*
 * Funzione utilizzata per gestire il client
 */
void *gestisciClient(void *arg);

/** 
 * Funzione di gestione della partita:
 *  - determina la fine della partita
 *  - determina il vincitore
 */
void *gameManagement(void *arg);

/**
 * Funzione che permette di inizalizzare una lista di thread
 */
tid_list initThreadList();

/**
 * Funzione che permette di aggiungere un TID alla lista dei thread attivi
 * Restituisce la nuova testa della lista.
 */
tid_list insertThread(pthread_t new, tid_list l);

/**
 * Funzione utilizzata per eliminare un thread dalla lista
 */
tid_list deleteThread(pthread_t toDelete, tid_list l);

/**
 * Funzione utilizzata per deallocare una lista di tid
 */
tid_list freeList(tid_list l);

#endif 