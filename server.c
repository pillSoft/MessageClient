#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include "server.h"
#include "messageControlServer.h"

int connectionFd = 0;
int clientFd = 0;
int *clients = 0;
int clientsNum = 0;


int main (void) {
    
    //Gestione del segnale SIGINT(CTRL+C)
    signal(SIGINT,handleCtrlC);
    //Allocazione del primo elemento per contenere l'ID dei client
    clients = malloc (sizeof(int));
    //Inizializzazione della pipe del server
    initConnectionPipe();
    //Ciclo infinito per gestire le richieste da parte dei client
    do{
	//Stringa contenente il comando letto dalla pipe del server
    	char str[20] = "";
        //Lettura del comando da gestirte
        if (readLine (connectionFd, str)){
            int id;
            char scelta;
            //Utilizzo di un char* per la lettura dell'operazione da fare
            //e per la lettura dell'Id del processo che la richiede
            char *c;
            c = str;
            scelta = *c;
            *c++;
            *c++;
            //Trasformazione dell'Id in intero
            id = atoi(c);
            switch(scelta) {
                //Connessione al server
                case '1':
                    if (!clientIsConnected(id)) {
                        //Quando il client non e' ancora connesso viene aggiunto alla lista dei client connessi
                        addClient(id);
                        printf("Connessione di : %d\n", id);
                        //Notifica agli altri client della connessione
                        notifyAboutConnection(id,1);
                    } else {
                        printf("Client gia connesso");
                    }
                    break;
                //Richiesta della lista dei client connessi
                case '2'://2-PID
                    if (clientIsConnected(id)) {
                        //Quando un client connesso gli viene inviata la lista dei client connessi
                        sendList(id);
                    } else {
                        printf("Client non connesso");
                    }
                    break;
                //Invio di un messaggio
                case '3':
                    if (clientIsConnected(id)) {
                        char mss[256]="";
                        char dest[20]="";
                        int destinatario;
                        //Lettura messaggio
                        readLine(connectionFd, mss);
                        //Lettura destinatario
                        readLine(connectionFd, dest);
                        destinatario = atoi(dest);

                        if (clientIsConnected(destinatario)) {
                            //Quando il client e' connesso si invia il messaggio tramite la sendMessage()
                            //e notifichiamo al mittente l'invio
                            sendMessage(id, mss, destinatario);
                            sendNotify("Messaggio inviato correttamente", id);
                        } else {
                            //In caso il destinatario non fosse connesso si informa il mittente
                            sendNotify("Destinatario non connesso al server", id);
                        }
                    } else {
                        printf("Client non connesso");
                    }
                    break;
                //Disconnessione dal server
                case '4':
                    if (clientIsConnected(id)) {
                        //Client rimosso dalla lista con l'apposita funzione
                        removeClient(id);
                        printf("Disconnessione di : %d\n", id);
			notifyAboutConnection(id,0);
                    } else {
                        printf("Client non connesso");
                    }
                    break;
            }
            fflush(stdout);
        }
    }while(1);
}


/**
 * Funzione che rimuove un client dalla lista dei client connessi
 * @param id Client da rimuovere
 */
void removeClient(int id) {
    //Azzeramento dell'id del client da rimuovere
    int i;
    for (i = 0; i < clientsNum; ++i) {
        if (clients[i] == id) clients[i] = 0;
    }
    //Spostamento dell'intero 0 in fondo all'array
    for (i = 0; i < clientsNum-1; ++i) {
        if (clients[i] == 0){
            clients[i] = clients[i+1];
            clients[i+1] = 0;
        }
    }

}

/**
 * Funzione che aggiunge un client nella lista
 * @param id
 */
void addClient(int id) {
	int i;
    //Cerca la prima posizione con 0 e inserisce l'id del client
	int findZero=0;
	for (i = 0; i < clientsNum; ++i) {
		if(clients[i]==0){
			findZero = 1;
			clients[i]=id;
			break;		
		}
    }
    //Se l'array tutto pieno viene allocato una posizione e inserito l'id del client
	if(findZero==0){
		clientsNum++;
		clients = realloc(clients, clientsNum * sizeof(*clients));
		clients[clientsNum - 1] = id;
	}
}


/**
 * Controlla se un client connesso
 * @param id Id del client da controllare
 * @return Esito del controllo
 */
int clientIsConnected(int id){
    int res = 0;
	int i;
    for (i = 0; i < clientsNum; ++i) {
        if (clients[i] == id) res=1;
    }
    return res;
}

/**
 * Funzione usata per gestire il segnale SIGINT
 * @param sig Segnale SIGINT (CTRL+C)
 */
void handleCtrlC(int sig){
    //Per ogni client connesso al server comunica la terminazione del server ed invia un segnale sigint
    int i;
    for(i=0;i<clientsNum;i++){
        if(clients[i] > 0) {
            sendNotify("Server terminato", clients[i]);
        }
    }
    printf("Programma terminato\n");
    fflush(stdout);
    //Chiusura della pipe
    closeConnectionPipe();
    free(clients);
    exit(0);
    return;
}

/**
 * Funzione che notifica la connessione di un client a tutti gli altri
 * @param newClient Id del client appena connesso
 */

void notifyAboutConnection(int newClient,int action){
    char tmpMessage[50]  = "";
    
    if(action==1){
	    char tmpMessage1[50] = "";
	    sprintf(tmpMessage1,"Ti sei connesso con id %d",newClient);
	    if(newClient>0)
		sendNotify(tmpMessage1, newClient);

    }
    //Composizione del messaggio
    if(action==1)
        sprintf(tmpMessage,"Connessione di %d",newClient);
    else
        sprintf(tmpMessage,"Disconnesione di %d",newClient);
    //Invio del messagio ad ogni Client tranne lui stesso
    int i;
    for(i=0;i<clientsNum;++i){
        if(clients[i]!=newClient && clients[i]>0)
            sendNotify(tmpMessage, clients[i]);
    }
}


