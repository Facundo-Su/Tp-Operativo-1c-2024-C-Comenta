#ifndef ENVIO_RESIZE_H_
#define ENVIO_RESIZE_H_

#include "global.h"

void envio_resize(int);
//t_tabla_paginas* tabla_paginas_segun_pid(int);
//nt tamanio_asignado(int);
//t_pagina* siguiente_pagina_libre(t_tabla_paginas*);
int encontrar_marco_libre();
void enviar_respuesta_resize(int , int );
//t_pagina* ultima_pagina_asignada(t_tabla_paginas*);
//void liberar_pagina_y_marco(t_pagina*);
//void asignar_marco(int pid, t_pagina * pagina, int ultimo_marco);
void asignar_marco(int , t_pagina *);
//int obtener_marco_disponible_ultimo(ultimo_marco_usado_por_el_pid);
#endif W