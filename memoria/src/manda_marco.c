#include "manda_marco.h"

void mandar_marco(int cliente) {
    t_list* lista = recibir_paquete(cliente);
    int* pid = list_get(lista,0);
	int* num_pag = list_get(lista,1);
    t_tabla_paginas* tabla = tabla_paginas_segun_pid(*pid);
    t_pagina* pagina_proceso = list_get(tabla->paginas,*num_pag);
    log_info(logger,"PID: %i- Pagina: %i- Marco: %i",*pid, *num_pag, pagina_proceso->num_marco);
    enviar_marco(pagina_proceso->num_marco, OBTENER_MARCO, cliente); 
}

void enviar_marco(int marco , op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &marco, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

