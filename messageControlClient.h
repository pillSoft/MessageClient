void sendToServer(char *message);
void sendToServerLight(int fd, char *message);
int readLine (int fd, char *str);
int connectToServerPipe();
int connectToClientPipe();