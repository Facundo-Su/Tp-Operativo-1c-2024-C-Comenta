#include "manda_marco.h"

void mandar_marco(int cliente) {
   
    t_list* lista = recibir_paquete(cliente);
    int* pid = list_get(lista,0);
	int* num_pag = list_get(lista,1);
    log_warning(logger, "mando marco++++++++++++++++");
    log_info(logger,"PID: %i- Pagina: %i",*pid, *num_pag);

    t_tabla_paginas* tabla = tabla_paginas_segun_pid(*pid);

    if(tabla == NULL){
        log_error(logger, "no se encontro la tabla");
    }else{
        log_warning(logger, "mando marco++++++++++++++++");
        log_info(logger,"marco de la tabla es %i",list_size(tabla->paginas));
    }

    t_pagina* pagina_proceso = list_get(tabla->paginas,*num_pag);
    log_warning(logger, "mando marco++++++++++++++++");
    log_warning(logger, "el marco es %i",pagina_proceso->num_marco);

    log_info(logger,"PID: %i- Pagina: %i- Marco: %i",*pid, *num_pag, pagina_proceso->num_marco);

    t_paquete* paquete = crear_paquete(OBTENER_MARCO);

    log_warning(logger, "mando marco");
    agregar_a_paquete(paquete, &(pagina_proceso->num_marco), sizeof(int));
    log_warning(logger, "mando marco");
	enviar_paquete(paquete, cliente);
    log_warning(logger, "mando marco");
	eliminar_paquete(paquete);
    //enviar_marco(pagina_proceso->num_marco, OBTENER_MARCO, cliente); 
}

void enviar_marco(int marco , op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
        log_warning(logger, "mando marco");
	agregar_a_paquete(paquete, &marco, sizeof(int));
        log_warning(logger, "mando marco");
	enviar_paquete(paquete, cliente_fd);
    log_warning(logger, "mando marco");
	eliminar_paquete(paquete);
}

