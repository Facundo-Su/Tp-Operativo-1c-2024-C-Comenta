#ifndef MANDA_MARCO_H_
#define MANDA_MARCO_H_
#include "global.h"

void mandar_pagina(int);
void mandar_marco(int cliente);
void enviar_marco(int,op_code,int);
t_pagina* pagina_segun_numero(int );
int pid_segun_num_de_pagina(int );
//t_tabla_paginas* tabla_paginas_segun_pid(int );


#endif 