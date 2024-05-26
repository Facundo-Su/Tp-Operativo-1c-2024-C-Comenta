#include "interfaces.h"

void io_sleep_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	agregar_cola_ready(pcb);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);

		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_sleep(interfaz->nombre_interface, blocked->unidad_trabajo,blocked->pcb);
	}
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
}

void ejecutar_io_sleep(char * nombre_de_interfaz_sleep,int unidad_trabajo_sleep,t_pcb * pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_de_interfaz_sleep,lista_interfaces);
	if(interfaz == NULL){
		log_error(logger,"No se encontro la interfaz %s", nombre_de_interfaz_sleep);
		finalizar_pcb(pcb);

	}else{
        if(!interfaz->en_uso){
			interfaz->en_uso = true;
			interfaz->pid = pcb->contexto->pid;
            pcb->estado = WAITING;
            list_add(lista_bloqueado_io,pcb);
			enviar_dormir(pcb->contexto->pid,unidad_trabajo_sleep,interfaz->codigo_cliente);
			log_error(logger,"LLEGUEE");

        }else{
			pcb->estado = WAITING;
			t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
			blocked->pcb = pcb;
			blocked->unidad_trabajo = unidad_trabajo_sleep;
			//pcb->contexto->pc--;
            agregar_cola_bloqueados_interfaces(interfaz,blocked);
        }
	}
}

void enviar_dormir(int pid,int tiempo,int codigo_cliente){
	t_paquete * paquete = crear_paquete(EJECUTAR_IO_SLEEP);
	agregar_a_paquete(paquete, &pid, sizeof(int));
	agregar_a_paquete(paquete, &tiempo, sizeof(int));
	enviar_paquete(paquete,codigo_cliente);
	free(paquete);
}

t_interfaz * buscar_interfaz_por_nombre(char* interfaz, t_list * lista){
	int d = list_size(lista);
	log_error(logger, "EL TAMAÑO ES %i", d);
	t_interfaz * valor;
	if(d>0){
		log_error(logger, " NOMBRE ES %s", interfaz);
		for(int c = 0; c<d;c++){
			valor = list_get(lista,c);
			log_error(logger, "EL NOMBRE ES %s", valor->nombre_interface);
			if(strcmp(interfaz,valor->nombre_interface) ==0){
				return valor;
			}
		}
	}
	return NULL;
}
t_interfaz * buscar_interfaz_por_pid(int pid, t_list * lista){
	int d = list_size(lista);
	t_interfaz * valor;
	log_error(logger, "EL PID asass %i", pid);
	if(d>0){
		for(int c = 0; c<d;c++){
			valor = list_get(lista,c);
			log_error(logger, "EL PID %i", valor->pid);
			if(pid == valor->pid){
				log_error(logger, "HHHHHHHHHHHH");
				return valor;
			}
		}
	}
	return NULL;
}

/*void agregar_cola_bloqueados_interfaces(t_interfaz * interfaz, t_pcb * pcb){
    pthread_mutex_lock(&(interfaz->cola_espera->sem_mutex));
    queue_push(interfaz->cola_espera->cola, pcb);
    pthread_mutex_unlock(&(interfaz->cola_espera->sem_mutex));
}
t_pcb * quitar_cola_bloqueados_interfaces(t_interfaz * interfaz){
    pthread_mutex_lock(&(interfaz->cola_espera->sem_mutex));
    t_pcb* pcb = queue_pop(interfaz->cola_espera->cola);
    pthread_mutex_unlock(&(interfaz->cola_espera->sem_mutex));
    return pcb;
}*/
void agregar_cola_bloqueados_interfaces(t_interfaz * interfaz, void * dato){//t_blocked_io * blocked){
    pthread_mutex_lock(&(interfaz->cola_espera->sem_mutex));
    queue_push(interfaz->cola_espera->cola, dato);
    pthread_mutex_unlock(&(interfaz->cola_espera->sem_mutex));
}
void * quitar_cola_bloqueados_interfaces(t_interfaz * interfaz){
    pthread_mutex_lock(&(interfaz->cola_espera->sem_mutex));
    void* dato = queue_pop(interfaz->cola_espera->cola);
    pthread_mutex_unlock(&(interfaz->cola_espera->sem_mutex));
    return dato;
}
void agregar_interfaces(char * nombre_interfaz,int conexion_obtenido){
    t_interfaz* interfaz = malloc(sizeof(t_interfaz));
    interfaz->nombre_interface = nombre_interfaz;
    interfaz->codigo_cliente = conexion_obtenido;
    interfaz->en_uso =false;
    interfaz->cola_espera = inicializar_cola();
    list_add(lista_interfaces,interfaz);
}