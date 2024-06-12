#include "utils.h"
void finalizar_pcb(t_pcb * pcb){
	sigue = false;
    liberar_recursos(pcb->contexto->pid);
    liberar_proceso(pcb);
}
void eliminar_pcb(int pid){
    t_pcb * pcb = encontrar_pcb(pid);
    finalizar_pcb(pcb);
}

t_pcb*buscar_pcb_bloqueados(int pid){
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		int i = queue_size(recurso->cola_bloqueados->cola);
		if(i>0){
			for(int c = 0; c<i;c++){
				t_pcb * pcb = list_get(recurso->cola_bloqueados->cola->elements,c);
				if(pid == pcb->contexto->pid){
					list_iterator_destroy(iterador);
					list_remove(recurso->cola_bloqueados->cola->elements,c);
					return pcb;
				}
			}
		}
	}
	list_iterator_destroy(iterador);
	return NULL;
}

t_pcb * buscar_pcb_colas(int pid, t_cola * cola){
	int d = queue_size(cola->cola);
	if(d>0){
		for(int c = 0; c<d;c++){
			t_pcb * pcb = list_get(cola->cola->elements,c);
			if(pid == pcb->contexto->pid){
				list_remove(cola->cola->elements,c);
				return pcb;
			}
		}
	}
	return NULL;
}
t_pcb * encontrar_pcb(int pid){
	t_pcb *bloqueado = buscar_pcb_bloqueados(pid);
	if(bloqueado!=NULL){
		return bloqueado;
	}
	t_pcb * ready = buscar_pcb_colas(pid, cola_ready);
	if(ready!=NULL){
		return ready;
	}
    t_pcb * new = buscar_pcb_colas(pid, cola_new);
	if(new!=NULL){
		return ready;
	}
	if(running!=NULL){
		//enviar_mensaje_instrucciones("kernel a interrupt", conexion_cpu_interrupt,ENVIAR_FINALIZAR);
		return NULL;
	}
	return NULL;
}
t_pcb * buscar_pcb_listas(int pid, t_list * lista){
	int d = list_size(lista);
	if(d>0){
		for(int c = 0; c<d;c++){
			t_pcb * pcb = list_get(lista,c);
			if(pid == pcb->contexto->pid){
				return list_remove(lista,c);;
			}
		}
	}
	return NULL;
}