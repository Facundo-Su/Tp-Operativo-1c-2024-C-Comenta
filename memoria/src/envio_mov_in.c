#include "envio_mov_in.h"

void envio_mov_in (int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int *marco = list_get(lista,0);
	int *desplazamiento = list_get(lista,1);
	t_marco *marco_obtenido = list_get(memoria->marcos, *marco);
	uint8_t *valor_leido = malloc(sizeof(uint8_t));
	memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, sizeof(uint8_t));

	enviar_registro_leido(*valor_leido,ENVIO_MOV_IN,cliente_fd); 
	log_error(logger, "valor leido es %i",*valor_leido);	
	int dir = (*marco * tam_pagina) + *desplazamiento;
	log_info(logger,"PID: %i- Accion: LEER - Direccion fisica: %i- Tamaño:  %i",marco_obtenido->pid, dir, tam_pagina); 
	free(valor_leido);
}

void enviar_registro_leido(int valor_encontrado, op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}