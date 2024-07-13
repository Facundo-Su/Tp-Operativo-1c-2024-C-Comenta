#include "envio_resize.h"
#include "finaliza_proceso.h"

void envio_resize(int cliente_fd){
	t_list* lista = recibir_paquete(cliente_fd);
	int* pid = list_get(lista,0);
	int* tamanio = list_get(lista,1);
	t_tabla_paginas *tabla = tabla_paginas_segun_pid(*pid);
	int tamanio_proceso = tabla->tamanio_proceso;
	//log_error(logger_memoria, "TAMAÑIO proceso %i", tamanio_proceso);
	//log_error(logger_memoria, "TAMAÑIO memoria %i", memoria->espacio_disponible);
	//log_warning(logger_memoria, "PID - %i TAMAÑIO - %i", *pid, *tamanio);
	
	if(*tamanio<tamanio_proceso){
		int tamanio_a_eliminar = tamanio_proceso-*tamanio;
		ejecutar_reduccion(tamanio_a_eliminar, tabla);
		enviar_respuesta_resize (cliente_fd,0);
	}
	else{
		int tamanio_a_agregar = *tamanio - tamanio_proceso;
		//log_error(logger_memoria, "TAMAÑIO a agregar %i", tamanio_a_agregar);
		//log_error(logger_memoria, "TAMAÑIO a agregar %i", tamanio_a_agregar);
		if(tamanio_a_agregar > memoria->espacio_disponible){
			//log_error(logger_memoria,"ERROR NO PUEDO AGRANDAR");
			enviar_respuesta_resize(cliente_fd, -1);
		}else{
			ejecutar_ampliacion(tamanio_a_agregar,tabla);
			enviar_respuesta_resize (cliente_fd,0);
		}
		
	}
}
void ejecutar_ampliacion(int tamanio_a_agregar, t_tabla_paginas * tabla){
	memoria->espacio_disponible-=tamanio_a_agregar;
	tabla->tamanio_proceso += tamanio_a_agregar;
	//log_error(logger_memoria,"tamanio de proceso  agrandar es %i", tamanio_a_agregar);
	//log_error(logger_memoria,"tamanio de proceso  actual es %i", tabla->tamanio_proceso);
	int cant_paginas_nuevas = (tamanio_a_agregar + tam_pagina -1)/tam_pagina;
	int cant_paginas = list_size(tabla->paginas);
	log_info(logger_memoria, "PID: %i - Tamaño Actual: %i - Tamaño a Ampliar: %i",tabla->pid, cant_paginas, tamanio_a_agregar*tam_pagina);
	crear_paginas(cant_paginas_nuevas,cant_paginas,tabla);
	
}

void crear_paginas(int paginas_necesarias, int contador, t_tabla_paginas * tabla){
	for(int i =0; i<paginas_necesarias;i++){
		t_pagina * pagina = malloc(sizeof(t_pagina)); 
		pagina->num_pagina = contador;
		asignar_marco(tabla->pid, pagina);
		list_add(tabla->paginas,pagina);
		contador++;
	}
}
void asignar_marco(int pid, t_pagina * pagina){ 

	int i = encontrar_marco_libre();
	if(i ==-1){
		//log_error(logger_memoria,"ERROR GIL");
	}
	t_marco * marco = list_get(memoria->marcos, i);
	marco = list_get(memoria->marcos,i);
	marco->is_free = false; 
	marco->pid = pid;
	//log_warning(logger_memoria,"asigne el marco %d",marco->num_marco);
	pagina->num_marco = i;
}
int encontrar_marco_libre() {
    int i;
    for(i=0;i<memoria->cantidad_marcos;i++) {
    	t_marco *marco = list_get(memoria->marcos, i);
		if(marco->is_free) {
            return i;
		}
	}
    return -1;
}
void ejecutar_reduccion(int tamanio_a_eliminar, t_tabla_paginas * tabla){
	memoria->espacio_disponible += tamanio_a_eliminar;
	tabla->tamanio_proceso -= tamanio_a_eliminar;
	int cant_paginas_a_eliminar = (tamanio_a_eliminar + tam_pagina -1)/tam_pagina;
	int i=0;
	int cantidad_paginas = list_size(tabla->paginas)-1;
	log_info(logger_memoria, "PID: %i - Tamaño Actual: %i - Tamaño a Reducir: %i",tabla->pid, cantidad_paginas, cant_paginas_a_eliminar*tam_pagina);
	while(i!= cant_paginas_a_eliminar){
		//log_warning(logger_memoria, "Elimine la pagina %i", (cantidad_paginas + i) - cant_paginas_a_eliminar);
		t_pagina * pagina =list_get(tabla->paginas,(cantidad_paginas - i));
		//log_error(logger_memoria,"la pagina que accedo es %i",(cantidad_paginas + i) - cant_paginas_a_eliminar);
		liberar_marco(pagina);
		//log_error(logger_memoria,"===================================");
		list_remove_element(tabla->paginas,pagina);
		free(pagina);
		i++;
	}
}
void liberar_marco(t_pagina * pagina){
	t_marco * marco = list_get(memoria->marcos,pagina->num_marco);
	marco->is_free= true;
	marco->pid=-1;

}

void enviar_respuesta_resize(int cliente_fd, int respuesta){
	t_paquete* paquete = crear_paquete(RESIZE_TAM);
	agregar_a_paquete(paquete,&(respuesta), sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}