#include "global.h"

void obtener_configuraciones() {
    puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    path_instrucciones =config_get_string_value(config,"PATH_INSTRUCCIONES");
    strcat(path_instrucciones,"/");
	auxiliar = config_get_int_value(config,"RETARDO_RESPUESTA");
    auxiliar = auxiliar* 1000;
    retardo_respuesta = (useconds_t) auxiliar;
}