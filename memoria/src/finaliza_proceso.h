#ifndef FINALIZA_PROCESO_H_
#define FINALIZA_PROCESO_H_
#include "global.h"

void finalizar_proceso(int);
void liberar_marcos(int, t_tabla_paginas*);
t_marco* marco_segun_pagina(t_pagina* pagina);


#endif 