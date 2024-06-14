#include "manda_marco.h"

void mandar_marco(int cliente) {
    t_list* lista = recibir_paquete(cliente);
    int* pid = list_get(lista,0);
	int* num_pag = list_get(lista,1);
    t_tabla_paginas* tabla = tabla_paginas_segun_pid(*pid);


    t_pagina* pagina_proceso = pagina_segun_numero(*num_pag);
    int pid = pid_segun_num_de_pagina(*num_pag);
    log_info(logger,"PID: %i- Pagina: %i- Marco: %i",pid, *num_pag, pagina_proceso->num_marco);
    enviar_marco(pagina_proceso->num_marco, MANDAME_PAGINA, cliente); //No estoy seguro aca del segundo parametro
}

t_pagina* pagina_segun_numero(int num) {

	t_tabla_paginas* aux;
    t_pagina* aux1;

    for (int i = 0; i <=list_size(memoria->lista_tabla_paginas); i++)
    {
        aux = list_get(memoria->lista_tabla_paginas, i);
        for (int j = 0; j <=list_size(aux->paginas); j++)
        {
        aux1 = list_get(aux->paginas, j);
        if (aux1->num_pagina == num)
            return aux1;
         }
    }
    abort();
}

int pid_segun_num_de_pagina(int num) {

	t_tabla_paginas* aux;
    t_pagina* aux1;

    for (int i = 0; i <=list_size(memoria->lista_tabla_paginas); i++)
    {
        aux = list_get(memoria->lista_tabla_paginas, i);
        for (int j = 0; j <=list_size(aux->paginas); j++)
        {
        aux1 = list_get(aux->paginas, j);
        if (aux1->num_pagina == num)
            return aux->pid;
         }
    }
    abort();
}

t_tabla_paginas* tabla_paginas_segun_pid(int pid)
{
	t_tabla_paginas* aux = NULL;
    for (int i = 0; i < list_size(memoria->lista_tabla_paginas); i++)
    {
        log_error(logger_memoria, " PID de tabla paginas segun pid - %d", pid);
        aux = list_get(memoria->lista_tabla_paginas, i);
        if (pid == aux->pid)
            return aux;
    }

	log_error(logger_memoria, "PID - %d No se encontro la Tabla de Paginas", pid);
    return NULL;
}

void enviar_marco(int marco , op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &marco, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

