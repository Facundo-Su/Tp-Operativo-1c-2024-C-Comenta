#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<assert.h>
#include <bits/types.h>
#include<readline/readline.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>

#include <time.h>

/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/
void decir_hola(char* quien);

typedef struct
{
	int size;
	void* stream;
} t_buffer;

extern t_log* logger;

typedef enum
{
	MENSAJE,
	PAQUETE,
    RECIBIR_PCB,
    FINALIZAR
}op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum{
	NEW,
	READY,
	RUNNING,
	WAITING,
	TERMINATED,
}t_estado;



void* serializar_paquete(t_paquete* , int );
void crear_buffer(t_paquete* paquete);
t_paquete* crear_paquete(op_code cod_op);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void* recibir_buffer(int *size, int socket_cliente);
char* recibir_instruccion(int socket_cliente);
char* obtener_mensaje(int socket_cliente);
void enviar_mensaje(char* , int );
t_list* recibir_paquete(int socket_cliente);
int recibir_operacion(int socket_cliente);
int crear_conexion(char *ip, char* puerto);
t_config* cargar_config(char* path);
int iniciar_servidor(char* puerto);
int esperar_cliente(int socket_servidor);
void recibir_mensaje(int socket_servidor);

#endif
