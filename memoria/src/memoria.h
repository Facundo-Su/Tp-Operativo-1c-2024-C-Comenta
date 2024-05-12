#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <utils/hello.h>
#include "global.h"
#include "crear_proceso.h"
#include "finaliza_proceso.h"




void iniciar_servidor_memoria(char *puerto) ;
void iterator(char* value);
void procesar_conexion(void* socket);
void enviar_tam_pagina(int tam , int cliente_fd);

#endif