#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <utils/hello.h>

char *puerto_escucha;
int tam_memoria;
int tam_pagina;
char *path_instrucciones;
int retraso_respuesta;
useconds_t retardo_respuesta;
int auxiliar;

t_log* logger_memoria;
t_config* config;
t_log* logger;


void obtener_configuraciones();
void iniciar_servidor_memoria(char *puerto) ;
void iterator(char* value);
void procesar_conexion(void* socket);

#endif