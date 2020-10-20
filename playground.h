#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#define MAX_MATRIX_DIM 100

/**
 * Struttura che rappresenta una coppia di coordinate riga-colonna
 */
typedef struct T_cord{
    int row;
    int column;
}coordinates;

/*
 * Struttura che rappresenta una casella della matrice del campo da gioco.
 * user_mov --> rappresenta lo spazio della casella in cui si muovono gli utenti
 * content --> rappresenta il contenuto della casella, cioe' locazione, ostacolo od oggetto
 */
typedef struct T_cell{
    int user_mov;
    int content;
}cell;

//Struttura che rappresenta il campo da gioco
/*
 * Nella matrice di inetri usata per rappresentare il campo da gioco:
 *  i valori >= 0 indicano gli oggetti (ad ogni oggetto viene assegnato un ID positivo)
 *  i valori == -1 rappresentano una casella vuota
 *  i valori == -2 rappresentano ostacoli
 *  i valori == -3 indicano locazioni
 *  i valori <= -4 indicano gli utenti (ad ogni utente viene assegnato un ID negativo)
 */
typedef struct T_playground{
    cell matrix[MAX_MATRIX_DIM][MAX_MATRIX_DIM]; //matrice campo da gioco
    int dimension; //dimensione (del lato) del campo da gioco
}playground;

#endif 