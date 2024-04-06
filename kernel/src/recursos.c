#include "recursos.h"

void ejecutar_wait(char*nombre,t_pcb*pcb){
	if (list_is_empty(lista_recursos)) {
	    return;
	}
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		if(strcmp(nombre,recurso->nombre) == 0){
			encontro = 1;
			if(recurso->instancias >0){
				recurso->instancias--;
				agregar_recurso_pcb(pcb->contexto->pid,nombre);
				log_info(logger,"PID: %i - Wait: %s - Instancias: %i",pcb->contexto->pid,recurso->nombre,recurso->instancias);
				//enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
				break;
			}else{
				pcb->estado = WAITING;
				log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
				log_info(logger ,"PID: %i - Bloqueado por: %s",pcb->contexto->pid,nombre);
				pcb->contexto->pc--;
                agregar_cola_bloqueados_recurso(recurso, pcb);
				pthread_mutex_unlock(&sem_exec);
				//list_add(lista_bloqueados,pcb);
			}
		}
	}
	if(encontro ==0){
		liberar_proceso(pcb);
		liberar_recursos(pcb->contexto->pid);
		log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE",pcb->contexto->pid);
	}
	list_iterator_destroy(iterador);
}

void ejecutar_signal(char*nombre,t_pcb*pcb){
	if (list_is_empty(lista_recursos)) {
	    return;
	}
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		if(strcmp(nombre,recurso->nombre) == 0){
			encontro = 1;
			t_recurso_pcb * recurso_pcb = buscar_recurso_pcb(nombre, pcb->contexto->pid);
			if(recurso_pcb != NULL){
				recurso->instancias++;
				quitar_recurso_pcb(pcb->contexto->pid,nombre);
				log_info(logger,"PID: %i - Signal: %s - Instancias: %i",pcb->contexto->pid,recurso->nombre,recurso->instancias);
				//enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
				if(!queue_is_empty(recurso->cola_bloqueados->cola)){
					t_pcb* pcb_bloqueado = quitar_cola_bloqueados_recurso(recurso);
					agregar_cola_ready(pcb_bloqueado);
					sem_post(&sem_ready);
				}
			}else{
				liberar_proceso(pcb);
				liberar_recursos(pcb->contexto->pid);
				log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE",pcb->contexto->pid);
			}
		}
	}
	if(encontro ==0){
		liberar_proceso(pcb);
		liberar_recursos(pcb->contexto->pid);
		log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE",pcb->contexto->pid);
	}
	list_iterator_destroy(iterador);
}

t_recurso_pcb * buscar_recurso_pcb(char*nombre,int pid){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
		if((strcmp(nombre,recurso->nombre) == 0)&&(pid == recurso->pid)){
			encontro=1;
			list_iterator_destroy(iterador);
			return recurso;
		}
	}
	if(encontro == 0){
		list_iterator_destroy(iterador);
		return NULL;
	}
}

void agregar_recurso_pcb(int pid, char*nombre){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
		if((strcmp(nombre,recurso->nombre) == 0) && (pid == recurso->pid)){
			encontro = 1;
			recurso->instancias++;
		}
	}
	if(encontro ==0){
		t_recurso_pcb* recurso = crear_recurso_pcb(nombre, pid);
		list_add(lista_recursos_pcb, recurso);	
        }
	list_iterator_destroy(iterador);
}

t_recurso_pcb*crear_recurso_pcb(char*nombre,int pid){
	t_recurso_pcb*recurso_nuevo=malloc(sizeof(t_recurso_pcb));
	recurso_nuevo->nombre = nombre;
	recurso_nuevo->instancias=1;
	recurso_nuevo->pid = pid;
	return recurso_nuevo;
}
void quitar_recurso_pcb(int pid, char*nombre){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
	t_recurso_pcb * aux;
	while(list_iterator_has_next(iterador)){
		t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
		if((strcmp(nombre,recurso->nombre) == 0) && pid == recurso->pid){
			if(recurso->instancias > 1){
				recurso->instancias--;
			}else{
				aux = recurso;
			}
		}
	}
	list_iterator_destroy(iterador);
	if(aux!= NULL){
		list_remove_element(lista_recursos_pcb,aux);
		free(aux);
	}
}


void agregar_cola_bloqueados_recurso(t_recurso * recurso, t_pcb * pcb){
    pthread_mutex_lock(&(recurso->cola_bloqueados->sem_mutex));
    queue_push(recurso->cola_bloqueados->cola, pcb);
    pthread_mutex_unlock(&(recurso->cola_bloqueados->sem_mutex));
}
t_pcb * quitar_cola_bloqueados_recurso(t_recurso * recurso){
    pthread_mutex_lock(&(recurso->cola_bloqueados->sem_mutex));
    t_pcb* pcb = queue_pop(recurso->cola_bloqueados->cola);
    pthread_mutex_unlock(&(recurso->cola_bloqueados->sem_mutex));
    return pcb;
}
void liberar_recursos(int pid){

	t_list_iterator* iterador = list_iterator_create(lista_recursos);

		while(list_iterator_has_next(iterador)){
			t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
			t_recurso_pcb * recurso_pcb = buscar_recurso_pcb(recurso->nombre,pid);
			if(recurso_pcb != NULL){
				int instancias = recurso_pcb->instancias;
				while(instancias!=0){
					if(!queue_is_empty(recurso->cola_bloqueados->cola)){
						t_pcb* pcb = quitar_cola_bloqueados_recurso(recurso);
						agregar_cola_ready(pcb);
						sem_post(&sem_ready);
					}
					recurso->instancias++;
					quitar_recurso_pcb(pid,recurso->nombre);
					instancias --;
				}
			}
		}
		list_iterator_destroy(iterador);
}

