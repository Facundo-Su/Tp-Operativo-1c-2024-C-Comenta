#include "manda_marco.h"

void mandar_pagina(int cliente) {
    t_list* lista = recibir_paquete(cliente);
	int* num_pag = list_get(lista,0);
    t_pagina* pagina_proceso = pagina_segun_numero(*num_pag);
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


void enviar_marco(int marco , op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &marco, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

