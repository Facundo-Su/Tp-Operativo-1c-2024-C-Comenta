#include "interfaces.h"

void sacar_meter_en_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	agregar_cola_ready(pcb);
    t_pcb pcb_blocked = quitar_cola_bloqueados_interfaces(interfaz);
    agregar_cola_ready(pcb_blocked);
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
}

void ejecutar_io_sleep(char * nombre_de_interfaz_sleep,int unidad_trabajo_sleep,t_pcb * pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_de_interfaz_sleep,lista_interfaces);
	if(interfaz == NULL){
		log_error(logger,"No se encontro la interfaz %s", nombre_de_interfaz_sleep);
		finalizar_pcb(pcb);

	}else{
        if(!interfaz->en_uso){
            enviar_dormir(pcb->contexto->pid,unidad_trabajo_sleep,interfaz->codigo_cliente);
            pcb->estado = WAITING;
            list_add(lista_bloqueado_io,pcb);
        }else{
            interfaz->en_uso = true;
            interfaz->pid = pcb->contexto->pid;
            pcb->estado = WAITING;
            agregar_cola_bloqueados_interfaces(interfaz,pcb);
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
	t_interfaz * valor;
	if(d>0){
		for(int c = 0; c<d;c++){

			t_interfaz * valor = list_get(lista,c);
			if(interfaz == valor->nombre_interface){
				return valor(lista,c);
			}
		}
	}
	return NULL;
}
t_interfaz * buscar_interfaz_por_pid(int pid, t_list * lista){
	int d = list_size(lista);
	t_interfaz * valor;
	if(d>0){
		for(int c = 0; c<d;c++){

			t_interfaz * valor = list_get(lista,c);
			if(pid == valor->pid){
				return valor;
			}
		}
	}
	return NULL;
}

void agregar_cola_bloqueados_interfaces(t_interfaz * interfaz, t_pcb * pcb){
    pthread_mutex_lock(&(interfaz->cola_espera->sem_mutex));
    queue_push(interfaz->cola_espera->cola, pcb);
    pthread_mutex_unlock(&(interfaz->cola_espera->sem_mutex));
}
t_pcb * quitar_cola_bloqueados_interfaces(t_interfaz * interfaz){
    pthread_mutex_lock(&(interfaz->cola_espera->sem_mutex));
    t_pcb* pcb = queue_pop(interfaz->cola_espera->cola);
    pthread_mutex_unlock(&(interfaz->cola_espera->sem_mutex));
    return pcb;
}
void agregar_interfaces(char * nombre_interfaz,int conexion_obtenido){
    t_interfaz* interfaz = malloc(sizeof(t_interfaz));
    interfaz->nombre_interface = nombre_interfaz;
    interfaz->codigo_cliente = conexion_obtenido;
    interfaz->en_uso =false;
    interfaz->cola_espera = inicializar_cola();
    list_add(lista_interfaces,interfaz);
}