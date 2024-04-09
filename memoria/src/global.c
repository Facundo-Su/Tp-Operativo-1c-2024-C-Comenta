#include "global.h"
t_log * logger;

t_list *lista_instrucciones;
char *puerto_escucha;
int tam_memoria;
int tam_pagina;
char *path_instrucciones;
int retraso_respuesta;
useconds_t retardo_respuesta;
int auxiliar;

t_log* logger_memoria;
t_config* config;
void obtener_configuraciones() {
    puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    path_instrucciones = config_get_string_value(config,"PATH_INSTRUCCIONES");
    strcat(path_instrucciones,"/");
    log_info(logger_memoria, "%s",path_instrucciones);
	auxiliar = config_get_int_value(config,"RETARDO_RESPUESTA");
    auxiliar = auxiliar* 1000;
    //retardo_respuesta = (useconds_t) auxiliar;
}

void cargar_lista_instruccion(char *ruta ,int pid) {
    t_instrucciones* instruccion = malloc(sizeof(t_instruccion));
    instruccion->pid = pid;
    instruccion->instrucciones = list_create();
    FILE* archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        //log_error(logger_consola_memoria, "El archivo %s no pudo ser abierto.", ruta);
        free(instruccion);  // Liberar la memoria asignada a instruccion
    } else {
        t_list* auxiliar = leer_pseudocodigo(archivo);
        list_add_all(instruccion->instrucciones, auxiliar);
        list_add(lista_instrucciones, instruccion);
        int cantidad = list_size(auxiliar);

        t_instrucciones* instruuu = list_get(lista_instrucciones, 0);
        int cantidad2 = list_size(lista_instrucciones);

        //log_info(logger_consola_memoria, "La lista total total de general es %i", cantidad2);
       // log_info(logger_consola_memoria, "El valor de la pid primero es %i", instruuu->pid);
        fclose(archivo);
    }
}

char* obtener_ruta(char* valorRecibido) {
    /*char* ruta = malloc(strlen(path_instrucciones) + strlen(valorRecibido) + 1); // +5 para ".txt" y el carácter nulo
    strcpy(ruta, path_instrucciones);
    strcat(ruta, valorRecibido);
    return ruta;*/
        if (path_instrucciones == NULL || valorRecibido == NULL) {
        log_info(logger_memoria, "Error: path_instrucciones o valorRecibido es NULL\n");
        return NULL;
    }

    size_t longitud_path = strlen(path_instrucciones);
    size_t longitud_valor = strlen(valorRecibido);
    size_t longitud_total = longitud_path + longitud_valor + 1; // +1 para el carácter nulo

    // Asignar memoria suficiente para la ruta (incluyendo el carácter nulo)
    char* ruta = malloc(longitud_total);
    if (ruta == NULL) {
        log_info(logger_memoria, "Error: No se pudo asignar memoria para la ruta\n");
        return NULL; // Manejar el error de asignación de memoria
    }

    // Copiar la parte inicial de la ruta (path_instrucciones)
    strcpy(ruta, path_instrucciones);

    // Concatenar el valor recibido (nombre de archivo) a la ruta
    strcat(ruta, valorRecibido);

    return ruta;
}
t_list* leer_pseudocodigo(FILE* pseudocodigo){
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;
    int cantidad_parametros;
    t_list* instrucciones_correspondiente_al_archivo = list_create();
	t_list * instrucciones_del_pcb = list_create();
	int j=0;
    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

    	//log_info(logger_consola_memoria,"el valor es %s" ,instruccion);
    	char* valor_remplazo = strdup(instruccion);
        list_add(instrucciones_del_pcb,valor_remplazo);
        char *instruc_aux_nose23 = list_get(instrucciones_del_pcb,j);
        j++;
       // log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose23);
    }

    char *instruc_aux_nose = list_get(instrucciones_del_pcb,0);
   // log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose);
    char *instruc_aux_nose2 = list_get(instrucciones_del_pcb,1);
    //log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose2);
    //char *instruc_aux_nose3 = list_get(instrucciones_del_pcb,2);
    //log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose3);
    return instrucciones_del_pcb;

}