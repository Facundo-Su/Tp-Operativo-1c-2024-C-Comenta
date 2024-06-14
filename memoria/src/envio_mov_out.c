#include "envio_mov_out.h"

void envio_mov_out (int cliente_fd){       
    t_list* lista = recibir_paquete(cliente_fd);
	int *marco = list_get(lista,0);
	int *desplazamiento= list_get(lista,1);
	int *pagina = list_get(lista,2);
	int *pid = list_get(lista,3);
	uint32_t *valor_remplazar = list_get(lista,4);
	memcpy(memoria->espacio_usuario + (*marco * tam_pagina) + *desplazamiento, valor_remplazar, sizeof(uint32_t));
	//enviar_respuesta(cliente_fd, ENVIO_MOV_OUT);	//Aca no estoy seguro si es el codigo de operacion correcto
    int dir_mov = (*marco * tam_pagina) + *desplazamiento;

	int prueba;
	memcpy(prueba,memoria->espacio_usuario + (*marco * tam_pagina) + *desplazamiento, sizeof(uint32_t));
	log_warning(logger_memoria,"el valor que almacene es %i",prueba);
	//t_pagina* pagina_mov_out = obtener_pagina(*pid, *pagina);
	//pagina_mov_out->m=1;
	log_info(logger_memoria,"PID: %i- Accion: ESCRIBIR - Direccion fisica: %i- Tamaño:  %i",*pid,dir_mov, tam_pagina);
	
}


void enviar_respuesta(int cliente_fd,op_code operacion){
	t_paquete* paquete = crear_paquete(operacion);
	int var =1;
	agregar_a_paquete(paquete, &var, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}