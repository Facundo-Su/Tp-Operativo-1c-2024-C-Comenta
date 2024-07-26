#include "stdin_read.h"

void stdin_read(int cliente_fd, op_code codigo_respuesta){ 
    t_list* lista = recibir_paquete(cliente_fd);
    int *pid = list_get(lista, 0);
    int *marco = list_get(lista,1);
	int *desplazamiento = list_get(lista,2);
    int *tamanio = list_get(lista,3);
	void *valor_a_copiar = valor_a_copiar= list_get(lista,4);
    //log_info(logger, "me llego para fread pid %i, marco %i, desplazamiento %i, tamanio %i, palabra %s",*pid, *marco,*desplazamiento, *tamanio,valor_a_copiar);
    memcpy(memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, valor_a_copiar, *tamanio);
    void * m = malloc(*tamanio);
    memcpy(m,memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, *tamanio);
    log_info(logger,"el valor leido es %s",m);
    enviar_respuesta_IO(1, codigo_respuesta,cliente_fd);
    int dir = (*marco * tam_pagina) + *desplazamiento;
    log_info(logger,"PID: %i- Accion: ESCRIBIR - Direccion fisica: %i- Tamaño:  %i",*pid, dir, *tamanio);
}


void enviar_respuesta_IO(int valor_encontrado, op_code operacion, int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}
 