#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "client.h"
#include "messageControlClient.h"

/**
 *  Stringa contenente le scelte che l'utente puo' effettuare
 */
char *stringaMenu = "1) Connessione al server\n2) Richiesta elenco ID\n3) Invio di un messaggio\n4) Disconnessione dal server\n5) Uscita dal programma\n";
/**
 * Variabile che indica se il client e' connesso o meno
 */       
int isConnected = 0;

int main (void) {
    /**
     * File descriptor della pipe usata nella comuinicazione con il server
     */
    int fd;
    
    /**
     * Intero che indica la scelta effettuata dall'utente
     */
    int choice = 0;

    //Gestione dei segnali
    signal(SIGUSR1,handleReceive);
    signal(SIGUSR2,handleReceive);
    signal (SIGINT,handleCtrlC);

    //Ripetizione del ciclo finche' la scelta non e' 5(terminazione)
    do {
     	//Array usato per la memorrizazione delle varie stringhe
    	char message [256] = "";
        //Acquisizione della sscelta dall'utente
        printf("%s",stringaMenu);
        scanf("%d", &choice);
        //Composizione del messaggio per il server
        sprintf(message, "%d-%d",choice, getpid());
        switch (choice) {
            //Connessione al server
            case 1:
                //Se il client non e' ancora connesso manda una richiesta di connessione al server
                if(isConnected) {
                    printf("Sei gia connesso al server.\n");
                }else{
                    sendToServer(message);
                    isConnected = 1;
                }
                break;
            //Richiesta della lista dei client connessi
            case 2:
                //Se il client e' gia connesso manda una richiesta di invio della lista dei client connessi
                if(isConnected) {
                    sendToServer(message);
                }else{
                    printf("Prima connettiti al server.\n");
                }

                break;
            //Invio di un messaggio
            case 3:
                if(isConnected) {
                    //variabili locali usate per il salvataggio della richiesta e del messaggio da inviare
                    char savedStr[20] = "";
                    char savedMessage[256] = "";
                    strcat(savedStr,message);
                    //Inserimento e salvataggio del messaggio per l'invio
                    printf("Inserisci il messaggio:");
                    getMessage(savedMessage);
                    char s;
                    //Ciclo usato per l'invio di messaggi a piu' client
                    do{
                        //Apertura del pipe per inviare la richiesta
                        fd = connectToServerPipe();
                        //Invio al server della richiesta e del messaggio
                        sendToServerLight(fd,savedStr);
                        sendToServerLight(fd,savedMessage);
                        //Richichiesta dell'inserimento dell'id del destinatario
                        printf("Inserisci il PID del destinatario:");
                        scanf("%s",message);
                        //Invio dell'id del destinatario al server
                        sendToServerLight(fd,message);
                        close(fd);
                        //L'utente puo' scegliere se ripetere l'invio ad un altro destinatario
                        printf("Vuoi inserire un altro destinatario?(y/n)\n");
                        scanf(" %c", &s);
                    }while(s == 'y' || s == 'Y');
                }else{
                    printf("Prima connettiti al server.\n");
                }
                break;
            //Disconnessione dal server
            case 4:
                //Se il client e' gia' connesso manda una richiesta di disconnessione al server
                if(isConnected) {
                    sendToServer(message);
                    isConnected = 0;
                    printf("Ti sei disconnesso dal server.\n");
                }else{
                    printf("Prima connettiti al server.\n");
                }
                break;
            //Terminazione del client
            case 5:
                //Richiamo alla stessa funzione usata per gestire il Ctrl+C
                //Essa effettua la disconnessione del client ed esce 
                handleCtrlC(0);
                break;
        }
	
    }while(choice != 5);
    printf("Terminazione del processo.\n");
    return 0;
}

/**
 * Funzione che permette l'inserimento di un messaggio nell'array passato come parametro
 * La lettura e' effettuata carattere per carattere
 * @param pString Array dove inserire caratteri
 */
void getMessage(char pString[256]) {
    //Creazione e inizializzazione del char *, per scrivere un carattere alla volta
    char *c;
    c = pString;
    //Flush dei caratteri iniziali 'a capo' e 'EOF'
    while((*c = getchar()) == '\n' ||  *c == EOF);
    //Lettura e scrittura di un carattere alla volta, aggiungendo sempre il terminatore
    if(*c != '\n') {
        *c++;
	int i;
        for (i = 1; i < 256; i++) {
            *c = getchar();
            if (*c == '\n') {
                *c = '\0';
                i = 256;
            }
            *c++;
        }
        *c = '\0';
    }else{
        *c = '\0';
    }
}

/**
 * Funzione di gestione dei segnali custom SIGUSR1 e SIGUSR2
 * @param sig Segnale ricevuto
 */
void handleReceive(int sig) {
    int clientFd;
    char message [256];
    //Apertura della pipe del client
    clientFd = connectToClientPipe();
    //Lettura del messaggio
    if (readLine(clientFd, message)){
        if(sig == SIGUSR1) {//SIGUSR1 e' utilizzato per messaggi fra client
            printf("%s\n", message);
	    fflush(stdout);
        }
        if(sig == SIGUSR2){//SIGUSR2 e' utilizzato per conferme e errori
            printf("Notifica dal server:  %s\n",message);
	    fflush(stdout);
        }
    }
    if(strcmp(message,"Server terminato") == 0){
	if(isConnected) {
	    isConnected = 0;
	    printf("Disconnessione dal server\n");
            fflush(stdout);
        }
	
    }
    //Chiusura del file
    close(clientFd);
    //Ridefinizione dei segnali
    signal(SIGUSR1,handleReceive);
    signal(SIGUSR2,handleReceive);
    return;
}

/**
 * Funzione usata per gestire il segnale SIGINT
 * @param sig Segnale SIGINT (CTRL+C)
 */
void handleCtrlC(int sig){
    char message [256];
    //Se il client  connesso viene disconnesso
    if(isConnected) {
	sprintf(message, "4-%d", getpid());
        sendToServer(message);
        isConnected = 0;
	printf("Disconnessione dal server\n");
	fflush(stdout);
    }
    //Dopo la disconnessione il programma terminera' se abbiamo inserito 5
    //Ridefinizione del segnale
    signal (SIGINT,handleCtrlC);
    return;
}

