void sendMessage(int mittente,char mess[256], int destinatario);
void sendNotify(char *messaggio, int destinatario);
void sendList(int destinatario);
void send(int signalType, char messaggio[256], int destinatario);
void closeClientPipe(char c[100] );
void makeClientList(char pString[256]);
void initClientPipe(char c[100]);
void closeConnectionPipe();
void initConnectionPipe();


//Definizioni delle variabili esterne utilizzate dalla maggior parte delle funzioni del progetto
extern int connectionFd;
extern int clientFd;
extern int *clients;
extern int clientsNum;
