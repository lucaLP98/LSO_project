/*
 *  file : user_tree.h
 */
#ifndef USER_TREE_H
#define USER_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "utilities.h"
#include "playground.h"

#define ONLINE 1
#define OFFLINE 0

//DEFINIZIONE STRUTTURA UTENTE
struct T_user{
    char username[50];
    char password[50];
    int status;
    int found_objects;
    int userID;
    int object;
    coordinates current_position;
};
typedef struct T_user user;

//DEFINIZIONE STRUTTURA PER ABR DI UTENTI
struct T_abrUsers{
    user User;

    struct T_abrUsers* dx;
    struct T_abrUsers* sx;
};
typedef struct T_abrUsers ABRusers;
typedef ABRusers* usersTree;

//DEFINIZIONE DI FUNZIONI E PROCEDURE PER LA MANIPOLAZIONE DI UN ABR DI UTENTI

/*
 * Inizializza un albero di utenti
 */
usersTree initUsersTree();

/*
 * Funzione che restituisce 1 se l'albero degli utenti e' vuoto, 0 altrimenti
 */
int emptyUsersTree(usersTree t);

/*
 * stampa l'albero degli utenti mediante un algoritmo del tipo inOrder
 * (stampando quindi in ordine alfabetico)
 */
void inOrderViewUsers(usersTree t);

/*
 * Crea un nuovo nodo Utente specificando username e password e
 * lo restituisce al chiamante
 */
usersTree allocateNewNodeUsers(char newUser[], char newPassword[]);

/*
 * Inserisce un nuovo utente nell'albero e restituisce l'albero modificato
 */
usersTree insertNewUser(usersTree t, char newUsername[], char newPassword[]);

/*
 * cerca un utente nell'albero tramite il campo username
 * e restituisce il sottoalbero avente come radice il nodo del utente cercato.
 * Restituisce NULL se non c'e' nessun utente associato al username data in input
 */
usersTree searchUsers(usersTree t, char username[]);

/**
 * Funzione che restituisce una stringa contenente tutte le informazioni 
 * sugli utenti attualmente online
 */
void usersInfoIntoString(usersTree t, char str[]);

#endif
