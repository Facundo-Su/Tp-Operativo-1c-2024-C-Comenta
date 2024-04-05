
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
    if(planificador == RR || planificador = VRR){
		 pthread_t hilo_quantum;
		 pthread_create(&hilo_quantum, NULL, interrupcion_quantum, NULL);
		 pthread_detach(hilo_quantum);
	}
    while (1)
    {
        sem_wait(&sem_ready);
        pthread_mutex_lock(&sem_exec);
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
}
void de_ready_a_fifo(){
    t_pcb* pcb =quitar_cola_ready();
    enviar_por_dispatch(pcb);
}
void de_ready_a_round_robin(){
    sem_post(&semaforo_quantum);
    de_ready_a_fifo();
}
void *interrupcion_quantum(){
    while(1){
        if(!queue_is_empty(cola_ready)){
            enviar_mensaje_instrucciones("interrumpido por quantum",conexion_cpu_interrupt,ENVIAR_DESALOJAR);
            usleep(quantum * 1000);
        }
        usleep(200 * 1000);
    }
}
void enviar_por_dispatch(t_pcb* pcb) {
	char * estado_anterior = estado_a_string(pcb->estado);
	log_info(logger, "PID: %i - Estado Anterior: %s - Estado Actual: RUNNING",pcb->pid,estado_anterior);
    pcb->estado=RUNNING;
    pthread_mutex_lock(&mutex_lista_ejecucion);
    pthread_mutex_unlock(&mutex_lista_ejecucion);
    enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
}

