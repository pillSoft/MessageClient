int clientIsConnected(int id);
void removeClient(int id);
void handleCtrlC(int sig);
int readLine (int fd, char *str);
void addClient(int id);




void notifyAboutConnection(int newClient, int action);
