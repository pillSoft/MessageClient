server: server.o messageControlServer.o
	cc server.o messageControlServer.o -o server
server.o: server.c server.h messageControlServer.h
	cc -c server.c 
messageControlServer.o: messageControlServer.c messageControlServer.h
	cc -c messageControlServer.c 

