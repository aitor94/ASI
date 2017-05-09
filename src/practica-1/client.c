#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM 30

int pid,hilos,seniales;

void respuesta();
void respuesta2();
void respuesta3();
void *funcionhilo(void * arg);
void creahilo();

int main(int argc,char**argv)
{
	int pid1,pid2;
	char cadena[TAM];

	hilos=0;
	seniales=0;	

	if(argc!=2)
	{	
		printf("Error, numero de argumentos");
		exit(0);
	}
	else
		pid=atoi(argv[1]);

	printf("pid monitor=%d\n",pid);	

	signal(SIGUSR2,respuesta);

	printf("Ejercicio 1, pulsa para continuar:");
	fgets(cadena,TAM,stdin);

	kill(pid,SIGUSR1);
	sleep(5);

	printf("Ejercicio 2, pulsa para contiunuar:");
	fgets(cadena,TAM,stdin);

	kill(pid,SIGUSR1);

	printf("Ejercicio 3, pulsa para continuar:");
	fgets(cadena,TAM,stdin);

	kill(pid,SIGUSR1);
	sleep(1);

	if((pid1=fork())==0) //aqui hijo 1
	{
		printf("hijo2 creado con pid=%d\n",getpid());

		signal(SIGUSR2,creahilo);

		while(1)
			pause();
	}
	else if((pid2=fork())==0) //aqui hijo 2
	{
		printf("hijo 2 con pid=%d\n",getpid());
		
		signal(SIGUSR2,respuesta2);
		signal(SIGUSR1,respuesta3);
		while(1)
			pause();	
	}
	else //aqui el padre
	{
		kill(pid,SIGUSR1);

		printf("Ejercicio 4 y 5, pulsa para continuar:");
		fgets(cadena,TAM,stdin);
		
		kill(pid,SIGUSR1);

		printf("Ejercicio 6, pulsa para continuar:");
		fgets(cadena,TAM,stdin);
		
		kill(pid,SIGUSR1);
		sleep(1);
		int status;
		
		kill(pid1,SIGINT);
		waitpid(pid1,&status,0);

		kill(pid2,SIGINT);
		waitpid(pid2,&status,0);

		exit(0);		
	}
}

void respuesta()
{
	printf("respuesta, senial resibida\n");
	kill(pid,SIGUSR2);
}

void respuesta2()
{
	printf("respuesta 2, senial resibida\n");
	kill(pid,SIGUSR2);
	seniales++;
	printf("Senial resibida wey ->%d\n",seniales);
}
void respuesta3()
{
	printf("recibida SIGUSR1\n");
}
void creahilo()
{
	printf("Creando hilo...\n");
	pthread_t thd;
	pthread_create(&thd,NULL,(void*)funcionhilo,NULL);
}

void *funcionhilo(void * arg)
{
	hilos++;
	int i;

	for(i=0;i<4;i++)
	{
		printf("Hijo 1 sigo vivo");
		sleep(5);
	}
}
