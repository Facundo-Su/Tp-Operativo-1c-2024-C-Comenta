#ifndef RECURSOS_H_
#define RECURSOS_H_

#include "global.h"

void ejecutar_wait(char*,t_pcb*);
void ejecutar_signal(char*,t_pcb*);
t_recurso_pcb * buscar_recurso_pcb(char*,int );
void agregar_recurso_pcb(int , char*);
t_recurso_pcb *crear_recurso_pcb(char*,int );
void quitar_recurso_pcb(int , char*);
void agregar_cola_bloqueados_recurso(t_recurso * , t_pcb * );
t_pcb * quitar_cola_bloqueados_recurso(t_recurso * );
void liberar_recursos(int );


#endif