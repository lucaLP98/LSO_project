/*
 * file : user_tree.c
 */
#include "user_tree.h"

int emptyUsersTree(usersTree t){
    return (t == NULL);
}

usersTree initUsersTree(){
    return NULL;
}

void printUser(user u){
    char buf[50];

    writeOnSTDOUT(" - ");
    writeOnSTDOUT(u.username);
    writeOnSTDOUT("\t\tOggetti consegnati ");
    sprintf(buf, "%d", u.found_objects);
    writeOnSTDOUT(buf); memset(buf, '\0', 50);
    writeOnSTDOUT("\t Posizione : ");
    sprintf(buf, "R. %d C. %d", u.current_position.row, u.current_position.column);
    writeOnSTDOUT(buf);
    if(u.object != -1){
        memset(buf, '\0', 50);
        sprintf(buf, "%d", u.object);
        writeOnSTDOUT("\t Porta l'oggetto ");
        writeOnSTDOUT(buf);
    }
    writeOnSTDOUT("\n\n");
}

void inOrderViewUsers(usersTree t){
    if( !emptyUsersTree(t) ){
        inOrderViewUsers(t->sx);
        if(t->User.status == ONLINE) printUser(t->User);
        inOrderViewUsers(t->dx);
    }
}

usersTree allocateNewNodeUsers(char newUser[], char newPassword[]){
    usersTree tmp;

    tmp = (usersTree)malloc(sizeof(ABRusers));
    strcpy(tmp->User.username, newUser);
    strcpy(tmp->User.password, newPassword);
    tmp->User.status = OFFLINE;
    tmp->User.found_objects = 0;
    tmp->User.userID = 0;
    tmp->User.object = -1;
    tmp->sx = NULL;
    tmp->dx = NULL;

    return tmp;
}

usersTree insertNewUser(usersTree t, char newUsername[], char newPassword[]){
    if( !emptyUsersTree(t) ){
        if( strcmp(newUsername, t->User.username ) < 0 )
            t->sx = insertNewUser(t->sx, newUsername, newPassword);
        else if( strcmp(newUsername, t->User.username ) > 0 )
            t->dx = insertNewUser(t->dx, newUsername, newPassword);
    }else{
        t = allocateNewNodeUsers(newUsername, newPassword);
    }

    return t;
}

usersTree searchUsers(usersTree t, char username[]){
    if( !emptyUsersTree(t) ){
        if( strcmp(username, t->User.username ) < 0 )
            return searchUsers( t->sx, username );

        if( strcmp(username, t->User.username ) > 0 )
            return searchUsers( t->dx, username );
    }

    return t;
}

void usersInfoIntoString(usersTree t, char str[]){
    char buf[20];

    if( !emptyUsersTree(t) ){
        usersInfoIntoString(t->sx, str);
        if(t->User.status == ONLINE){
            strcat(str, " > ");
            strcat(str, t->User.username);
            strcat(str, "\tOggetti depositati: ");
            sprintf(buf, "%d\t", t->User.found_objects);
            strcat(str, buf);
            strcat(str, "Posizione: (");
            memset(buf, '\0', 20);
            sprintf(buf, "%d, %d)\n", t->User.current_position.row, t->User.current_position.column);
            strcat(str, buf);
        }
        usersInfoIntoString(t->dx, str);
    }
}