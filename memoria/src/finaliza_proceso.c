#include "finaliza_proceso.h"

void finalizar_proceso(int pid) {
    t_tabla_paginas* tabla_de_paginas = tabla_paginas_segun_pid(pid);
    liberar_marcos(pid, tabla_de_paginas);
    list_remove_element(memoria->lista_tabla_paginas, tabla_de_paginas);
	list_destroy_and_destroy_elements(tabla_de_paginas->paginas, free);
	free(tabla_de_paginas);
}

void liberar_marcos(int pid, t_tabla_paginas* tabla) {
    log_info(logger,"PID: %i- Tamaño: %i", pid, tabla->paginas_necesarias);

    for (int j = 0; j <= tabla->paginas_necesarias; j++)
    {
        t_pagina* pagina = list_get(tabla->paginas, j);
		t_marco* marco = marco_segun_pagina(pagina);
        if (marco != NULL) {
            marco->is_free = true;
            marco->pid = -1;
    }
    }
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

t_marco* marco_segun_pagina(t_pagina* pagina) {
	t_marco* aux = NULL;

    for (int i = 0; i < list_size(memoria->marcos); i++){
        aux = list_get(memoria->marcos, i);
        if (pagina->num_marco == aux->num_marco)
            return aux;
    }

	log_error(logger_memoria, "Pagina - %d No se encontro el marco", pagina->num_pagina);
    return NULL;
}