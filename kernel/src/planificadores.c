
#include "planificadores.h"
void agregar_cola_new(t_pcb * pcb){
    pthread_mutex_lock(&(cola_new->sem_mutex));
    queue_push(cola_new->cola, pcb);
    pthread_mutex_unlock(&(cola_new->sem_mutex));
    sem_post(&sem_new);
}
void agregar_cola_ready(t_pcb * pcb){
    pthread_mutex_lock(&(cola_ready->sem_mutex));
    queue_push(cola_ready->cola, pcb);
    pthread_mutex_unlock(&(cola_ready->sem_mutex));
}
t_pcb * quitar_cola_ready(){
    pthread_mutex_lock(&(cola_ready->sem_mutex));
    t_pcb* pcb = queue_pop(cola_ready->cola);
    pthread_mutex_unlock(&(cola_ready->sem_mutex));
    return pcb;
}
t_pcb * quitar_cola_new(){
    pthread_mutex_lock(&(cola_new->sem_mutex));
    t_pcb* pcb = queue_pop(cola_new->cola);
    pthread_mutex_unlock(&(cola_new->sem_mutex));
    return pcb;
}
void planificador_largo_plazo(){
    while (1)
    {
        sem_wait(&sem_new);
        sem_wait(&sem_grado_multiprogramacion);
        t_pcb * pcb = quitar_cola_new();
        pcb->estado = READY;
        agregar_cola_ready(pcb);
    }
    
}
void planificador_corto_plazo(){
    while (1)
    {

        sem_wait(&sem_ready);
        sem_wait(&sem_exec);
        if(detener){
            pthread_mutex_lock(&sem_detener);
        }
        switch (planificador)
        {
        case FIFO:
            de_ready_a_fifo();
            break;
        case RR:
            de_ready_a_round_robin();
            break;
        case VRR:
        
            break;
        
        default:
            break;
        }
    }
    
    void de_ready_a_fifo(){
        t_pcb* pcb =quitar_cola_ready();
        enviar_por_dispatch(pcb);
    }
    void de_ready_a_round_robin(){
        sem_post(&semaforo_quantum);
        de_ready_a_fifo();
    }
}