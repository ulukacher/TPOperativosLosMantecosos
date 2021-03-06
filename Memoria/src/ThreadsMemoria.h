#ifndef THREADSMEMORIA_H_
#define THREADSMEMORIA_H_

#include "Memoria.h"

uint32_t Hash(uint32_t pid, uint32_t pag);
uint32_t FrameLookup(uint32_t pid, uint32_t pag);
uint32_t cuantasPagTieneTodos(uint32_t pid);
uint32_t cuantasPagTieneVivos(uint32_t pid);

void IniciarPrograma(uint32_t pid, uint32_t cantPag, int socketFD);
void SolicitarBytes(uint32_t pid, uint32_t numPag, uint32_t offset,	uint32_t tam, int socketFD);
void AlmacenarBytes(Paquete paquete, int socketFD);
void AsignarPaginas(uint32_t pid, uint32_t cantPag, int socketFD);
void LiberarPaginas(uint32_t pid, uint32_t numPag, int socketFD);
void FinalizarPrograma(uint32_t pid, int socketFD);

int RecibirPaqueteMemoria (int socketFD, char receptor[11], Paquete* paquete);
void accion(void* socket);

#endif /* THREADSMEMORIA_H_ */
