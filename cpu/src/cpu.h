/*
 * cpu.h
 *
 *  Created on: Sep 2, 2023
 *      Author: utnso
 */

#ifndef SRC_CPU_H_
#define SRC_CPU_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/hello.h>
#include <limits.h>

t_log* logger;
t_config* config;

int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha_dispatch;
char *puerto_escucha_interrupt;
int cantidad_entrada_tlb;

int conexion_memoria;

t_contexto_ejecucion* pcb;



typedef enum{
	FIFO,
	LRU,
}algoritmo_tlb;

algoritmo_tlb algoritmo;

//variable que uso en global
bool recibi_archivo;
bool hay_desalojo;
bool hay_finalizar;
bool hayInterrupcion;
bool instruccion_ejecutando;

t_contexto_ejecucion * contexto_ejecucion;
t_instruccion* instruccion_a_realizar;

typedef enum{
	AX,
	BX,
	CX,
	DX,
    EAX,
    EBX,
    ECX,
    EDX,
    SI,
    DI
}t_estrucutra_cpu;

typedef struct{
	int marco;
	int desplazamiento;
	int nro_pagina;
}t_traduccion;

typedef struct{
	int pid;
	int pagina;
	int marco;
    bool is_free;
    int llegada_fifo;
    int last_time_lru;
}t_estructura_tlb;

t_list* tlb;
int contador_fifo;

//semaforos
sem_t contador_instruccion;
sem_t resize_llegado;
pthread_mutex_t contador_marco_obtenido;
pthread_mutex_t respuesta_ok;


int marco_obtenido;
int tamanio_pagina;
int valor_retorno_resize;

void obtener_configuracion();
void procesar_conexion(void *conexion1);
void iniciar_servidor_cpu();
void generar_conexion_memoria();
void ejecutar_ciclo_de_instruccion(int cliente_fd);
void iniciar_servidor_interrupt(char * puerto);
void transformar_en_instrucciones(char* auxiliar);
char** parsear_instruccion(char* instruccion);
void fetch(int cliente_fd);
void decode(t_instruccion* instrucciones,int cliente_fd);
void solicitar_instruccion_ejecutar_segun_pc(int pc,int pid);
void enviar_recurso_a_kernel(char* recurso, op_code operacion, int cliente_fd);
void enviar_a_memoria_copy_string(int parametro,t_traduccion* traducido,t_traduccion* traducido2);
void enviar_memoria_ajustar_tam(int tamanio_modificar);
void obtener_el_marco(int nro_pagina,op_code operacion);
void asignar_valor_registro(int valor, t_estrucutra_cpu registros_aux);
void enviar_io_stdin_read(char* ,t_traduccion* ,uint32_t ,int );
void enviar_IO_SLEEP(char* parametro,int parametro2,int cliente_fd);
void enviar_io_stdout_write(char* ,t_traduccion* ,uint32_t ,int );
int consultar_tlb(int nro_pagina, int pid);
t_traduccion* mmu_traducir(int );


t_estrucutra_cpu devolver_registro(char* registro);
void setear(t_estrucutra_cpu pos, uint32_t valor) ;
uint32_t obtener_valor(t_estrucutra_cpu pos);



#endif /* SRC_CPU_H_ */
