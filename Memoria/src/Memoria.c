#include "Memoria.h"

//Variables del Archivo de Config
int PUERTO;
uint32_t MARCOS;
uint32_t MARCO_SIZE;
uint32_t ENTRADAS_CACHE;
uint32_t CACHE_X_PROC;
char* REEMPLAZO_CACHE;
unsigned int RETARDO_MEMORIA;
char* IP;

void* BloquePrincipal;
void* ContenidoMemoria;
int tamanioTotalBytesMemoria;
int tamEstructurasAdm;
int cantPagAsignadas;
int socketABuscar;

t_list* tablaCache;
t_list* listaHilos;
bool end;

//semaforos
pthread_mutex_t mutexTablaCache = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexTablaPagina = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexContenidoMemoria = PTHREAD_MUTEX_INITIALIZER;

void obtenerValoresArchivoConfiguracion() {
	t_config* arch = config_create("ArchivoConfiguracion.txt");
	PUERTO= config_get_int_value(arch, "PUERTO");
	MARCOS=config_get_int_value(arch, "MARCOS");
	MARCO_SIZE=config_get_int_value(arch, "MARCO_SIZE");
	ENTRADAS_CACHE=config_get_int_value(arch, "ENTRADAS_CACHE");
	CACHE_X_PROC=config_get_int_value(arch, "CACHE_X_PROC");
	REEMPLAZO_CACHE=config_get_string_value(arch, "REEMPLAZO_CACHE");
	RETARDO_MEMORIA=config_get_int_value(arch, "RETARDO_MEMORIA");
	IP=config_get_string_value(arch, "IP");
	config_destroy(arch);
}

void InicializarTablaDePagina() {
	uint32_t i;
	for(i=0;i<MARCOS;i++){
		TablaDePagina[i].Frame = i;
	}
}

int main(void) {
	obtenerValoresArchivoConfiguracion();
	imprimirArchivoConfiguracion();
	tablaCache = list_create();

	tamEstructurasAdm = sizeof(RegistroTablaPaginacion) * MARCOS;
	tamanioTotalBytesMemoria = (MARCOS * MARCO_SIZE) + tamEstructurasAdm;
	BloquePrincipal = malloc(tamanioTotalBytesMemoria); //Reservo toda mi memoria
	ContenidoMemoria = BloquePrincipal + tamEstructurasAdm; //guardo el puntero donde empieza el contenido
	cantPagAsignadas = 0;


	InicializarTablaDePagina();

	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) userInterfaceHandler, NULL);

	ServidorConcurrente(IP, PUERTO, MEMORIA, &listaHilos, &end, accion);

	pthread_join(hiloConsola, NULL);
	list_destroy_and_destroy_elements(tablaCache, free);
	free(BloquePrincipal);
	exit(3); //TODO: ¿Esto va? Se supone que termina todos los hilos del proceso.
	return 0;
}
