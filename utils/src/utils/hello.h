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
	ENVIARREGISTROCPU,
	INSTRUCCIONES_A_MEMORIA,
	MANDAME_PAGINA,
    FINALIZAR,
	CREAR_PROCESO,
	ENVIAR_DESALOJAR,
	ENVIAR_FINALIZAR,
	EJECUTAR_WAIT,
	EJECUTAR_SIGNAL,
	OBTENER_MARCO,
	ENVIO_RESIZE,
	ENVIO_MOV_IN,
	ENVIO_MOV_OUT,
	EJECUTAR_IO_SLEEP,
	EJECUTAR_STDIN_READ,
	EJECUTAR_STDOUT_WRITE,
	EJECUTAR_IO_FS_CREATE,
	EJECUTAR_IO_FS_DELETE,
	EJECUTAR_IO_FS_TRUNCATE,
	EJECUTAR_IO_FS_WRITE,
	EJECUTAR_IO_FS_READ,
	CONEXION_INTERFAZ,
	RESIZE_TAM,
	OUT_OF_MEMORY,
	COPY_STRING_MEMORIA,
	IO_SLEEP,
	EJECUTAR_IO_STDIN_READ,
	OK_STDOUT_WRITE,
	DIRECCION_FISICA,
	ENVIAR_DIRECCION,
	OK_STDIN_READ,
	CONECTAR_IO
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
	TERMINATED
}t_estado;





typedef enum
{
    SET,
	MOV_IN,
	MOV_OUT,
	SUB,
	SUM,
	JNZ,
	RESIZE,
	COPY_STRING,
	WAIT,
	SIGNAL,
	IO_GEN_SLEEP,
	IO_STDIN_READ,
	IO_STDOUT_WRITE,
	IO_FS_CREATE,
	IO_FS_DELETE,
	IO_FS_TRUNCATE,
	IO_FS_WRITE,
	IO_FS_READ,
	EXIT
}op_instrucciones;
typedef struct{
	op_instrucciones nombre;
    t_list* parametros;
}t_instruccion;

typedef struct{
	uint32_t ax;
	uint32_t bx;
	uint32_t cx;
	uint32_t dx;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t si;
	uint32_t di;
}t_registros_pcb;

typedef struct{
	int pid;
	int pc;
	int quantum;
	t_registros_pcb* registros;
}t_contexto_ejecucion;
typedef struct
{
	t_contexto_ejecucion * contexto;
	t_estado estado;
}t_pcb;


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
void enviar_mensaje_instrucciones(char* mensaje, int socket_cliente,op_code operacion);

void empaquetar_registro(t_paquete* paquete, t_registros_pcb* regisroPCB);
void empaquetar_pcb(t_paquete* paquete, t_contexto_ejecucion* contexto);

void mostrar_contexto_ejecucion(t_contexto_ejecucion* contexto_ejecucion);
t_contexto_ejecucion* desempaquetar_pcb(t_list* paquete);
t_registros_pcb* desempaquetar_registros(t_list* paquete, int* posicion);
void enviar_pcb(t_contexto_ejecucion* pcb, int socket_cliente,op_code operacion);


#endif
