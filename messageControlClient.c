#include <memory.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "messageControlClient.h"


/**
 * Stringhe che identificano le pipe del server e dei client
 */
char *connectionPipe = "severPipe";
char *clientPipe = "clientPipe";

/**
 * Funzione che permette di scrivere il messaggio passato come parametro sulla pipe del server
 * La funzione apre e chiude il file
 * @param message
 */
void sendToServer(char *message) {
    //Calcolo della lunghezza del messaggio
    int messageLen = strlen(message) + 1;
    int fd;
    //Apertura della pipe
    do {
        fd = open(connectionPipe, O_WRONLY);
        if (fd == -1) sleep(1);
    } while (fd == -1);
    //Invio del messaggio
    write(fd, message, messageLen);
    //Chiusura del file
    close(fd);
}

/**
 * Funzione che permette di scrivere il messaggio passato come parametro sulla pipe del server
 * La funzione richiede il file gia aperto
 * @param fd File descriptor
 * @param message Stringa
 */
void sendToServerLight(int fd, char *message) {
    //Calcolo della lunghezza del messaggio
    int messageLen = strlen(message) + 1;
    //Chiusura del file
    write(fd, message, messageLen);
}

/**
 * Legge una linea terminata da '\0'
 * @param fd File Descriptor
 * @param str Stringa dove salvare la lettura
 * @return Esito della lettura
 */
int readLine (int fd, char *str) {
    int n;
    //Lettura di un carattere fino al terminatore
    do {
        n = read(fd, str, 1);
    } while (n > 0 && *str++ != '\0');
    //Restituisce false (0) quando raggiungiamo la fine della stringa
    return (n > 0);
}

/**
 * Funzione che restituisce il descrittore della pipe del server
 * @return File descriptor
 */
int connectToServerPipe(){
    int fd;
    //Apertura della pipe del server in scrittura
    do {
        fd = open(connectionPipe, O_WRONLY);
        if (fd == -1) sleep(1);
    } while (fd == -1);
    return fd;
}

/**
 * Funzione che restituisce il descrittore della pipe del server
 * @return File descriptor
 */
int connectToClientPipe(){
    int fd;
    char pipe [50] = "";
    char strPid[20] = "";
    //Creazione della stringa per aprire la pipe concatenando clientPipe e il PID del processo
    sprintf(strPid, "%d",getpid() );
    strcat(pipe,clientPipe);
    strcat(pipe,strPid);
    //Apertura della pipe del client in scrittura
    do {
        fd = open(pipe, O_RDONLY);
        if (fd == -1) sleep(1);
    } while (fd == -1);
    return fd;
}
