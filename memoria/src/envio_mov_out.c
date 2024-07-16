#include "envio_mov_out.h"

void envio_mov_out (int cliente_fd){       
    t_list* lista = recibir_paquete(cliente_fd);
	int *marco = list_get(lista,0);
	int *desplazamiento= list_get(lista,1);
	int *pagina = list_get(lista,2);
	int *pid = list_get(lista,3);
	uint32_t *valor_remplazar = list_get(lista,4);
	int tamanio;
	if(*valor_remplazar <255){
		tamanio =1;
	}else{
		tamanio =4;
	}
	memcpy(memoria->espacio_usuario + (*marco * tam_pagina) + *desplazamiento, valor_remplazar, tamanio);
    int dir_mov = (*marco * tam_pagina) + *desplazamiento;
	int prueba;
	memcpy(&prueba,memoria->espacio_usuario + (*marco * tam_pagina) + *desplazamiento, tamanio);
	log_error(logger_memoria, "valor leido es %i",prueba);
	void *valor_leido = malloc(36);
    memcpy(valor_leido, memoria->espacio_usuario+ (*marco * tam_pagina) + *desplazamiento, 36);
	log_info(logger_memoria,"PID: %i- Accion: ESCRIBIR - Direccion fisica: %i",*pid,dir_mov);
	
}


void enviar_respuesta(int cliente_fd,op_code operacion){
	t_paquete* paquete = crear_paquete(operacion);
	int var =1;
	agregar_a_paquete(paquete, &var, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}