#include "interfaces.h"

void io_sleep_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	//como pongo el bool que muestre true o false en vez de 0 o 1
	log_error(logger,"ya termine de usar que pase el siguiente ya tengo el estado en uso= %i",interfaz->en_uso);
	log_error(logger,"el estado de interfaz %s es %i", interfaz->nombre_interface,interfaz->en_uso);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_sleep(interfaz->nombre_interface, blocked->unidad_trabajo,blocked->pcb);
	}
}

void ejecutar_io_stdin_read(char* nombre_interfaz, int marco,int desplazamiento,int tamanio,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
				list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_stdin_read(nombre_interfaz,marco,desplazamiento,tamanio,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->unidad_trabajo = 0;
				blocked->nro_marco = marco;
				blocked->tamanio = tamanio;
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}

void ejecutar_io_stdin_write(char* nombre_interfaz, int marco,int desplazamiento,int tamanio,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            	list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_stdin_write(nombre_interfaz,marco,desplazamiento,tamanio,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->unidad_trabajo = 0;
				blocked->nro_marco = marco;
				blocked->tamanio = tamanio;
				blocked->desplazamiento=desplazamiento;
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE FWRITE %i, %i, %i", blocked->nro_marco,blocked->desplazamiento,blocked->tamanio);
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}


void enviar_a_io_stdin_write(char* nombre_interfaz, int marco,int desplazamiento,int tamanio,t_pcb* pcb,int codigo_cliente){
	t_paquete* paquete=crear_paquete(EJECUTAR_STDOUT_WRITE);
	log_error(logger,"ENVIEE PID %i, MARCO %i, DESPLAZAMIENTO %i, tamanio %i,",pcb->contexto->pid,marco,desplazamiento,tamanio);
	agregar_a_paquete(paquete, &(pcb->contexto->pid), sizeof(int));
	agregar_a_paquete(paquete,&marco,sizeof(int));
	agregar_a_paquete(paquete,&desplazamiento,sizeof(int));
	agregar_a_paquete(paquete,&tamanio,sizeof(int));
	enviar_paquete(paquete, codigo_cliente);
	eliminar_paquete(paquete);
}

void enviar_a_io_stdin_read(char* nombre_interfaz, int marco,int desplazamiento,int tamanio,t_pcb* pcb,int codigo_cliente){
	t_paquete* paquete=crear_paquete(EJECUTAR_STDIN_READ);
	agregar_a_paquete(paquete, &(pcb->contexto->pid), sizeof(int));
	agregar_a_paquete(paquete,&marco,sizeof(int));
	agregar_a_paquete(paquete,&desplazamiento,sizeof(int));
	agregar_a_paquete(paquete,&tamanio,sizeof(int));

	enviar_paquete(paquete, codigo_cliente);
	log_warning(logger,"mensaje enviado");
	log_warning(logger,"envie con el codigo de cliente %i",codigo_cliente);
	eliminar_paquete(paquete);
}


void io_stdout_write_ready(int pid){

	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		log_error(logger, "SALI DE BLOQUEADO FWRITE");
		ejecutar_io_stdin_write(interfaz->nombre_interface, blocked->nro_marco,blocked->desplazamiento, blocked->tamanio,blocked->pcb);
	}
}

void io_stdin_read_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_stdin_read(interfaz->nombre_interface, blocked->nro_marco,blocked->desplazamiento, blocked->tamanio,blocked->pcb);
	}
}


void io_stdin_write_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_stdin_write(interfaz->nombre_interface, blocked->nro_marco,blocked->desplazamiento, blocked->tamanio,blocked->pcb);
	}
}

void io_fs_create_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_fs_create(interfaz->nombre_interface,blocked->nombre_archivo_crear,blocked->pcb);
	}
}


void io_fs_delete_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_fs_delete(interfaz->nombre_interface,blocked->nombre_archivo_eliminar,blocked->pcb);
	}
}

void io_fs_write_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
    t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_fs_write(interfaz->nombre_interface,blocked->nombre_archivo,blocked->nro_marco,blocked->desplazamiento,blocked->tamanio,blocked->puntero,blocked->pcb);
	}
}

