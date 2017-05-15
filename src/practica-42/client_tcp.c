#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define GET 30
#define BUFFER 256

int main(int argc, char**argv){
	char buffer[BUFFER],*host,*port,*dni;

	printf("Cadena recibida = %s\n",argv[1]);
	
	dni = strtok(argv[1],"@");
	host = strtok(NULL,":");
	port = strtok(NULL,">");

	printf("%s %s %s\n",dni,host,port);
	
	int s = socket(AF_INET,SOCK_STREAM,0);
	struct hostent *h = gethostbyname(host);
	struct in_addr ip_addr;
	struct sockaddr_in server;

	ip_addr =*(struct in_addr *)(h->h_addr);
	server.sin_family = AF_INET;
	server.sin_addr = ip_addr;
	server.sin_port = htons(atoi(port));
	
	connect(s,(struct sockaddr *)&server,sizeof(server));

	sprintf(buffer,"user %s\n",dni);

	send(s,buffer,strlen(buffer),0);
	printf("TX:<%s>\n",buffer);
	recv(s,buffer,BUFFER,0);
	printf("RX:<%s>\n",buffer);

	sprintf(buffer,"get clave\n");
	send(s,buffer,strlen(buffer),0);
	printf("TX:<%s>\n",buffer);
	recv(s,buffer,BUFFER,0);
	printf("RX:<%s>\n",buffer);

	char clave[BUFFER];
	sscanf(buffer,"CLAVE%s",clave);

	send(s,clave+1,strlen(clave),0);
	printf("TX:<%s>\n",clave);

	recv(s,buffer,BUFFER,0);
	printf("RX:<%s>\n",buffer);
}
