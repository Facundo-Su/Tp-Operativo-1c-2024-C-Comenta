#ifndef CREAR_PCB_H_
#define CREAR_PCB_H_

#include "global.h"

t_list* leer_pseudocodigo(FILE* );
char* obtener_ruta(char* );
void crear_pcb(int );
void guardar_proceso_en_memoria(int);
void cargar_lista_instruccion(int, char* );
void enviar_instrucciones(int );


#endif 