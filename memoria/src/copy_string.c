#include "copy_string.h"

void copy_string(int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int *pid = list_get(lista,0);
	int *tamanio = list_get(lista,1);
	int *marco_origen = list_get(lista, 2);
    int *desplazamiento_origen = list_get(lista, 3);
    int *marco_destino = list_get(lista, 4);
    int *desplazamiento_destino = list_get(lista, 5);
	void* valor_a_copiar = malloc(sizeof(*tamanio));
    memcpy(valor_a_copiar, memoria->espacio_usuario + (*marco_origen *tam_pagina) + *desplazamiento_origen, *tamanio);
    memcpy(memoria->espacio_usuario + (*marco_destino *tam_pagina) + *desplazamiento_destino, valor_a_copiar, *tamanio);
    log_info(logger,"PID: %i- Marco origen: %i Marco destino %i - Valor a copiar: %s",*pid, *marco_origen, *marco_destino, valor_a_copiar);
    free(valor_a_copiar);
}