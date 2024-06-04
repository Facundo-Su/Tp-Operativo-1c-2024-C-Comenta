#include "envio_mov_in.h"

//Ante un pedido de lectura, devolver el valor que se encuentra a partir de la dirección física pedida.
//TODO: Es importante tener en cuenta que las peticiones pueden ocupar más de una página!!!


void envio_mov_in (int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int *marco = list_get(lista,0);
	int *desplazamiento = list_get(lista,1);
	int *tamanio_a_leer = list_get(lista, 2);
	t_marco *marco_obtenido = list_get(memoria->marcos, *marco);
	uint32_t *valor_leido = malloc(sizeof(uint32_t));
	for (int i = 0; i <= *tamanio_a_leer; i++) {
	memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, sizeof(uint32_t));
	enviar_registro_leido(*valor_leido,ENVIO_MOV_IN,cliente_fd);
	}
	int dir= *marco * tam_pagina;
	log_info(logger,"PID: %i- Accion: LEER - Direccion fisica: %i- Tamaño:  %i",marco_obtenido->pid, dir, *tamanio_a_leer); //Aca se podria preguntar que direccion fisica me pide realmente el logger
}

/*
t_pagina * pagina_segun_pid(int pid,int nro_pagina){
	t_tabla_paginas* aux;
	t_pagina* aux1;

    for (int i = 0; i < list_size(memoria->lista_tabla_paginas); i++)
    {
        aux = list_get(memoria->lista_tabla_paginas, i);
        if (pid == aux->pid) {
			for (int j=0; j < list_size(aux->paginas); j++) {
				aux1 = list_get(aux->paginas, j);
				if (nro_pagina == aux1->num_pagina) {
					return aux1;
				}
			}
		}
            
    }

	log_error(logger_memoria, "PID - %d No se encontro la Tabla de Paginas", pid);
    abort();
}
*/

void enviar_registro_leido(int valor_encontrado, op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}
