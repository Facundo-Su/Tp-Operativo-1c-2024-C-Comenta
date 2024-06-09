#include "global.h"

t_memoria* memoria;
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
    memoria->espacio_usuario= malloc(tam_memoria);
	memoria->tamanio_marcos = tam_pagina;
	memoria->cantidad_marcos = tam_memoria/tam_pagina;
}

void inicializar_estructuras(){
    logger_memoria = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);
    logger = log_create("./memoria_principal.log", "CPU", true, LOG_LEVEL_INFO);
    memoria = malloc(sizeof(t_memoria));
    lista_instrucciones = list_create();
    memoria->lista_tabla_paginas = list_create();
    memoria->marcos = list_create();
    inicializar_marcos();
}

void inicializar_marcos() {
    int i, desplazamiento = 0;
    for(i=0; i<memoria->cantidad_marcos; i++) {
        t_marco *marco = malloc(sizeof(t_marco));
        marco->base = desplazamiento;
        marco->is_free = true;
        marco->num_marco = i;
        marco->pid = -1;
        list_add(memoria->marcos, marco);
        desplazamiento+= memoria->tamanio_marcos;
    }
}