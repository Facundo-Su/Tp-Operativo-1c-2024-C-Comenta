#include "envio_resize.h"

void envio_resize (int cliente_fd){
    t_list* lista = recibir_paquete(cliente_fd);
	int* pid = list_get(lista,0);
	int* tamanio = list_get(lista,1);
}