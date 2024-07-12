#ifndef KERNEL_H_
#define KERNEL_H_

#include <utils/hello.h>
#include <funcionesfs.h>

char* tipo_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
char* puerto_kernel;
char* ip_memoria;
char* puerto_memoria;
char* path_base_dialfs;
int block_size;
int block_count;
int retraso_compactacion;
char* rutita_prueba = "bitmap.dat";

char* interfaz_name;


t_log* logger;
t_config* config;


int conexion_memoria_prueba;

int conexion_kernel;
int conexion_memoria;

void* info_stdout_write;

pthread_t conexion_kernel_hilo;
pthread_mutex_t mutex_respuesta_stdout_write;


void asignar_algoritmo(char *algoritmo);
void obtener_configuracion();
void iniciar_consola();
void iniciar_interfaz_generica();
void iniciar_interfaz_stdin();
void iniciar_interfaz_stdout();
void iniciar_interfaz_dialfs();
void iniciar_recurso();
void generar_conexion();
void procesar_conexion(void *);
void recibir_mensaje(int);
void enviar_stdin_memoria(int , int , int , int ,void* , int , op_code);
//void enviar_respuesta_borrar_archivo(int cliente_fd,int pid)

#endif