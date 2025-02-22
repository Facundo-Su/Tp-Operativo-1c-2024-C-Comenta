#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>
#include "utils/hello.h"
#include <commons/collections/list.h> 
#include <string.h>

typedef struct{
	int pid;
	t_list* instrucciones;
}t_instrucciones;

typedef struct {
	int num_marco;
    int num_pagina;
} t_pagina;

typedef struct {
	int num_marco;
    int base;
    bool is_free;
    int pid;
} t_marco;

typedef struct {
	int pid;
    t_list * paginas;
    int tamanio_proceso;
}t_tabla_paginas;

typedef struct{
	void * espacio_usuario;
	t_list * marcos;
	t_list  * lista_tabla_paginas;
	int tamanio_marcos;
	int cantidad_marcos;
	int espacio_disponible;
}t_memoria;

extern t_memoria* memoria;

extern t_list *lista_instrucciones;
extern char *puerto_escucha;
extern int tam_memoria;
extern int tam_pagina;
extern char *path_instrucciones;
extern int retraso_respuesta;
extern useconds_t retardo_respuesta;
extern int auxiliar;

extern t_log* logger_memoria;
extern t_log* logger;
extern t_config* config;


void obtener_configuraciones();
void inicializar_estructuras();
void inicializar_marcos();
void enviar_ok(int cliente_fd) ;
t_tabla_paginas* tabla_paginas_segun_pid(int);

#endif