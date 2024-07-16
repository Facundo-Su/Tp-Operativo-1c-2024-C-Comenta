
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
    // t_list *ready_cola = obtener_procesos_cola(cola_ready);
    // printf("Cola READY:\n");
    // if (ready_cola != NULL && list_size(ready_cola) > 0) {
    //     for (int i = 0; i < list_size(ready_cola); i++) {
    //         int pid = (int) list_get(ready_cola, i);
    //         printf("%d\n", pid);
    //     }
    // }

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
    // t_list *ready_cola = obtener_procesos_cola(cola_vrr);
    // printf("Cola READY:\n");
    // if (ready_cola != NULL && list_size(ready_cola) > 0) {
    //     for (int i = 0; i < list_size(ready_cola); i++) {
    //         int pid = (int) list_get(ready_cola, i);
    //         printf("%d\n", pid);
    //     }
    // }
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
        log_info(logger,"PID: %i - Estado Anterior: NEW - Estado Actual: READY",pcb->contexto->pid);
        agregar_cola_ready(pcb);

    }
    
}
void planificador_corto_plazo(){
    // if(planificador == RR || planificador == VRR){
	// 	 pthread_t hilo_quantum;
	// 	 pthread_create(&hilo_quantum, NULL, interrupcion_quantum, NULL);
	// 	 pthread_detach(hilo_quantum);
	// }
    int sem_value;
    while (1)
    {
        sem_wait(&sem_ready);
        sem_getvalue(&sem_ready, &sem_value);
        log_error(logger,"Valor del semáforo después de sem_wait: %d\n", sem_value);
        pthread_mutex_lock(&sem_exec);
        log_warning(logger,"Valor del semáforo después de sem_wait: %d\n", sem_value);
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
    de_ready_a_fifo();
}
void de_ready_a_vrr(){
    //pthread_mutex_lock(&sem_vrr);
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

void iniciar_quantum_vrr(t_parametros_vrr * parametros){
    pthread_t hilo_quantum_vrr;
    pthread_create(&hilo_quantum_vrr, NULL, interrupcion_quantum_vrr, parametros);
    pthread_detach(hilo_quantum_vrr);
}

void *interrupcion_quantum_vrr(t_parametros_vrr * parametros)
{   
    inicio_vrr = temporal_create();
    if(parametros->quantum_restante > 0){
        //log_warning(logger, "EJECUTANDO PID %i QUATUM RESTANTE DE %i",parametros->pid,parametros->quantum_restante);
        usleep(parametros->quantum_restante * 1000);
        
    }else{
        usleep(quantum * 1000);
    }

    interrumpir_cpu(parametros->pid);

}

void iniciar_quantum(int pid)
{
    pthread_t hilo_quantum;
    pthread_create(&hilo_quantum, NULL, interrupcion_quantum_rr, pid);
    pthread_detach(hilo_quantum);
}

void *interrupcion_quantum_rr(int pid)
{
    usleep(quantum * 1000);
    interrumpir_cpu(pid);
}

void interrumpir_cpu(int pid){
    //log_warning(logger, "PID: %i - Interrumpido por fin de Quantum", pid);
    t_paquete* paquete = crear_paquete(ENVIAR_DESALOJAR);
    agregar_a_paquete(paquete, &pid, sizeof(int));
    enviar_paquete(paquete, conexion_cpu_interrupt);
    eliminar_paquete(paquete);
}

void enviar_por_dispatch(t_pcb* pcb) {
	char * estado_anterior = estado_a_string(pcb->estado);
	log_info(logger, "PID: %i - Estado Anterior: %s - Estado Actual: RUNNING",pcb->contexto->pid,estado_anterior);
    pcb->estado=RUNNING;
    running= pcb;
    enviar_pcb(pcb->contexto,conexion_cpu,RECIBIR_PCB);
  
    if(planificador == RR){
        
        iniciar_quantum(pcb->contexto->pid);
    }
    if(planificador == VRR){
        t_parametros_vrr * parametros = malloc(sizeof(t_parametros_vrr));
        parametros->pid = pcb->contexto->pid;
        parametros->quantum_restante = pcb->contexto->quantum;
        iniciar_quantum_vrr(parametros);
    }
    //pthread_mutex_unlock(&sem_quantum);
}

void inciar_planificadores(){
    pthread_create(&hilo_planificador_largo_plazo,NULL,(void*) planificador_largo_plazo,NULL);
	pthread_create(&hilo_planificador_corto_plazo,NULL,(void*) planificador_corto_plazo,NULL);
}
