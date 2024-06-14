#include "envio_resize.h"
#include "finaliza_proceso.h"

/*
Ampliacion: Al final del mismo
Si la memoria se encuentra llena: Out Of Memory.

Reducción:  Al final del mismo. 
Si es necesario, desde el final hasta la primera

Dudas: 
-Que pasa si un proceso con 8 bytes de tamanio se le asignan 200 bytes de tamanio
-

*/

void envio_resize (int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int* pid = list_get(lista,0);
	int* tamanio = list_get(lista,1);
	int cantidad_de_marcos = *tamanio/memoria->tamanio_marcos;
	t_tabla_paginas* tabla = list_create();
	tabla = tabla_paginas_segun_pid(*pid);
	int memoria_llena = 0;
	log_error(logger_memoria, "PID - %d Tamanio - %d Cantidad de marcos - %d", *pid, *tamanio, cantidad_de_marcos);
	log_warning(logger_memoria,"el tamanio que tiene asigando es - %d", tamanio_asignado(*pid));
	if(tamanio_asignado(*pid) < *tamanio) { 	//Ampliacion de proceso
		for (int i = 0; i < cantidad_de_marcos; i++){
			t_pagina* pagina_libre = siguiente_pagina_libre(tabla);
			log_error(logger_memoria, "PID - %d Pagina libre: %d", *pid, pagina_libre->num_pagina);
			asignar_marco(*pid, pagina_libre, &memoria_llena);
			if(memoria_llena == 1) {
				enviar_out_of_memory(OUT_OF_MEMORY, cliente_fd);
				return; //Podria ser un break creo
			}
		}
	} else { //Reduccion de proceso

	while (tamanio_asignado(*pid) >= *tamanio){
		t_pagina* pagina_libre = ultima_pagina_asignada(tabla);
		if (pagina_libre == NULL) {
                log_error(logger_memoria, "PID - %d No se pudo encontrar una página asignada para liberar", *pid);
                return;
            }
		liberar_pagina_y_marco(pagina_libre);
	}
	}
}

int tamanio_asignado(int pid) {
	t_marco* aux;
	int aux1=0;

    for (int i = 0; i < list_size(memoria->marcos); i++)
    {
        aux = list_get(memoria->marcos, i);
        if (pid == aux->pid) {
			aux1 += memoria->tamanio_marcos;
		}
    }
	return aux1;
}

t_pagina* siguiente_pagina_libre(t_tabla_paginas* tabla) {
	t_pagina* aux;

	if(list_size(tabla->paginas) == 0) {

		list_add(tabla,)
		return NULL;
	}

    for (int i = 0; i < list_size(tabla->paginas); i++)
    {
		log_error(logger_memoria, " PID de tabla paginas segun pid - %d", tabla->pid);
        aux = list_get(tabla->paginas, i);
        if (aux->p == 0) {
			return aux;	
		}
            
    }
	log_error(logger_memoria, "PID - %d No hay paginas que no esten asignadas a un marco", tabla->pid);
    return NULL;
}

void asignar_marco(int pid, t_pagina * pagina, int *memoria_llena){ //Le asigna a una pagina de proceso un marco libre
	t_marco * marco;
	int i = encontrar_marco_libre();
	if(i!=-1){
		marco = list_get(memoria->marcos,i);
		marco->is_free = false;
		marco->pid = pid;
		pagina->num_marco = i;
		pagina->p=1;
	}
	else {
		*memoria_llena = 1;
	} 
}

int encontrar_marco_libre() {
    int i;
	t_marco* marco;
    for(i=0;i<memoria->cantidad_marcos;i++) {
		log_warning(logger_memoria, "cantidad de marcos es - %d", list_size(memoria->marcos));
    	marco = list_get(memoria->marcos, i);
		if(marco->is_free) {

            return i;
		}
	}
    return -1;
}

t_pagina* ultima_pagina_asignada(t_tabla_paginas* tabla) {
	t_pagina* aux;
	t_pagina* ultima = NULL;

    for (int i = 0; i < list_size(tabla->paginas); i++)
    {  
		aux = list_get(tabla->paginas, i);
        if (aux->num_marco == 1) {
			ultima = aux;
		}    
    }
	if (ultima != NULL && ultima->num_pagina != -1) {
        return ultima;
    }

	log_error(logger_memoria, "PID - %d El proceso ya no tiene paginas para ser liberadas", tabla->pid);
    return NULL;
}

void liberar_pagina_y_marco(t_pagina* pagina) {
	t_marco* marco = marco_segun_pagina(pagina);
	 if (marco != NULL) {
        marco->is_free = true;
        marco->pid = -1;
    }
    pagina->num_marco = -1;
    pagina->p = 0;
}

void enviar_out_of_memory(op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}