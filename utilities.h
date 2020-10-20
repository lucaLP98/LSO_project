#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/*
 * Stampa a video la scritta LOG
 */
void logScreen();

/*
 * Stampa a video la scritta REG
 */
void regScreen();

/*
 * Stampa a video la scritta MENU
 */
void menuScreen();

/*
 * Funzione utilizzata per segnalare un errore utilizzando perror
 */
void error(char *msg, int error_code);

/*
 * Funzione che permette di scrivere su standard output utilizzando la funzione write
 */
void writeOnSTDOUT(char *msg);

/*
 * Funzione utilizzata per mettere in pausa l'esecuzione del processo, 
 * finche l'utente non preme INVIO
 */
void pausa();

/**
 * Funzione che permette di pulire una stringa
 */
void cleanString(char str[]);

#endif 