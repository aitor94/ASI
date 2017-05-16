#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define PORT 3010
#define CLAVE 0x45168519L
#define TAM 1024

void finEj();
void enterrar();

struct sembuf semSube0 = {0,1,SEM_UNDO};
struct sembuf semBaja0 = {0,-1,SEM_UNDO};
struct sembuf semSube1 = {1,1,0};
struct sembuf semBaja1 = {1,-1,0};
struct sembuf semSube2 = {2,1,0};
struct sembuf semBaja2 = {2,-1,0};



int main(int argc, char **argv){

	//DEC
	struct sockaddr_in server_in, client_in;
	int pidH1,pidN1;
	int sd, fd;
	int addrlen;
	char mensaje[20];
	int memId, semId;
	char *punt;
	
	int dsp1,dsp2, resul;
	char tipo;
	//E1
	//socket
	server_in.sin_family=AF_INET;
	server_in.sin_addr.s_addr=inet_addr("127.0.0.1");
	server_in.sin_port=htons(PORT);

	//memoria
	memId = shmget(CLAVE, TAM, 0666|IPC_CREAT);	
	punt = shmat(memId, 0, 0666|IPC_CREAT);
	
	//semaforo
	semId = semget(CLAVE,3,0666);

	

	signal(SIGCHLD,enterrar);
	if((pidH1=fork())==0){

		sd = socket(AF_INET,SOCK_STREAM,0);
		bind(sd,(struct sockaddr*)&server_in,sizeof(struct sockaddr_in));
		listen(sd,10);
		addrlen=sizeof(struct sockaddr_in);
		while(fd=accept(sd,(struct sockaddr*)&server_in,&addrlen))
		{
			sleep(2);
			if((pidN1=fork())==0){
			
				close(sd);	
				printf("Atiende peticion\n");
				semop(semId,&semSube0,1);//Sube un proceso atendido
				printf("**Sem0 sube: %d\n",semctl(semId,0,GETVAL)); 
				while((read(fd,mensaje,sizeof(mensaje)))!=0){


						printf("Mensaje recibido: %s \n",mensaje);
					if(strcmp(mensaje,"hello!!\n")==0){
						strcpy(mensaje,"OK\n");
						write(fd,mensaje,strlen(mensaje));
					}else{
					
						semop(semId,&semBaja1,1);//Proceso activo de operacion
						
						sscanf(mensaje,"<%d>%c",&dsp1,&tipo);	
						
						if(tipo == '='){
							sscanf(mensaje,"<%d>=%d",&dsp1,&dsp2);
							semop(semId,&semBaja2,1);//entra escribe
							
							*(int*)(punt+dsp1) = dsp2;
							semop(semId,&semSube2,1);//sale escribe
							strcpy(mensaje,"\n");
							write(fd,mensaje,strlen(mensaje));
						}else if(tipo == '?'){ //AQUI ME SUBE DOS VALORES AL SEMAFORO 0: ERROR DE SECRETO 7
							semop(semId,&semBaja2,1); //entra lee
							dsp2=*(int*)(punt+dsp1);
							semop(semId,&semSube2,1); //sale lee
							sprintf(mensaje,"%d\n",dsp2);
							write(fd,mensaje,strlen(mensaje));
						}else if(tipo == '+'){
							sscanf(mensaje,"<%d>+<%d>",&dsp1,&dsp2);
							semop(semId,&semBaja2,1); //entra lee dsp1
							resul = *(int *)(punt+dsp1);
							semop(semId,&semSube2,1); //sale lee dsp1
							semop(semId,&semBaja2,1); //entra lee dsp2
							resul = resul + *(int *)(punt +dsp2);
							semop(semId,&semSube2,1); //sale lee dsp2
							sprintf(mensaje,"%d\n",resul);
							write(fd,mensaje,strlen(mensaje));
						}
					
						semop(semId,&semSube1,1); //Fin de proceso activo de operacion
					}

				}
				
				semop(semId,&semBaja0,1);
				printf("**Sem0 baja: %d\n",semctl(semId,0,GETVAL)); 
				close(fd);
				exit(0);
			}
		}
		//NO ESTOY SEGURO DE ESTA	
		//semop(semId,&semBaja0,1);
		//printf("**Sem0 baja: %d\n",semctl(semId,0,GETVAL)); 
		close(fd);
		exit(0);

	}
	finEj();
}



void finEj(){
	printf("Fin ejercicio\n");
	getchar();
}

void enterrar(){
	wait();
	printf("Hijo enterrado\n");
}
