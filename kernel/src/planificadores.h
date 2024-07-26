#ifndef PLANIFICADORES_H_
#define PLANIFICADORES_H_
#include "global.h"
#include "utils.h"
typedef struct {
    int pid;
    int quantum_restante;
} t_parametros_vrr;

void agregar_cola_new(t_pcb * );
void agregar_cola_ready(t_pcb *);
void agregar_cola_vrr(t_pcb *);
t_pcb * quitar_cola_ready();
t_pcb * quitar_cola_new();
t_pcb * quitar_cola_vrr();
void planificador_largo_plazo();
void planificador_corto_plazo();
void de_ready_a_fifo();
void de_ready_a_round_robin();
void de_ready_a_vrr();
void *interrupcion_quantum();
void enviar_por_dispatch(t_pcb* );
void inciar_planificadores();
void *interrupcion_quantum_rr(int);
void *interrupcion_quantum_vrr(t_parametros_vrr * );
#endif

