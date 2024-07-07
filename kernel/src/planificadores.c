
#include "planificadores.h"

void agregar_cola_new(t_pcb * pcb){
    pthread_mutex_lock(&(cola_new->sem_mutex));
    queue_push(cola_new->cola, pcb);
    pthread_mutex_unlock(&(cola_new->sem_mutex));
    sem_post(&sem_new);
}
void agregar_cola_ready(t_pcb * pcb){
    pcb->estado = READY;
    pthread_mutex_lock(&(cola_ready->sem_mutex));
    queue_push(cola_ready->cola, pcb);
    pthread_mutex_unlock(&(cola_ready->sem_mutex));
    sem_post(&sem_ready);
}
t_pcb * quitar_cola_ready(){
    pthread_mutex_lock(&(cola_ready->sem_mutex));
    t_pcb* pcb = queue_pop(cola_ready->cola);
    pthread_mutex_unlock(&(cola_ready->sem_mutex));
    return pcb;
}

void agregar_cola_vrr(t_pcb * pcb){
    pcb->estado = READY;
    pthread_mutex_lock(&(cola_vrr->sem_mutex));
    queue_push(cola_vrr->cola, pcb);
    pthread_mutex_unlock(&(cola_vrr->sem_mutex));
    sem_post(&sem_ready);
}
t_pcb * quitar_cola_vrr(){
    pthread_mutex_lock(&(cola_vrr->sem_mutex));
    t_pcb* pcb = queue_pop(cola_vrr->cola);
    pthread_mutex_unlock(&(cola_vrr->sem_mutex));
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
        if(detener){
            pthread_mutex_lock(&sem_detener_largo);
        }
        t_pcb * pcb = quitar_cola_new();
        log_info(logger,"Se agrego el proceso %i a la cola READY",pcb->contexto->pid);
        agregar_cola_ready(pcb);

    }
    
}
void planificador_corto_plazo(){
    if(planificador == RR || planificador == VRR){
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
        sigue = true;
        switch (planificador)
        {
        case FIFO:
            de_ready_a_fifo();
            break;
        case RR:
            de_ready_a_round_robin();
            break;
        case VRR:
            de_ready_a_vrr();
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
    pthread_mutex_unlock(&sem_interrupcion);
    de_ready_a_fifo();
}
void de_ready_a_vrr(){
    pthread_mutex_unlock(&sem_interrupcion);
    pthread_mutex_lock(&sem_vrr);
    if(!queue_is_empty(cola_vrr->cola)){
        t_pcb* pcb =quitar_cola_vrr();
        enviar_por_dispatch(pcb);   
    }else{
        de_ready_a_fifo();
    }
}
void *interrupcion_quantum(){
    while(1){
        //if(!queue_is_empty(cola_ready->cola)){
                pthread_mutex_lock(&sem_interrupcion);
                if(planificador == VRR){
                    inicio_vrr = temporal_create();
                }
                //log_error(logger,"%i", queue_size(cola_vrr->cola));
                if(!queue_is_empty(cola_vrr->cola)){
                    pthread_mutex_lock(&(cola_vrr->sem_mutex));
                    t_pcb* pcb = queue_peek(cola_vrr->cola);
                    pthread_mutex_unlock(&(cola_vrr->sem_mutex));
                    //log_error(logger,"EJECUTA QUANTUM DE %i",pcb->contexto->quantum);
                    if(planificador == VRR){
                        pthread_mutex_unlock(&sem_vrr);
                    }
                    pthread_mutex_lock(&sem_quantum);
                    usleep(pcb->contexto->quantum * 1000);
                    if(sigue){
                        log_info(logger,"PID: %i - Desalojado por fin de Quantum", 1); //TODO Agregar el PID!!
                        enviar_mensaje_instrucciones("interrumpido por quantum",conexion_cpu_interrupt,ENVIAR_DESALOJAR);
                    }
                }else{
                    
                   if(planificador == VRR){
                        pthread_mutex_unlock(&sem_vrr);
                    }
                    pthread_mutex_lock(&sem_quantum);
                    usleep(quantum * 1000);
                    if(sigue){
                        log_info(logger,"PID: %i - Desalojado por fin de Quantum", 1); //TODO Agregar el PID!!
                        enviar_mensaje_instrucciones("interrumpido por quantum",conexion_cpu_interrupt,ENVIAR_DESALOJAR);
                    }
                }
            }
}
void enviar_por_dispatch(t_pcb* pcb) {
	char * estado_anterior = estado_a_string(pcb->estado);
	log_info(logger, "PID: %i - Estado Anterior: %s - Estado Actual: RUNNING",pcb->contexto->pid,estado_anterior);
    pcb->estado=RUNNING;
    running= pcb;
    enviar_pcb(pcb->contexto,conexion_cpu,RECIBIR_PCB);
    pthread_mutex_unlock(&sem_quantum);
}

void inciar_planificadores(){
    pthread_create(&hilo_planificador_largo_plazo,NULL,(void*) planificador_largo_plazo,NULL);
	pthread_create(&hilo_planificador_corto_plazo,NULL,(void*) planificador_corto_plazo,NULL);
}