void io_fs_read_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
	t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_fs_read(interfaz->nombre_interface,blocked->nombre_archivo,blocked->nro_marco,blocked->desplazamiento,blocked->tamanio,blocked->puntero,blocked->pcb);
	}
}

void io_fs_truncate_ready(int pid){
	t_pcb* pcb= buscar_pcb_listas(pid,lista_bloqueado_io);
	t_interfaz *interfaz = buscar_interfaz_por_pid(pid,lista_interfaces);
	interfaz->en_uso = false;
	interfaz->pid = -1;
	log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: READY",pcb->contexto->pid);
	vuelta_io_vrr(pcb);
	//pthread_mutex_unlock(&sem_exec);
	if(!queue_is_empty(interfaz->cola_espera->cola)){
		t_blocked_io * blocked= quitar_cola_bloqueados_interfaces(interfaz);
		//log_info(logger,"PID: %i - Estado Anterior: WAITING - Estado Actual: RUNNING",pcb->contexto->pid);
		//agregar_cola_ready(pcb_blocked);
		ejecutar_io_fs_truncate(interfaz->nombre_interface,blocked->nombre_archivo,blocked->tamanio,blocked->pcb);
	}
}


void ejecutar_io_sleep(char * nombre_de_interfaz_sleep,int unidad_trabajo_sleep,t_pcb * pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_de_interfaz_sleep,lista_interfaces);
	if(interfaz == NULL){
		log_error(logger,"No se encontro la interfaz %s", nombre_de_interfaz_sleep);
		finalizar_pcb(pcb);
	}else{
		log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
        if(!interfaz->en_uso){
			interfaz->en_uso = true;
			interfaz->pid = pcb->contexto->pid;
            pcb->estado = WAITING;
			sigue = false;
			pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            list_add(lista_bloqueado_io,pcb);
			pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
			//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			enviar_dormir(pcb->contexto->pid,unidad_trabajo_sleep,interfaz->codigo_cliente);
			log_warning(logger,"nadie esta en uso , puedo mandar directamente");
			pthread_mutex_unlock(&sem_exec);

        }else{
			pcb->contexto->quantum= obtener_tiempo_vrr();
			pcb->estado = WAITING;
			t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
			blocked->pcb = pcb;
			blocked->unidad_trabajo = unidad_trabajo_sleep;
			//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
			sigue = false;
			log_warning(logger,"hay alguien que esta usando , me paso a bloqueado");
            agregar_cola_bloqueados_interfaces(interfaz,blocked);
			pthread_mutex_unlock(&sem_exec);
        }
	}
}	

