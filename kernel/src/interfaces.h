#ifndef INTERFACES_H_
#define INTERFACES_H_
#include "global.h"
#include "utils.h"
#include "planificadores.h"
void ejecutar_io_sleep(char * ,int ,t_pcb * );
void enviar_dormir(int ,int ,int );
void io_sleep_ready(int );
t_interfaz * buscar_interfaz_por_nombre(char* , t_list * );
void agregar_cola_bloqueados_interfaces(t_interfaz * , void * );
void * quitar_cola_bloqueados_interfaces(t_interfaz * );
void agregar_interfaces(char * ,int );
t_interfaz * buscar_interfaz_por_pid(int pid, t_list * lista);
#endif