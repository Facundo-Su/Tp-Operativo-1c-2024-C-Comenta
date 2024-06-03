#include "envio_mov_in.h"

void envio_mov_in (int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int *marco = list_get(lista,0);
	int *desplazamiento = list_get(lista,1);
	int *nro_pag_mov_in = list_get(lista,2);
	t_marco *marco_obtenido = list_get(memoria->marcos, *marco);
	//asignar_marco(pid, *nro_pag_mov_in);
	uint32_t *valor_leido = malloc(sizeof(uint32_t));
	memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, sizeof(uint32_t));
	enviar_registro_leido(*valor_leido,ENVIO_MOV_IN,cliente_fd);
	int dir_mov_in = *marco *tam_pagina;
	log_info(logger,"PID: %i- Accion: LEER - Direccion fisica: %i",marco_obtenido->pid,dir_mov_in);
	//enviar_respuesta(cliente_fd,RESPUESTA_MOV_IN);
}

void enviar_registro_leido(int valor_encontrado, op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}