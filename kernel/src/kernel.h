#ifndef KERNEL_H_
#define KERNEL_H_

#include <utils/hello.h>
#include "global.h"
#include "planificadores.h"
#include "utils.h"
#include "interfaces.h"


void generar_conexion();
void procesar_conexion(void *);
void iniciar_consola();
void iniciar_servidor_kernel();
void enviar_memoria_finalizar(int pid);
t_tipo_fs obtener_el_tipo_fs(char* tipo);
#endif