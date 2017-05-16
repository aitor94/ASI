#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DNI 0x72106540L
#define GET 30
#define BUFFER 256
#define FIFOW "/tmp/72106540E-W"
#define FIFOR "/tmp/72106540E-R"

int fifow, fifor, pidmonitor, memoria,semid,fin;
void saludo();
void respuesta();
void terminar();

int main(int argc, char** argv){
	
	char ge[GET];
	int pidpadre = getpid();
	pidmonitor = atoi(argv[1]);
	int hijo1, hijo2;
	fin = 1;
	
	signal(SIGUSR1,saludo);
	signal(SIGUSR2,respuesta);
	printf("Pid del monitor -> %d\n",pidmonitor);

	semid = semget(DNI,4,IPC_CREAT|0666);
	struct sembuf up0 = {0,1,0};
	struct sembuf down0 = {0,-1,0};
	struct sembuf up1 = {1,1,0};
	struct sembuf down1 = {1,-1,0};
	struct sembuf up2 = {2,1,0};
	struct sembuf down2 = {2,-1,0};

	printf("Ejercicio 1, pulsa para iniciar:");
	fgets(ge,GET,stdin);

	//kill(pidmonitor,SIGUSR1);

	mkfifo(FIFOR,S_IRWXU);
	mkfifo(FIFOW,S_IRWXU);
	fifor = open(FIFOR,O_RDWR);
	if(fifor==-1){
		printf("Error fifor\n");
		unlink(FIFOR);
		exit(0);
	}

	fifow = open(FIFOW,O_RDWR);
	if(fifow==-1){
		printf("Error fifow\n");
		unlink(FIFOW);
		exit(0);
	}

	memoria = shmget(DNI,1024,IPC_CREAT|0666);
	
	char * segmento;
	segmento = shmat(memoria, NULL, 0);
	
	if((hijo1=fork())==0){
		printf("Hijo 1 %d\n",getpid());
		signal(SIGUSR1,saludo);
		signal(SIGUSR2,respuesta);
		segmento = shmat(memoria,NULL,0);
		printf("Hijo 1 seniales definidas\n");
		while(1){
			//pause();
			semop(semid,&down1,1);
			respuesta();
			//semop(semid,&up1,1);
		}
	}
	else if((hijo2=fork())==0){
		printf("Hijo 2 %d\n",getpid());
		signal(SIGUSR1,saludo);
		signal(SIGUSR2,respuesta);
		segmento = shmat(memoria, NULL, 0);
		printf("Hijo 2 seniales definidas\n");
		while(1){
			//pause();
			semop(semid,&down2,1);
			respuesta();
			//semop(semid,&up2,1);
		}
	}
	else{
		printf("%d %d %d",getpid(),hijo1,hijo2);
		while(fin==1){
			semop(semid,&down0,1);
			respuesta();
			//semop(semid,&up0,1);
		}
		printf("Soy el padre, pulsa para terminar:");
		fgets(ge,GET,stdin);
		
		int status;
		
		struct shmid_ds buff;
		shmdt(segmento);
		shmctl(memoria,IPC_RMID,&buff);
		
		semctl(semid,IPC_RMID,1);

		unlink(FIFOR);
		unlink(FIFOW);

		kill(hijo1,SIGINT);
		waitpid(hijo1,&status,0);

		kill(hijo2,SIGINT);
		waitpid(hijo2,&status,0);

		exit(0);
	}
}

void terminar(){
	fin = 0;
}
void saludo(){
	printf("Atendiendo senial 1 %d\n",getpid());
	char buffer[BUFFER];
	sprintf(buffer,"H<%d>",getpid());
	write(fifow,buffer,strlen(buffer));
	printf("%d enviando saludo\n",getpid());	
}

void respuesta(){
	int num;
	char type, buffer[BUFFER];
	char * segmento;
	segmento = shmat(memoria, NULL, 0);

	read(fifor,buffer,BUFFER);
	sscanf(buffer,"%c",&type);
	num = *((int*) (buffer+1));

	printf("Recibido tipo -> %c\n",type);

	if(type=='E'){
		sprintf(buffer,"e<%d><%d>",getpid(),num);
		write(fifow,buffer,strlen(buffer));
	}
	else if(type=='R'){
		int clave = *((int *) (segmento+num));
		sprintf(buffer,"r<%d><%d>",getpid(),clave);
		*((int *) (segmento + clave)) = getpid();
		write(fifow,buffer,strlen(buffer));
	}
	else if(type=='+'){
		int desp1, desp2, num1, num2, send;
		desp1 = num;
		desp2 = *((int*)(buffer+5));
		num1 = *((int*)(segmento+desp1));
		num2 = *((int*)(segmento+desp2));
		
		send = num1+num2;

		printf("Recibidos %d %d\n",num1,num2);
		sprintf(buffer,"+<%d><%d>",getpid(),send);
		write(fifow,buffer,strlen(buffer));
	}
	else if(type=='*'){
		int desp1, desp2, num1, num2;
                desp1 = num;
                desp2 = *((int*)(buffer+5));
		struct sembuf up = {3,1,0};
		struct sembuf down = {3,-1,0};

		semop(semid,&down,1);
                num1 = *((int*) (segmento+desp1));
                num2 = *((int*)(segmento+desp2));
		semop(semid,&up,1);
		
		int send = num1*num2;
		printf("Recibidos %d %d\n",num1,num2);
		sprintf(buffer,"*<%d><%d>",getpid(),send);
		write(fifow,buffer,strlen(buffer));	
	}
}
