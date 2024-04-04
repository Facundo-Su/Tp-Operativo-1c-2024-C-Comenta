#ifndef KERNEL_H_
#define KERNEL_H_

#include <utils/hello.h>

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
void iniciar_consola();
void iniciar_recurso();
void generar_conexion();
void procesar_conexion(void *);
void recibir_mensaje(int);


#endif