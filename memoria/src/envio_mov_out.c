#include "envio_mov_out.h"

void envio_mov_out (int cliente_fd){       
    t_list* lista = recibir_paquete(cliente_fd);
	int *marco = list_get(lista,0);
	int *desplazamiento= list_get(lista,1);
	int *pagina = list_get(lista,2);
	int *pid = list_get(lista,3);
	uint32_t *valor_remplazar = list_get(lista,4);
    int *tamanio_a_escribir = list_get(lista, 5);
	memcpy(memoria->espacio_usuario + (*marco * tam_pagina) + *desplazamiento, valor_remplazar, sizeof(uint32_t));
	int dir_mov = *marco * tam_pagina;
	//t_pagina* pagina_mov_out = obtener_pagina(*pid, *pagina); !!
	//pagina_mov_out->m=1;
	log_info(logger,"PID: %i- Accion: ESCRIBIR - Direccion fisica: %i- Tamaño:  %i",*pid,dir_mov,*tamanio_a_escribir);
	//enviar_respuesta(cliente_fd,RESPUESTA_MOV_OUT); !!
}