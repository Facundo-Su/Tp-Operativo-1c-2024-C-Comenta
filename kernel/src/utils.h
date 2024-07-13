#ifndef UTILS_H_
#define UTILS_H_
#include "recursos.h"
t_pcb * encontrar_pcb(int);
t_pcb * buscar_pcb_listas(int , t_list * );
void ejecutar_script(char* );
void iniciar_proceso(char*);
t_list* leer_script(FILE* );
char* obtener_ruta(char* );
t_pcb * buscar_pcb_bloqueados_io(int );
t_list *obtener_procesos_bloqueados_recursos();
t_list * obtener_procesos_bloqueados_io();
t_list * obtener_procesos_lista(t_list * );
t_list * obtener_procesos_cola(t_cola*);
void listar_proceso_por_estado();
void eliminar_pcb(int pid);
void finalizar_pcb(t_pcb * );
void enviar_memoria_finalizar(int );
#endif