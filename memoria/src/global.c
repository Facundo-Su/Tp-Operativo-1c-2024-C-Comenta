#include "global.h"

t_list *lista_instrucciones;
char *puerto_escucha;
int tam_memoria;
int tam_pagina;
char *path_instrucciones;
int retraso_respuesta;
useconds_t retardo_respuesta;
int auxiliar;
t_log* logger;
t_log* logger_memoria;
t_config* config;
void obtener_configuraciones() {
    config = cargar_config("./memoria.config");
    puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    path_instrucciones = config_get_string_value(config,"PATH_INSTRUCCIONES");
    strcat(path_instrucciones,"/");
    log_info(logger_memoria, "%s",path_instrucciones);
	auxiliar = config_get_int_value(config,"RETARDO_RESPUESTA");
    auxiliar = auxiliar* 1000;
    retardo_respuesta = (useconds_t) auxiliar;
}

void inicializar_estructuras(){
    logger_memoria = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);
    logger = log_create("./memoria_principal.log", "CPU", true, LOG_LEVEL_INFO);
    lista_instrucciones = list_create();
}