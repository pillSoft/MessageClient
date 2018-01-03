client: client.o messageControlClient.o
	cc client.o messageControlClient.o -o client
client.o: client.c client.h messageControlClient.h
	cc -c client.c
messageControlClient.o: messageControlClient.c messageControlClient.h
	cc -c messageControlClient.c 

