#ifndef FINALIZA_PROCESO_H_
#define FINALIZA_PROCESO_H_
#include "global.h"

void finalizar_proceso(int);
t_tabla_paginas* tabla_paginas_segun_pid(int pid);
t_marco* marco_segun_pagina(t_pagina* pagina);


#endif 