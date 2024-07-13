#include "stdout_write.h"


void stdout_write(int cliente_fd,op_code codigo_respuesta){ 
	
    t_list* lista = recibir_paquete(cliente_fd);
	int *pid = list_get(lista, 0);
    int *marco = list_get(lista,1);
	int *desplazamiento = list_get(lista,2);
    int *tamanio = list_get(lista,3);
	void *valor_leido = malloc(*tamanio);
	log_error(logger," el pid es %i",*pid);
	log_error(logger," el marco es %i",*marco);
	log_error(logger," el desplazamiento es %i",*desplazamiento);
	log_error(logger," el tamanio es %i",*tamanio);
	log_info(logger, "me llego para fwrite pid %i, marco %i, desplazamiento %i, tamanio %i",*pid, *marco,*desplazamiento, *tamanio);
	log_error(logger, "me llego para fwrite");
    memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, *tamanio);
	log_error(logger, "me llego para fwrite");
	enviar_registro_leido_IO(valor_leido, codigo_respuesta,*tamanio,cliente_fd);
	log_error(logger, "me llego para fwrite");
	int dir = (*marco * tam_pagina) + *desplazamiento;
	log_info(logger,"PID: %i- Accion: LEER - Direccion fisica: %i- Tamaño:  %i",*pid, dir, *tamanio);
	//log_info(logger,"EL DATO ES %s",valor_leido);
	free(valor_leido);
}

void enviar_registro_leido_IO(void* valor_encontrado, op_code operacion,int tamanio, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	//log_info(logger,"EL DATO ES %s",valor_encontrado);
	agregar_a_paquete(paquete, valor_encontrado, tamanio);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}