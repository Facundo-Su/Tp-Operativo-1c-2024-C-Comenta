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
	int memoria_llena = 0;
	t_tabla_paginas* tabla = tabla_paginas_segun_pid(*pid);
	
	if(tamanio_asignado(*pid) < *tamanio) { 	//Ampliacion de proceso

	for (int i = 0; i < cantidad_de_marcos; i++){
		t_pagina* pagina_libre = siguiente_pagina_libre(*tabla);
		asignar_marco(*pid, pagina_libre);
		if(memoria_llena == 1) {
			enviar_out_of_memory(OUT_OF_MEMORY, cliente_fd);
			break;
		}
	}
	} else { //Reduccion de proceso
	
	}
}

int tamanio_asignado(int pid) {
	t_marco* aux;
	int aux1;

    for (int i = 0; i < list_size(memoria->marcos); i++)
    {
        aux = list_get(memoria->marcos, i);
        if (pid == aux->pid) {
			aux1 = aux1 + memoria->tamanio_marcos;
		}
        
    }
	return aux1;
}

t_pagina* siguiente_pagina_libre(t_tabla_paginas tabla) {
	t_pagina* aux;

    for (int i = 0; i < list_size(tabla.paginas); i++)
    {
        aux = list_get(tabla.paginas, i);
        if (aux->p == 0) {
			return aux;	
		}
            
    }

	log_error(logger_memoria, "PID - %d No hay paginas que no esten asignadas", tabla.pid);
    abort();
}

void asignar_marco(int pid, t_pagina * pagina){ //Le asigna a una pagina de proceso un marco libre
	t_marco * marco;
	int i = encontrar_marco_libre();
	if(i!=-1){
		marco = list_get(memoria->marcos,i);
		marco->is_free = false;
		marco->pid = pid;
		cargar_en_espacio_memoria(marco->num_marco);
		pagina->num_marco = i;
		pagina->p=1;
	}
	else {
		//memoria_llena = 1;
	} 
}

int encontrar_marco_libre() {
    int i;
	t_marco* marco;
    for(i=0;i<memoria->cantidad_marcos;i++) {
    	marco = list_get(memoria->marcos, i);
		if(marco->is_free) {
            return i;
		}
	}
    return -1;
}

void cargar_en_espacio_memoria(int marco){
	//sem_wait(&contador_espera_cargar);
	//memcpy(memoria->espacio_usuario + (marco * tam_pagina), &datos_obtenidos, sizeof(datos_obtenidos));
	//free(datos_obtenidos);
}

void enviar_out_of_memory(op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}