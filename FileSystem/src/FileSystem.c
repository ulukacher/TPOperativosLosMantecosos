/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <SocketsL.c>

int PUERTO;
char* PUNTO_MONTAJE;
char* IP_KERNEL;
int PUERTO_KERNEL;

void obtenerValoresArchivoConfiguracion() {
	int contadorDeVariables = 0;
	int c;
	FILE *file;
	file = fopen("ArchivoConfiguracion.txt", "r");
	if (file) {
		while ((c = getc(file)) != EOF)
			if (c == '=')
			{
				if (contadorDeVariables == 3)
				{
					fscanf(file, "%i", &PUERTO_KERNEL);
				}

				if (contadorDeVariables == 2)
				{
				char buffer[10000];
				IP_KERNEL = fgets(buffer, sizeof buffer, file);
				strtok(IP_KERNEL, "\n");
				contadorDeVariables++;
				}
				if (contadorDeVariables == 1)
				{
					char buffer[10000];
					PUNTO_MONTAJE = fgets(buffer, sizeof buffer, file);
					strtok(PUNTO_MONTAJE, "\n");
					contadorDeVariables++;

				}
				if (contadorDeVariables == 0) {
					fscanf(file, "%i", &PUERTO);
					contadorDeVariables++;
				}
			}
		fclose(file);
	}
}

void imprimirArchivoConfiguracion() {
	int c;
	FILE *file;
	file = fopen("ArchivoConfiguracion.txt", "r");
	if (file) {
		while ((c = getc(file)) != EOF)
			putchar(c);
		fclose(file);
	}
}

int main(void) {
	//conectarAlServidor("127.0.0.1", 5000);
	obtenerValoresArchivoConfiguracion();
	imprimirArchivoConfiguracion();
	int socketFD = ConectarServidor(PUERTO_KERNEL, IP_KERNEL);
	EnviarHandshakeString(socketFD);
	RecibirMensajeString(socketFD);
	RecibirMensajeString(socketFD);

	return 0;
}
