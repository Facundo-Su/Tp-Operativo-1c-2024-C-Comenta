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

t_log* logger;
t_config* config;

int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha_dispatch;
char *puerto_escucha_interrupt;
int conexion_memoria;



//variable que uso en global
bool recibi_archivo;
bool hay_desalojo;
bool hay_finalizar;
bool hayInterrupcion;

t_contexto_ejecucion * contexto_ejecucion;


//semaforos
sem_t contador_instruccion;



void obtener_configuracion();
void procesar_conexion(void *conexion1);
void iniciar_servidor_cpu();
void generar_conexion_memoria();
void ejecutar_ciclo_de_instruccion(int cliente_fd);




#endif /* SRC_CPU_H_ */
