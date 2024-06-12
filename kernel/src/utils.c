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
void iniciar_proceso(char* ruta){
	t_pcb* pcb = retorno_pcb();
	t_paquete* paquete =crear_paquete(CREAR_PROCESO);
	int pid = pcb->contexto->pid;
	ruta = strtok(ruta, "\n");
	agregar_a_paquete(paquete, ruta, strlen(ruta) + 1);
	agregar_a_paquete(paquete, &(pid), sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
	agregar_cola_new(pcb);
	if(primero == true){
		pthread_mutex_unlock(&sem_exec);
	}
	primero = false;
}
t_list* leer_script(FILE* pseudocodigo){
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;
	t_list * instrucciones = list_create();
	int j=0;
    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

    	//log_info(logger_consola_memoria,"el valor es %s" ,instruccion);
    	char* valor_remplazo = strdup(instruccion);
        list_add(instrucciones,valor_remplazo);
        char *instruc_aux_nose23 = list_get(instrucciones,j);
        j++;
       log_info(logger,"la instruccion es  %s",instruc_aux_nose23);
    }
    return instrucciones;
}
void ejecutar_script(char* ruta){
	char *ruta_final = obtener_ruta(ruta);
	FILE* archivo = fopen(ruta_final, "r");
    if (archivo == NULL) {
        log_error(logger, "El archivo %s no pudo ser abierto.", ruta); 
    } else {
        t_list* auxiliar = leer_script(archivo);
        fclose(archivo);
		log_info(logger, "%i",list_size(auxiliar));
		for(int i =0; i< list_size(auxiliar);i++){

			char * instruccion = list_get(auxiliar, i);
			char** instruccion_parseada = string_split(instruccion, " ");
			//log_info(logger, "ASASASASAS");
			log_info(logger, "%s",instruccion_parseada[0]);
			if (strcmp(instruccion_parseada[0], "INICIAR_PROCESO") == 0){
				log_info(logger, "%s",instruccion_parseada[1]);
				//char *ruta_proceso = instruccion[1];
				iniciar_proceso(instruccion_parseada[1]);
			}
		}
    }
}
char* obtener_ruta(char* valor_recibido) {
   

    size_t longitud_path = strlen("/home/utnso/c-comenta-pruebas/");
    size_t longitud_valor = strlen(valor_recibido);
    size_t longitud_total = longitud_path + longitud_valor + 1; // +1 para el carácter nulo

    // Asignar memoria suficiente para la ruta (incluyendo el carácter nulo)
    char* ruta = malloc(longitud_total);
    if (ruta == NULL) {
        log_info(logger, "Error: No se pudo asignar memoria para la ruta\n");
        return NULL; // Manejar el error de asignación de memoria
    }

    // Copiar la parte inicial de la ruta (path_instrucciones)
    strcpy(ruta, "/home/utnso/c-comenta-pruebas");

    // Concatenar el valor recibido (nombre de archivo) a la ruta
    strcat(ruta, valor_recibido);

    return ruta;
}