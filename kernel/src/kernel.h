#ifndef KERNEL_H_
#define KERNEL_H_

#include <utils/hello.h>
#include "global.h"
#include "planificadores.h"
#include "utils.h"

typedef struct 
{
    int posicion;
    t_interfaz* interfaz;
}t_interfaz_encontrada;




void generar_conexion();
void procesar_conexion(void *);
void iniciar_consola();
void iniciar_servidor_kernel();
void ejecutar_io_sleep(char * nombre_de_interfaz_sleep,int unidad_trabajo_sleep,t_pcb * pcb);
void enviar_dormir(int pid,int tiempo,int codigo_cliente);
void sacar_meter_en_ready(int pid);
t_interfaz * buscar_interfaces_listas(char* interfaz, t_list * lista);
#endif