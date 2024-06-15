#include "stdout_write.h"


void stdout_write(int cliente_fd){ 
    t_list* lista = recibir_paquete(cliente_fd);
    int *marco = list_get(lista,0);
	int *desplazamiento = list_get(lista,1);
	int *pid = list_get(lista, 2);
    int *tamanio = list_get(lista,3);
	void *valor_leido = malloc(*tamanio);
    memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, *tamanio);
	enviar_registro_leido_IO(valor_leido, RESPUESTA_STDOUT_WRITE,*tamanio,cliente_fd);
	int dir = (*marco * tam_pagina) + *desplazamiento;
	log_info(logger,"PID: %i- Accion: LEER - Direccion fisica: %i- Tamaño:  %i",*pid, dir, *tamanio);
	free(valor_leido);
}

void enviar_registro_leido_IO(void* valor_encontrado, op_code operacion,int tamanio, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(tamanio));
	log_error(logger,"envie el info con la conexion %i", cliente_fd);
	enviar_paquete(paquete, cliente_fd);
	log_error(logger,"envie el info con la conexion %i", cliente_fd);
	eliminar_paquete(paquete);
}