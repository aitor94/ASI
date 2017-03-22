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

#define DNI 0x72106540L
#define GET 30

int main(int argc,char** argv)
{
	int memoria;
	char gets[GET];
	
	printf("Ejercicio 1, inserta para continuar\n");
	fgets(gets,GET,stdin);

	memoria=shmget(DNI,1024,IPC_CREAT|0666);

	if(memoria==-1)
	{
		printf("Error shmget\n");
		exit(0);
	}

	char* segmento;
	segmento=shmat(memoria,NULL,0);

	printf("bien enganchado a sh\n");

	int secreto1 = *( (int*) segmento);

	printf("Secreto 1->%d\n",secreto1);

	int offset = *((int*) segmento+1);

	char* secreto2 = segmento+offset;
	char secreto22[GET];
	strcpy(secreto22,secreto2);
 	
	printf("secreto 2->%s\n",secreto2);

	sleep(3);

	shmdt(segmento);

	printf("Ejercicio 2, pulse para continuar\n");
	fgets(gets,GET,stdin);
	
	sleep(1);
	
	memoria=shmget(DNI,1024,IPC_CREAT|0666);

	if(memoria==-1)
	{
		printf("ej2, error shm\n");
		exit(0);
	}	
	
	sleep(1);
	segmento=shmat(memoria,NULL,0);

	printf("bien enganchado ej2\n");
	
	strcpy(segmento,secreto22);
	printf("ej2, secreto2->%s\n",segmento);
	printf("ej22, secreto2->%s\n",secreto22+1);
	
	offset=atoi(secreto22+1);
	offset=offset+16;

	printf("offset ej2->%d\n",offset);

	sprintf(secreto22,"<%d>",secreto1);

	printf("cadena secreto1->%s\n",secreto22);

	strcpy(segmento+offset,secreto22);
	
	sleep(1);
	shmdt(segmento);

	printf("Ejercicio 3, pulsa para continuar:\n");
	fgets(gets,GET,stdin);

	int dni2=540;
	int semid=semget(DNI,1,IPC_CREAT|0666);
	semctl(semid,0,SETVAL,dni2);

	printf("Ejercicio 4, pulse para continuar:\n");
	fgets(gets,GET,stdin);

	semid=semget(DNI,2,IPC_CREAT|0666);
	semctl(semid,0,SETVAL,1);
	semctl(semid,1,SETVAL,2);

	printf("Ejercicio 5, pulsa para continuar:\n");
	fgets(gets,GET,stdin);

	struct sembuf up;
	up.sem_num=0;
	up.sem_op=1;
	up.sem_flg=0;

	struct sembuf down;
	down.sem_num=0;
	down.sem_op=-1;
	down.sem_flg=0;

	semid=semget(DNI,1,IPC_CREAT|0666);
	memoria=shmget(DNI,1024,IPC_CREAT|0666);

        if(memoria==-1)
        {
                printf("ej5, error shm\n");
                exit(0);
        }

        sleep(1);
        segmento=shmat(memoria,NULL,0);
	
	semop(semid,&down,1);

	int sec5= *((int*) segmento);
	printf("secreto 5->%d\n",sec5);

	*((int*) segmento) = -sec5;
	sleep(3);

	semop(semid,&up,1);

	printf("Ejercicio 6, pulsa para continuar:");
	fgets(gets,GET,stdin);
	
	struct sembuf up2[2] = { {0,1,0}, {1,1,0} };	
	
	struct sembuf down2[2] = { {0,-1,0}, {1,-1,0} };
	
	semid=semget(DNI,1,IPC_CREAT|0666);
        memoria=shmget(DNI,1024,IPC_CREAT|0666);

        if(memoria==-1)
        {
                printf("ej6, error shm\n");
                exit(0);
        }

        sleep(1);
        segmento=shmat(memoria,NULL,0);

        semop(semid,down2,2);

	int sec6= *((int*) segmento);
        printf("secreto 6->%d\n",sec6);

        *((int*) segmento) = -sec6;
        sleep(3);

        semop(semid,up2,2);

	printf("Ejercicio 7, pulsa para continuar:");
        fgets(gets,GET,stdin);

        semid=semget(DNI,1,IPC_CREAT|0666);
        memoria=shmget(DNI,1024,IPC_CREAT|0666);	

	if(memoria==-1)
        {
                printf("ej5, error shm\n");
                exit(0);
        }

        sleep(1);
        segmento=shmat(memoria,NULL,0);
	
	printf("Esperando a semaforo\n");
        semop(semid,&down,1);
	printf("Dentro de semaforo\n");	

	semctl(semid,0,IPC_RMID,1);
	shmdt(segmento);
	struct shmid_ds buff2;
	shmctl(memoria,IPC_RMID,&buff2);

	printf("Fin, pulsa para continuar:");
        fgets(gets,GET,stdin);
}
