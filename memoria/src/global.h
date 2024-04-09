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
t_list* leer_pseudocodigo(FILE* pseudocodigo);
#endif