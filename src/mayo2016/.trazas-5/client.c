#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <unistd.h>

#define SERVER_PORT 3001
#define DESTIP "127.0.0.1"

#define CLAVE 0x72106540L	// Clave de los recursos. Sustituir por DNI.
#define TIME 4			// Temporizador de retransmisión
#define BUFLEN 256	// Tamaño de bufferes genérico

#define SIZE_SHM 1024	//Tamaño del segmento de memoria compartida

#define MAX_DEV	4	//Maximo # dispositivos en la tabla de sesiones


/* El array de semáforos a crear tiene tamaño 4
	El semáforo 0 servirá para controlar el acceso a lecturas de valores de
		los semáforos 1 y 2
	El semáforo 1 para poder modificar su valor en comando SEM
	El semáforo 2 para poder modificar su valor en comando SEM
	el semáforo 3 para controlar los accesos a la tabla de sesiones en 
		fase de registro.
*/
#define MAX_SEM 4	// Número de semáforos en el array

/* Lista de tipos de mensaje recibidos en la cola de mensajes. */

#define COMM_SPEED '2'
#define COMM_RPM '3'
#define COMM_SEM '4'
#define COMM_BYE '5'

#define MAX_COMMAND 6

char *udp_cmd[]={
	"HELLO", "PORT","SPEED","RPM","SEM","BYE",""
};

#define OFF_DATA_TBL 0	// Desplazamiento de la tabla de sesiones en SHM

#define ST_FREE 0
#define ST_PID 1
#define ST_DATA 2
#define LEN_NAME 16	//Usar como maximo nombres de 8

struct st_data {
	int state;	// State of register
	char name[LEN_NAME];	// Name of device
	int speed;	// velocidad 
	int rpm;	// revoluciones por minuto
	int port;	// Original port 
	int sem;	// Sem number 
	int semval;	// Sem value 
	pid_t pid;	// Process identifier
};

/*************************************************************************/
/* Función a utilizar para sustituir a signal() de la libreria.
Esta función permite programar la recepción de la señál de temporización de
alarm() para que pueda interrumpir una funcion bloqueante.
El alumno debe saber como utilizarla.
*/
int signal_EINTR(int sig,void(*handler)())
{
struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	return(sigaction(sig,&sa,NULL));
}
/*************************************************************************/


int main(int argc, char *argv[])
{
	int id = shmget(CLAVE,1024,0666|IPC_CREAT);
	int semid = semget(CLAVE,1,0666|IPC_CREAT);
	shmctl(id,IPC_RMID,(struct shmid_ds *)NULL);
	semctl(semid,0,IPC_RMID,0);
	char gets[30];
	
	printf("Pulsa para iniciar:");
	fgets(gets,30,stdin);

	int s, slen;
	struct sockaddr_in si_me, si_other, origin;
	char buffer[BUFLEN];
	char dev[] = "DEVICE-1";
	
	slen = sizeof(si_other);

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))<1){
		perror("Error al crear el socket\n");
		exit(0);
	}
	
	memset(&si_me,0,sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(3000);
	si_me.sin_addr.s_addr = INADDR_ANY;

	memset(&si_other,0,sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(3001);
	si_other.sin_addr.s_addr = INADDR_ANY;

	memset(&origin,0,sizeof(origin));

	if(bind(s,(struct sockaddr*)&si_me,sizeof(struct sockaddr_in))==-1){
		perror("Error al hacer bind\n");
		exit(0);
	}
	
	sprintf(buffer,"%s %d %s",udp_cmd[0],getpid(),argv[1]);

	sendto(s,buffer,strlen(buffer),0,(struct sockaddr*)&si_other,slen);

	recvfrom(s,buffer,BUFLEN,0,(struct sockaddr *)&si_other,&slen);
	printf("Recibido %s\n",buffer);
	
	getsockname(s,(struct sockaddr *)&origin,&slen);
	printf("Puerto -> %d\n",ntohs(origin.sin_port));
	sprintf(buffer,"%s %d",udp_cmd[1],ntohs(origin.sin_port));

	sendto(s,buffer,strlen(buffer),0,(struct sockaddr *)&si_other,slen);

	recvfrom(s,buffer,BUFLEN,0,(struct sockaddr *)&si_other,&slen);
	printf("Recibido %s\n",buffer);

	sendto(s,udp_cmd[5],strlen(udp_cmd[5]),0,(struct sockaddr *)&si_other,slen);

	exit(0);
}


