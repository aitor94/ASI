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

#define DNI 0x72106540L
#define GET 30
#define PORT 3000
#define PORT2 3001
#define BUFFER 256

int main(int argc, char** argv){
	char ge[GET], buffer[BUFFER];
	struct sockaddr_in server;

	printf("Comenzar:");
	fgets(ge,GET,stdin);

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc<0){
		printf("Error creando socket\n");
		exit(0);
	}

	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT2);

	if(bind(socket_desc,(struct sockaddr *)&server,sizeof(struct sockaddr_in))<0){
		printf("Error en el bind");
		exit(0);
	}
	
	printf("Inicio bucle.....\n");
}
