#include "utilities.h"

void writeOnSTDOUT(char *msg){
    if(write(STDOUT_FILENO, msg, strlen(msg)) < 0 ) 
        error("\n ERRORE WRITE !\n", -3);
}

void error(char *msg, int error_code){
    perror(msg);
    exit(error_code);
}

void pausa(){
    char c;
    writeOnSTDOUT("\n Premere INVIO per continuare...\n");
    while((c = getchar()) != '\n');
}

void cleanString(char str[]){
    memset(str, '\0', strlen(str));
}

void logScreen(){
    printf("\n");
    printf("%54s\n", "_      ______    ____");
    printf("%55s\n", " | |    |  __  |  / ___|" );
    printf("%54s\n", " | |    | |  | | | |  _");
    printf("%55s\n", " | |___ | |__| | | |_| |");
    printf("%55s\n", " |_____||______|  \\____|");
    printf("\n\n");
    printf("______________________________________________________________________________________________\n");
    printf("______________________________________________________________________________________________\n");
}

void regScreen(){
    printf("\n\t\t\t");
    printf("%35s\n\t\t\t", "      _____    _____     _____              ");
    printf("%35s\n\t\t\t", "     | ___ \\  |  ___|   /  ___\\         ");
    printf("%35s\n\t\t\t", "     | |_/ /  | |___    | |              ");
    printf("%35s\n\t\t\t", "     |    /   |  ___|   | |  __            ");
    printf("%35s\n\t\t\t", "     | |\\ \\   | |____   | |__| |           ");
    printf("%35s\n\t\t\t", "     \\_| \\_|  \\_____/   \\_____/          ");
    printf("\n");
    printf("_________________________________________________________________________________________________\n");
    printf("_________________________________________________________________________________________________\n");
    printf("\n");
}

void menuScreen(){
    printf("\t\t\t");
    printf("%35s\n\t\t\t", "      __  __   _____   _   _   _   _        ");
    printf("%35s\n\t\t\t", "     |  \\/  | |  ___| | \\ | | | | | |      ");
    printf("%35s\n\t\t\t", "     | .  . | | |___  |  \\| | | | | |       ");
    printf("%35s\n\t\t\t", "     | |\\/| | |  ___| | . ` | | | | |      ");
    printf("%35s\n\t\t\t", "     | |  | | | |___  | |\\  | | |_| |      ");
    printf("%35s\n\t\t\t", "     \\_|  |_/ \\_____/ \\_| \\_/  \\___/       ");
    printf("\n");
    printf("_________________________________________________________________________________________________\n");
    printf("_________________________________________________________________________________________________\n");
    printf("\n");
}