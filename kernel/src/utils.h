#ifndef UTILS_H_
#define UTILS_H_
#include "recursos.h"
t_pcb * encontrar_pcb(int);
t_pcb * buscar_pcb_listas(int , t_list * );
void ejecutar_script(char* );
void iniciar_proceso(char*);
t_list* leer_script(FILE* );
char* obtener_ruta(char* );
#endif