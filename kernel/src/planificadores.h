#ifndef PLANIFICADORES_H_
#define PLANIFICADORES_H_
#include "global.h"
#include "planificadores.h"
void agregar_cola_new(t_pcb * );
void agregar_cola_ready(t_pcb *);
t_pcb * quitar_cola_ready();
t_pcb * quitar_cola_new();
void planificador_largo_plazo();
void planificador_corto_plazo();
void de_ready_a_fifo();
void de_ready_a_round_robin();
void *interrupcion_quantum();
void enviar_por_dispatch(t_pcb* );
void inciar_planificadores();
#endif

