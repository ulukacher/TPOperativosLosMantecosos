/*
 * SocketsL.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */


#include "SocketsL.h"


int ConectarServidor(int PUERTO_KERNEL, char* IP_KERNEL)
{
	int socketFD = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in direccionKernel;

	direccionKernel.sin_family = AF_INET;
	direccionKernel.sin_port = htons(PUERTO_KERNEL);
	direccionKernel.sin_addr.s_addr = inet_addr(IP_KERNEL);
	memset(&(direccionKernel.sin_zero), '\0', 8);

	connect(socketFD,(struct sockaddr *)&direccionKernel, sizeof(struct sockaddr));
	//printf("%s", "Se conecto!Anda a kernel y aceptame\n");

	return socketFD;
}


int StartServidor(char* MyIP, int MyPort) // obtener socket a la escucha
{
	struct sockaddr_in myaddr; // dirección del servidor
	int yes = 1; // para setsockopt() SO_REUSEADDR, más abajo
	int SocketEscucha;

	if ((SocketEscucha = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if (setsockopt(SocketEscucha, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) // obviar el mensaje "address already in use" (la dirección ya se está 	usando)
	{
		perror("setsockopt");
		exit(1);
	}

	// enlazar
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = inet_addr(MyIP);
	myaddr.sin_port = htons(MyPort);
	memset(&(myaddr.sin_zero), '\0', 8);

	if (bind(SocketEscucha, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}

	// escuchar
	if (listen(SocketEscucha, 10) == -1)
	{
		perror("listen");
		exit(1);
	}

	return SocketEscucha;
}


void EnviarPaquete(int socketCliente, Paquete* msg, int cantAEnviar)
{

	void* datos = malloc(cantAEnviar);
	memcpy(datos,&(msg->header),TAMANIOHEADER);
	if(msg->header.esHandShake=='0')
		memcpy(datos+TAMANIOHEADER,(msg->Payload),msg->header.tamPayload);

	//Paquete* punteroMsg = datos;
	int enviado = 0; //bytes enviados
	int totalEnviado = 0;

	do
	{
		enviado = send(socketCliente,datos+totalEnviado,cantAEnviar-totalEnviado,0);
		//largo -= totalEnviado;
		totalEnviado += enviado;
		//punteroMsg += enviado; //avanza la cant de bytes que ya mando
	} while (totalEnviado != cantAEnviar);
	free(datos);
}


void EnviarMensaje(int socketFD, char* msg,char emisor[11])
{
	Paquete* paquete = malloc(TAMANIOHEADER + string_length(msg));
	Header header;
	header.esHandShake= '0';
	header.tamPayload = string_length(msg);
	strcpy(header.emisor, emisor);
	//printf("%d",sizeof(header));

	paquete -> header = header;
	paquete -> Payload = msg;
	//paquete.Payload = msg;

	EnviarPaquete(socketFD,paquete,TAMANIOHEADER + string_length(msg));

	free(paquete);
}

void EnviarHandshake(int socketFD,char emisor[11])
{
	Paquete* paquete = malloc(TAMANIOHEADER);
	Header header;
	header.esHandShake = '1';
	header.tamPayload = 0;
	strcpy(header.emisor, emisor);
	//printf("%d",sizeof(header));

	paquete -> header = header;
	//paquete -> Payload = ;
	//paquete.Payload = msg;

	EnviarPaquete(socketFD,paquete,TAMANIOHEADER);

	free(paquete);
}

/*void EnviarHandshake(int socketFD,char emisor[11])
{
	Paquete* paquete = malloc(TAMANIOHEADER + string_length(PAYLOADHANDSHAKE));
	Header header;

	header.esHandShake = '1';
	header.tamPayload = string_length(PAYLOADHANDSHAKE);
	strcpy(header.emisor, emisor);
	//printf("%d",sizeof(header));

	paquete -> header = header;
	paquete -> Payload = PAYLOADHANDSHAKE;
	//paquete.Payload = msg;

	EnviarPaquete(socketFD,paquete,TAMANIOHEADER + string_length(PAYLOADHANDSHAKE));

	free(paquete);
}*/


void EnviarHandshakeString(int socketFD)
{
	char* paquete = STRHANDSHAKE;

	send(socketFD,paquete,strlen(paquete),0);
}


void EnviarMensajeString(int socketFD, char* str)
{
	char* msj;
	int largo = strlen(str);

	asprintf(&msj,"%s%d","0",largo);
	strcat(msj,str);

	//char* mensajeAMandar = "07holaUri";

 	send(socketFD,msj,strlen(msj),0);
}


void RecibirMensajeString(int socketFD)
{
	char* header = malloc(TAMANIOHEADER + 1);
	char buf[256];
	int nbytes = recv(socketFD, header, TAMANIOHEADER, 0);

	if(nbytes > 0)
	{
		if(header[0] == '1')
		{
			printf("Gracias por aceptar mi conexion\n");
		}
		else
		{
			recv(socketFD, buf, header[1], 0);
			printf("El mensaje recibido es: %s\n", buf);
		}
	}
}


/*char* RecibirHandshake(int socketFD)
{
	Paquete* paquete = malloc(TAMANIOHEADER);
	int var = RecibirPaquete(paquete, socketFD, TAMANIOHEADER);


	if (paquete -> header.esHandShake != '1') //chequear que sea un handshake
	{
		var = -1;
	}

	//chequear que sea el emisor
	//if (strcmp(emisor, paquete->header.emisor)!=0) var=-1;

	if (var != -1)
	{
		return "Se recibió el Handshake";
	}
	else
	{
		return "Hubo un error al intentar recibir";
	}
}*/


int RecibirPaquete(void* paquete, int socketFD, unsigned short cantARecibir)
{
	void* datos = malloc(cantARecibir);
	//char* punteroMsg = paquete;
	int recibido = 0;
	int totalRecibido = 0;

	do
	{
		recibido = recv(socketFD, datos + totalRecibido, cantARecibir - totalRecibido, 0);
		//send(socketCliente,punteroMsg+totalEnviado,cantAEnviar-totalEnviado,0);
		totalRecibido += recibido;
	} while (totalRecibido != cantARecibir || recibido!=0);
	memcpy(paquete,datos,cantARecibir);

	free(datos);
	return recibido;
}


int RecibirHeader(int socketFD,Header* headerRecibido)
{
	//TODO: Hacer el malloc en donde invocas a la funcion, asi podemos obtener, cuando finaliza la funcion,
	//el emisor del mensaje y si poder validar si fue un handshake o no

	//Header* headerRecibido = malloc(TAMANIOHEADER);
	//Header* punteroMsg = headerRecibido;
	int var = RecibirPaquete(headerRecibido, socketFD, TAMANIOHEADER);

	//chequear que sea el emisor
	//if (strcmp(emisor, headerRecibido -> emisor) != 0) var = -1;
	if(var<0)
		return ERRORRECEPCION;
	else if(var ==0)
		return CONEXIONCERRADA;
	else {
			if (headerRecibido->tamPayload>=0)
				return headerRecibido -> tamPayload;
			else
				return -1;
		}

}
int RecibirPayload(int socketFD,char* mensaje,unsigned short tamPayload)
{

	void* mensajeRecibido = malloc(tamPayload);
	int cantRecibida = RecibirPaquete(mensajeRecibido, socketFD,tamPayload);
	strcpy(mensaje,mensajeRecibido);
	free(mensajeRecibido);
	return cantRecibida;

}
