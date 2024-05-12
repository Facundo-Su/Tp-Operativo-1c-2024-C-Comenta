#ifndef CREAR_PROCESO_H_
#define CREAR_PROCESO_H_

#include "global.h"

t_list* leer_pseudocodigo(FILE*);
char* obtener_ruta(char*);
void crear_proceso(int);
void guardar_proceso_en_memoria(int, int);
void cargar_lista_instruccion(int, char* );
void enviar_instrucciones(int);
t_tabla_paginas *crear_tabla_pagina(int, int);
t_list * crear_pagina(int);

#endif 