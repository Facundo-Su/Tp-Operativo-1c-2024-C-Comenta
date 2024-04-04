#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>
#include "utils/hello.h"
typedef struct
{
    t_queue *cola;
    pthread_mutex_t sem_mutex;
} t_cola;

extern sem_t sem_new;
extern sem_t sem_ready;
extern sem_t sem_grado_multiprogramacion;
extern pthread_mutex_t sem_detener;
extern t_cola * cola_new;
extern t_cola * cola_ready;
extern t_cola * cola_blocked;

char* puerto_escucha;
char* ip_memoria;
char* puerto_memoria;
char* ip_cpu;
char* puerto_cpu_dispatch;
char* puerto_cpu_interrupt;
char* algoritmo;
int quantum;
char **recursos;
char **instancia_recurso;
int grado_multiprogramacion_ini;
int contador_pcb;
bool detener;
typedef enum{
	FIFO,
	RR,
	VRR
}t_planificador;

t_log* logger;
t_config* config;

t_planificador planificador;


int conexion_memoria;
int conexion_cpu;
int conexion_cpu_interrupt;


void asignar_algoritmo(char *algoritmo);
void obtener_configuracion();

void iniciar_recurso();
void recibir_mensaje(int);
t_cola * inicializar_cola();
void inicializar_estructuras();
#endif