void ejecutar_io_fs_create(char *nombre_interfaz,char* nombre_archivo_f_create,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            	list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_f_create(nombre_interfaz,nombre_archivo_f_create,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->nombre_archivo_crear = nombre_archivo_f_create;
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE F_CREATE con el nombre de archivo %s",blocked->nombre_archivo_crear);
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}


void ejecutar_io_fs_write(char *nombre_interfaz,char* nombre_archivo_f_write,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            	list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_fs_write(nombre_interfaz,nombre_archivo_f_write,marco,desplazamiento,tamanio,puntero,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->nombre_archivo = nombre_archivo_f_write;
				blocked->unidad_trabajo = 0;
				blocked->nro_marco = marco;
				blocked->tamanio = tamanio;
				blocked->desplazamiento=desplazamiento;
				blocked->puntero = puntero;
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE FWRITE %i, %i, %i", blocked->nro_marco,blocked->desplazamiento,blocked->tamanio);
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE F_WRITE con el nombre de archivo %s",blocked->nombre_archivo);
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}


void ejecutar_io_fs_read(char *nombre_interfaz,char* nombre_archivo_f_read,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            	list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_f_read(nombre_interfaz,nombre_archivo_f_read,marco,desplazamiento,tamanio,puntero,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->nombre_archivo = nombre_archivo_f_read;
				blocked->unidad_trabajo = 0;
				blocked->nro_marco = marco;
				blocked->tamanio = tamanio;
				blocked->desplazamiento=desplazamiento;
				blocked->puntero = puntero;
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE F_read %i, %i, %i", blocked->nro_marco,blocked->desplazamiento,blocked->tamanio);
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE F_WRITE con el nombre de archivo %s",blocked->nombre_archivo);
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}



void ejecutar_io_fs_delete(char *nombre_interfaz,char* nombre_archivo_f_delete,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            	list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_f_create(nombre_interfaz,nombre_archivo_f_delete,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->nombre_archivo_eliminar = nombre_archivo_f_delete;
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE F_CREATE con el nombre de archivo %s",blocked->nombre_archivo_eliminar);
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}


void ejecutar_io_fs_truncate(char *nombre_interfaz,char* nombre_archivo_f_truncate,int tamanio,t_pcb* pcb){
	t_interfaz * interfaz = buscar_interfaz_por_nombre(nombre_interfaz,lista_interfaces);
	if(interfaz == NULL){
			log_error(logger,"No se encontro la interfaz %s", nombre_interfaz);
			finalizar_pcb(pcb);
		}else{
			log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->contexto->pid);
			if(!interfaz->en_uso){
				interfaz->en_uso = true;
				interfaz->pid = pcb->contexto->pid;
				pcb->estado = WAITING;
				pcb->contexto->quantum= obtener_tiempo_vrr();
				sigue = false;
				pthread_mutex_lock(&sem_lista_bloqueado_interfaces);
            	list_add(lista_bloqueado_io,pcb);
				pthread_mutex_unlock(&sem_lista_bloqueado_interfaces);
				enviar_a_io_f_truncate(nombre_interfaz,nombre_archivo_f_truncate,tamanio,pcb,interfaz->codigo_cliente);
				pthread_mutex_unlock(&sem_exec);

			}else{
				pcb->estado = WAITING;
				t_blocked_io * blocked = malloc(sizeof(t_blocked_io));
				pcb->contexto->quantum= obtener_tiempo_vrr();
				blocked->pcb = pcb;
				blocked->nombre_archivo = nombre_archivo_f_truncate;
				blocked->tamanio = tamanio;
				log_error(logger, "ESTOY ENTRANDO A BLOQUEADO DE F_CREATE con el nombre de archivo %s",blocked->nombre_archivo);
				//log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING2",pcb->contexto->pid);
				sigue = false;
				agregar_cola_bloqueados_interfaces(interfaz,blocked);
				pthread_mutex_unlock(&sem_exec);
			}
		}
}


void enviar_a_io_f_create(char *nombre_interfaz,char* nombre_archivo_f_create,t_pcb* pcb,int codigo_cliente){
	t_paquete * paquete = crear_paquete(EJECUTAR_IO_FS_CREATE);
	agregar_a_paquete(paquete, nombre_archivo_f_create, strlen(nombre_archivo_f_create) + 1);
	agregar_a_paquete(paquete, &pcb->contexto->pid, sizeof(int));
	enviar_paquete(paquete,codigo_cliente);
	log_warning (logger, "envie el nombre de archivo %s", nombre_archivo_f_create);
	free(paquete);
}

void enviar_a_io_f_truncate(char *nombre_interfaz,char* nombre_archivo_f_truncate,int tamanio,t_pcb* pcb,int codigo_cliente){
	t_paquete * paquete = crear_paquete(EJECUTAR_IO_FS_TRUNCATE);
	agregar_a_paquete(paquete, nombre_archivo_f_truncate, strlen(nombre_archivo_f_truncate) + 1);
	agregar_a_paquete(paquete, &tamanio, sizeof(int));
	agregar_a_paquete(paquete, &pcb->contexto->pid, sizeof(int));
	enviar_paquete(paquete,codigo_cliente);
	log_warning (logger, "envie el nombre de archivo %s", nombre_archivo_f_truncate);
	free(paquete);
}

void enviar_a_io_fs_write(char *nombre_interfaz,char* nombre_archivo_f_write,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb,int codigo_cliente){
	t_paquete * paquete = crear_paquete(EJECUTAR_IO_FS_WRITE);
	agregar_a_paquete(paquete, nombre_archivo_f_write, strlen(nombre_archivo_f_write) + 1);
	agregar_a_paquete(paquete, &marco, sizeof(int));
	agregar_a_paquete(paquete, &desplazamiento, sizeof(int));
	agregar_a_paquete(paquete, &tamanio, sizeof(int));
	agregar_a_paquete(paquete, &puntero, tamanio);
	agregar_a_paquete(paquete, &pcb->contexto->pid, sizeof(int));
	enviar_paquete(paquete,codigo_cliente);
	log_warning (logger, "envie el nombre de archivo %s", nombre_archivo_f_write);
	free(paquete);
}


void enviar_a_io_f_read(char *nombre_interfaz,char* nombre_archivo_f_read,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb,int codigo_cliente){
	t_paquete * paquete = crear_paquete(EJECUTAR_IO_FS_READ);
	agregar_a_paquete(paquete, nombre_archivo_f_read, strlen(nombre_archivo_f_read) + 1);
	agregar_a_paquete(paquete, &marco, sizeof(int));
	agregar_a_paquete(paquete, &desplazamiento, sizeof(int));
	agregar_a_paquete(paquete, &tamanio, sizeof(int));
	agregar_a_paquete(paquete, &puntero, tamanio);
	agregar_a_paquete(paquete, &pcb->contexto->pid, sizeof(int));
	enviar_paquete(paquete,codigo_cliente);
	log_warning (logger, "envie el nombre de archivo %s", nombre_archivo_f_read);
	free(paquete);
}


void enviar_a_io_f_delete(char *nombre_interfaz,char* nombre_archivo_f_delete,t_pcb* pcb,int codigo_cliente){
	t_paquete * paquete = crear_paquete(EJECUTAR_IO_FS_DELETE);
	agregar_a_paquete(paquete, nombre_archivo_f_delete, strlen(nombre_archivo_f_delete) + 1);
	agregar_a_paquete(paquete, &pcb->contexto->pid, sizeof(int));
	enviar_paquete(paquete,codigo_cliente);
	log_warning (logger, "envie el nombre de archivo %s", nombre_archivo_f_delete);
	free(paquete);
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
    if (d > 0) {
        for (int c = 0; c < d; c++) {
            valor = list_get(lista, c);
            if (strcmp(interfaz, valor->nombre_interface) == 0) {
                return valor;
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
			valor = list_get(lista,c);
			if(pid == valor->pid){
				//log_error(logger, "HHHHHHHHHHHH");
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
void agregar_interfaces(char * nombre_interfaz, int conexion_obtenido){
    t_interfaz* interfaz = malloc(sizeof(t_interfaz));
    interfaz->nombre_interface = strdup(nombre_interfaz); // Duplica la cadena
    interfaz->codigo_cliente = conexion_obtenido;
    interfaz->en_uso = false;
    interfaz->cola_espera = inicializar_cola();

	log_info(logger, "Se agrego la interfaz %s", interfaz->nombre_interface);
	log_warning(logger, "el codigo de cliente que recibi es : %i ",interfaz->codigo_cliente);
    list_add(lista_interfaces, interfaz);
}
int obtener_tiempo_vrr(){
	int restante =0;
	if(planificador == VRR){
		int tiempo = temporal_gettime(inicio_vrr);
		temporal_destroy(inicio_vrr);
		if(tiempo < quantum){
			restante = quantum-tiempo;
			log_info(logger, "%i",tiempo);
			log_info(logger, "%i",restante);
		}
	}
	return restante;
}
void vuelta_io_vrr(t_pcb * pcb){
	if(planificador== VRR){
		log_info(logger,"PID: %i - Agregado cola de prioridades vrr",pcb->contexto->pid);
		if(pcb->contexto->quantum ==0){
			agregar_cola_ready(pcb);
		}else{
			agregar_cola_vrr(pcb);
		}
	}else{
		agregar_cola_ready(pcb);
	}
}
