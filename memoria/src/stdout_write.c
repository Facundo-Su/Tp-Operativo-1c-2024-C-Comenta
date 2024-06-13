#include "stdout_write.h"


void stdout_write(int cliente_fd){ 
    t_list* lista = recibir_paquete(cliente_fd);
    int *marco = list_get(lista,0);
	int *desplazamiento = list_get(lista,1);
	int *pid = list_get(lista, 2);
    int *tamanio = list_get(lista,3);
    uint32_t *valor_leido = malloc(sizeof(uint32_t));
    memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, *tamanio);
    enviar_registro_leido_IO(*valor_leido, RESPUESTA_STDOUT_WRITE,cliente_fd);
	int dir = (*marco * tam_pagina) + *desplazamiento;
	log_info(logger,"PID: %i- Accion: LEER - Direccion fisica: %i- Tamaño:  %i",*pid, dir, *tamanio);
	free(valor_leido);
}

void enviar_registro_leido_IO(int valor_encontrado, op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}