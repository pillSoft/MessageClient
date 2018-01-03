#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h> 
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include "messageControlServer.h"


/**
 * Stringhe che identificano le pipe del server e dei client
 */
char *connectionPipe = "severPipe";
char *clientPipe = "clientPipe";

/**
 * Funzione utilizzata per mandare la lista dei client connessi al client passato come parametro
 * @param Destinatario del messaggio
 */
void sendList(int destinatario) {
    char messaggio[256] = "";
    //Salvataggio della lista nell'array
    makeClientList(messaggio);
    //Invio della lista al destinatario
    send(SIGUSR1,messaggio,destinatario);
}

/**
 * Funzione che salva nell'array passato come parametro la lista dei client
 * @param pString Array dove salvare la lista
 */
void makeClientList(char pString[256]) {
    char var[50]="";
    //Viene scritto una stringa iniziale
    strcat(pString, "Lista client connessi: \n");
    //Un client alla volta sono scritti nella lista
	int i;
    for (i = 0; i < clientsNum; ++i) {
        if(clients[i] != 0) {
            sprintf(var, "%d\n", clients[i]);
            strcat(pString, var);
        }
    }
}


/**
 * Funzione utilizzata per mandare il messaggio al client, passati come parametri
 * Utilizzata per i messaggi di errore o di informazione per i client
 * @param messaggio Messaggio da inviare
 * @param destinatario Id del destinatario del messaggio
 */
void sendNotify(char *messaggio, int destinatario) {
    //Invio del messaggio al destinatario
    send(SIGUSR2,messaggio,destinatario);
}

/**
 * Funzione utilizzata per mandare il messaggio al client, passati come parametri
 * Utilizzata per i messaggi inviati da client a client
 * @param mittente Id del mittente del messaggio
 * @param mess Messaggio da inviare
 * @param destinatario Id del destinatario del messaggio
 */
void sendMessage(int mittente,char mess[256], int destinatario) {
    char messaggio[256] ="";
    char strMittente[20] = "";
    //Composizione della stringa da inviare al client
    sprintf(strMittente,"%d",mittente);
    strcat(messaggio,strMittente);
    strcat(messaggio," ti ha inviato un messaggio : ");
    strcat(messaggio,mess);

    //Invio del messaggio al destinatario
    send(SIGUSR1,messaggio,destinatario);
}

/**
 * Funzione base che effettua l'invio del messaggio
 * @param signalType Tipo di segnale
 * @param messaggio Stringa da inviare
 * @param destinatario Id del destinatario
 */
void send(int signalType, char messaggio[256], int destinatario) {
    int messageLen;
    char strDestinatario[20]="";
    char pipe [100] = "";
    sprintf(strDestinatario,"%d",destinatario);
    //Invio del segnale al client, per avvertirlo dell'invio di un messaggio
    kill(destinatario,signalType);
    //Concatenazione del nome della pipe, creazione di essa
    strcat(pipe,clientPipe);
    strcat(pipe,strDestinatario);
    initClientPipe(pipe);
    //Calcolo della lunghezza del messaggio e  invio di esse
    messageLen = strlen(messaggio) + 1;
    write(clientFd, messaggio, messageLen);
    //Chiusura della pipe
    closeClientPipe(pipe);
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
 * Funzione che inizializza la pipe del server
 */
void initConnectionPipe(){
    //Eliminazione di una eventuale pipe non rimossa
    unlink(connectionPipe);
    //Creazione della pipe
    mknod (connectionPipe, S_IFIFO, 0);
    //Impostazioni dei permessi
    chmod (connectionPipe, 0660);
    //Apertura
    connectionFd = open (connectionPipe, O_RDONLY);
}

/**
 * Funzione che chiude la pipe del server
 */
void closeConnectionPipe(){
    //Chiusura della pipe
    close (connectionFd);
    //Eliminazione della pipe
    unlink(connectionPipe);
}

/**
 * Funzione che inizializza la pipe di un client
 * @param c Path della pipe
 */
void initClientPipe(char c[100]) {
    //Eliminazione di una eventuale pipe non rimossa
    unlink(c);
    //Creazione della pipe
    mknod (c, S_IFIFO, 0);
    //Impostazioni dei permessi
    chmod (c, 0660);
    //Apertura
    clientFd = open (c, O_WRONLY);
}

/**
 * Funzione che chiude la pipe di un client
 * @param c Path della pipe
 */
void closeClientPipe(char c[100] ) {
    //Chiusura della pipe
    close (clientFd);
    //Eliminazione della pipe
    unlink(c);
}
