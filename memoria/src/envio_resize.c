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

/*void envio_resize (int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int* pid = list_get(lista,0);
	int* tamanio = list_get(lista,1);
	int cantidad_de_marcos = *tamanio/memoria->tamanio_marcos;
	t_tabla_paginas* tabla = tabla_paginas_segun_pid(*pid);

	int memoria_llena = 0;
	log_warning(logger_memoria,"el pid de la tabla es - %d", tabla->pid);

	int restante = cantidad_de_marcos - list_size(tabla->paginas);

	int marco_disponible_restante  = marco_disponible_restante_funcion();
	
	if(tamanio_asignado(*pid) < *tamanio) { 	//Ampliacion de proceso

		int cantidad_de_marcos_agrandar =cantidad_de_marcos - list_size(tabla->paginas);
		if(marco_disponible_restante < restante) {
			enviar_out_of_memory(cliente_fd);
			return;
		}

		if(list_size(tabla->paginas) == 0) {
			log_error(logger_memoria,"pase por aca");
			t_pagina* pagina = malloc(sizeof(t_pagina));
			pagina->num_pagina = 0;
			pagina->num_marco = -1;
			pagina->p = 0;


			for(int i=0;i<list_size(memoria->marcos);i++) {
				t_marco* aux = list_get(memoria->marcos, i);
				if(aux->is_free) {
					t_marco * marco = list_get(memoria->marcos, i);
					marco = list_get(memoria->marcos,i);
					marco->is_free = false; 
					marco->pid = pid;
					pagina->num_marco = i;
					pagina->p=1;
					log_warning(logger_memoria,"asigne el marco %d",marco->num_marco);
					list_add(tabla->paginas,pagina);
					break;
				}
			}
		}

		for (int i = list_size(tabla->paginas); i < cantidad_de_marcos; i++){
			t_pagina* aux2 = malloc(sizeof(t_pagina));
			aux2->num_pagina = i;
			aux2->num_marco = -1;
			aux2->p = 0;
			list_add(tabla->paginas, aux2);

			t_pagina* pagina_aux_anterior = list_get(tabla->paginas, i-1);
			int ultimo_marco_usado_por_el_pid = pagina_aux_anterior->num_marco;

			t_pagina* aux = list_get(tabla->paginas, i);
			asignar_marco(*pid,aux,ultimo_marco_usado_por_el_pid);

		}
	
			enviar_ok(cliente_fd);

	} else { //Reduccion de proceso
		int pagina_a_borrar = list_size(tabla->paginas) - cantidad_de_marcos;

		for(int i=0;i<pagina_a_borrar;i++) {
			int ultimo_elemento = list_size(tabla->paginas);
			t_pagina* pagina = list_remove(tabla->paginas,ultimo_elemento);
			eliminar_el_marco(pagina);
			
		}
	}
}

void eliminar_el_marco(t_pagina* pagina) {
	t_marco* aux = NULL;   

    int nro_marco = pagina->num_marco;
    aux=list_get(memoria->marcos, nro_marco);
    aux->is_free = true;
    aux->pid = -1;
}

void enviar_out_of_memory(cliente_fd){
	t_paquete* paquete = crear_paquete(OUT_OF_MEMORY);
	int numero =-1;
	agregar_a_paquete(paquete,&(numero), sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

int marco_disponible_restante_funcion(){
	int contador=0;
	for(int i=0;i<list_size(memoria->marcos);i++) {
		t_marco* aux = list_get(memoria->marcos, i);
		if(aux->is_free) {
			contador++;
		}
	}
	return contador;
}

void agregar_pagina_a_tabla(int i) {
	t_pagina* aux = malloc(sizeof(t_pagina));
	aux->num_pagina = i;
	aux->num_marco = -1;
	aux->p = 0;
	list_add(memoria->marcos, aux);
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

void asignar_marco(int pid, t_pagina * pagina, int ultimo_marco){ //TODO 

	int i = encontrar_marco_libre(ultimo_marco);
	t_marco * marco = list_get(memoria->marcos, i);
	marco = list_get(memoria->marcos,i);
	marco->is_free = false; 
	marco->pid = pid;
	log_warning(logger_memoria,"asigne el marco %d",marco->num_marco);
	pagina->num_marco = i;
	pagina->p=1;
}

int encontrar_marco_libre(int ultimo_marco) {
    int i;
	int marco_no_tuvido_encuenta= list_size(memoria->marcos) -ultimo_marco;

	t_marco* marco;
	for(i=0;i<memoria->cantidad_marcos;i++) {

		if(ultimo_marco == list_size(memoria->marcos)) {
			ultimo_marco=0;
		}

    	marco = list_get(memoria->marcos, ultimo_marco);
		if(marco->is_free) {
			log_error(logger_memoria, "el valor de marco que encontre libre es - %d", i);
            return ultimo_marco;
		}
		ultimo_marco++;
	}
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
}*/

void envio_resize(int cliente_fd){
	t_list* lista = recibir_paquete(cliente_fd);
	int* pid = list_get(lista,0);
	int* tamanio = list_get(lista,1);
	t_tabla_paginas *tabla = tabla_paginas_segun_pid(*pid);
	int tamanio_proceso = tabla->tamanio_proceso;
	//log_error(logger_memoria, "TAMAÑIO proceso %i", tamanio_proceso);
	//log_error(logger_memoria, "TAMAÑIO memoria %i", memoria->espacio_disponible);
	log_warning(logger_memoria, "PID - %i TAMAÑIO - %i", *pid, *tamanio);
	
	if(*tamanio<tamanio_proceso){
		int tamanio_a_eliminar = tamanio_proceso-*tamanio;
		ejecutar_reduccion(tamanio_a_eliminar, tabla);
		enviar_respuesta_resize (cliente_fd,0);
	}
	else{
		int tamanio_a_agregar = *tamanio - tamanio_proceso;
		log_error(logger_memoria, "TAMAÑIO a agregar %i", tamanio_a_agregar);
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
	log_error(logger_memoria,"tamanio de proceso  agrandar es %i", tamanio_a_agregar);
	log_error(logger_memoria,"tamanio de proceso  actual es %i", tabla->tamanio_proceso);
	int cant_paginas_nuevas = (tamanio_a_agregar + tam_pagina -1)/tam_pagina;
	int cant_paginas = list_size(tabla->paginas);
	log_info(logger_memoria, "PID: %i - Tamaño Actual: %i - Tamaño a Ampliar: %i",tabla->pid, cant_paginas, tamanio_a_agregar);
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
	log_info(logger_memoria, "PID: %i - Tamaño Actual: %i - Tamaño a Reducir: %i",tabla->pid, cantidad_paginas, cant_paginas_a_eliminar);
	while(i!= cant_paginas_a_eliminar){
		log_warning(logger_memoria, "Elimine la pagina %i", (cantidad_paginas + i) - cant_paginas_a_eliminar);
		t_pagina * pagina =list_get(tabla->paginas,(cantidad_paginas - i));
		log_error(logger_memoria,"la pagina que accedo es %i",(cantidad_paginas + i) - cant_paginas_a_eliminar);
		liberar_marco(pagina);
		log_error(logger_memoria,"===================================");
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