#include "finaliza_proceso.h"

void finalizar_proceso(int pid) {

    t_tabla_paginas* tabla_de_paginas = tabla_paginas_segun_pid(pid);

    for (int j = 0; j <= tabla_de_paginas->paginas_necesarias; j++)
    {
        t_pagina* pagina = list_get(tabla_de_paginas->paginas, j);
		t_marco* marco = marco_segun_pagina(pagina);
        marco->is_free = true;
		marco->pid =-1;
		marco->llegada_fifo =0;
		marco->last_time_lru =0;
    }
}

t_tabla_paginas* tabla_paginas_segun_pid(int pid)
{
	t_tabla_paginas* aux;

    for (int i = 0; i < list_size(memoria->lista_tabla_paginas); i++)
    {
        aux = list_get(memoria->lista_tabla_paginas, i);
        if (pid == aux->pid)
            return aux;
    }

	log_error(logger_memoria, "PID - %d No se encontro la Tabla de Paginas", pid);
    abort();
}

t_marco* marco_segun_pagina(t_pagina* pagina) {

	t_marco* aux;

    for (int i = 0; i < list_size(memoria->marcos); i++)
    {
        aux = list_get(memoria->marcos, i);
        if (pagina->num_marco == aux->num_marco)
            return aux;
    }

	log_error(logger_memoria, "Pagina - %d No se encontro el marco", pagina->num_pagina);
    abort();
}