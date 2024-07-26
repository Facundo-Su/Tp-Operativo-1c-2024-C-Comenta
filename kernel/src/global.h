#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>
#include "utils/hello.h"
#include <commons/collections/list.h> 
#include <commons/temporal.h>


typedef struct
{
    t_queue *cola;
    pthread_mutex_t sem_mutex;
} t_cola;
typedef struct{
    char* nombre;
    int instancias;
    t_cola* cola_bloqueados;
}t_recurso;

typedef enum{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
}t_tipo_fs;

typedef enum{
    CREATE,
    DELETE,
    TRUNCATE,
    WRITE,
    READ
}t_operaciones_dialfs;
typedef struct{
    int codigo_cliente;
    char* nombre_interface;
    t_tipo_fs tipo_fs;
    bool en_uso;
    int pid;
    sem_t semaforo_contador;
    sem_t semaforo_uso;
    t_cola* cola_espera;
    sem_t semaforo_uso_ejecucion;
    t_cola* cola_en_ejecucion;
}t_interfaz;

typedef struct{
    char* nombre;
    int instancias;
    int pid;
}t_recurso_pcb;

typedef enum{
	FIFO,
	RR,
	VRR
}t_planificador;
typedef struct 
{
    int posicion;
    t_interfaz* interfaz;
}t_interfaz_encontrada;

typedef struct{
    t_pcb * pcb;
    int unidad_trabajo;
    int nro_marco;
    int tamanio;
    int desplazamiento;
    char* nombre_archivo_crear;
    char* nombre_archivo_eliminar;
    char* nombre_archivo;
    int puntero;
    t_operaciones_dialfs operacion;
}t_blocked_io;

extern sem_t sem_new;
extern sem_t sem_ready;
extern sem_t sem_grado_multiprogramacion;
extern pthread_mutex_t sem_exec;
extern pthread_mutex_t sem_detener;
extern pthread_mutex_t sem_detener_largo;
extern pthread_mutex_t sem_detener_conexion;
extern pthread_mutex_t sem_vrr;
extern pthread_mutex_t sem_quantum;
extern pthread_mutex_t sem_lista_bloqueado_interfaces;
extern pthread_mutex_t sem_memoria;
extern t_cola * cola_new;
extern t_cola * cola_ready;
extern t_cola * cola_blocked;
extern t_cola * cola_vrr;
extern t_cola * cola_rr;

extern t_list * lista_bloqueado_io;

extern int grado_multiprogramacion_ini;
extern int contador_pcb;
extern bool detener;
extern bool primero;
extern bool sigue;
extern t_log* logger;
extern t_config* config;
extern t_planificador planificador;
extern int conexion_memoria;
extern int conexion_cpu;
extern int conexion_cpu_interrupt;
extern int quantum;
extern char* puerto_escucha;
extern char ** recursos_config;
extern char ** instancias_recursos_config;
extern t_list * lista_recursos;
extern t_list * lista_recursos_pcb;
extern pthread_t hilo_planificador_largo_plazo;
extern pthread_t hilo_planificador_corto_plazo;
extern pthread_t hilo_conexion_memoria;
extern pthread_t hilo_conexion_cpu;
extern pthread_t hilo_conexion_cpu_interrupt;
extern t_pcb * running;
extern t_list* lista_interfaces;
extern t_temporal* inicio_vrr;
extern int contador_aux;

//

void asignar_algoritmo(char *);
void obtener_configuracion();
void inicializar_recursos();
void recibir_mensaje(int);
t_cola * inicializar_cola();
void inicializar_estructuras();
void crear_proceso();
t_contexto_ejecucion * crear_contexto();
void liberar_proceso(t_pcb * );
char* estado_a_string(t_estado );
t_registros_pcb* crear_registro();
t_pcb* retorno_pcb();
void manejo_sleep();

#endif