#ifndef KERNEL_H_
#define KERNEL_H_

#include <utils/hello.h>

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


t_log* logger;
t_config* config;


int conexion_kernel;
int conexion_memoria;


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


#endif