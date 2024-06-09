#ifndef ENVIO_RESIZE_H_
#define ENVIO_RESIZE_H_

#include "global.h"

void envio_resize(int);
t_tabla_paginas* tabla_paginas_segun_pid(int);
int tamanio_asignado(int);
t_pagina* siguiente_pagina_libre(t_tabla_paginas*);
void asignar_marco(int, t_pagina*, int*);
int encontrar_marco_libre();
void enviar_out_of_memory(op_code, int);
t_pagina* ultima_pagina_asignada(t_tabla_paginas*);
void liberar_pagina_y_marco(t_pagina*);

#endif 