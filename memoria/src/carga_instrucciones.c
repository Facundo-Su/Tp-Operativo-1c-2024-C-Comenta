#include "carga_instrucciones.h"

void cargar_lista_instruccion(int cliente_fd) {
	t_list * lista = recibir_paquete(cliente_fd);
	char * aux =list_get(lista,0);
	int* pid = list_get(lista,1);
	char *ruta = obtener_ruta(aux);

    t_instrucciones* instruccion = malloc(sizeof(t_instruccion));
    instruccion->pid = *pid;
    instruccion->instrucciones = list_create();
    FILE* archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        //log_error(logger_consola_memoria, "El archivo %s no pudo ser abierto.", ruta);
        free(instruccion);  // Liberar la memoria asignada a instruccion
    } else {
        t_list* auxiliar = leer_pseudocodigo(archivo);
        list_add_all(instruccion->instrucciones, auxiliar);
        list_add(lista_instrucciones, instruccion);
        //log_info(logger_consola_memoria, "La lista total total de general es %i", cantidad2);
       // log_info(logger_consola_memoria, "El valor de la pid primero es %i", instruuu->pid);
        fclose(archivo);
    }
}

char* obtener_ruta(char* valor_recibido) {
    /*char* ruta = malloc(strlen(path_instrucciones) + strlen(valorRecibido) + 1); // +5 para ".txt" y el carácter nulo
    strcpy(ruta, path_instrucciones);
    strcat(ruta, valorRecibido);
    return ruta;*/
    if (path_instrucciones == NULL || valor_recibido == NULL) {
        log_info(logger_memoria, "Error: path_instrucciones o valorRecibido es NULL\n");
        return NULL;
    }

    size_t longitud_path = strlen(path_instrucciones);
    size_t longitud_valor = strlen(valor_recibido);
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
    strcat(ruta, valor_recibido);

    return ruta;
}
t_list* leer_pseudocodigo(FILE* pseudocodigo){
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;
    int cantidad_parametros;
	t_list * instrucciones_del_pcb = list_create();
	int j=0;
    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

    	//log_info(logger_consola_memoria,"el valor es %s" ,instruccion);
    	char* valor_remplazo = strdup(instruccion);
        list_add(instrucciones_del_pcb,valor_remplazo);
        char *instruc_aux_nose23 = list_get(instrucciones_del_pcb,j);
        j++;
       log_info(logger_memoria,"la instruccion es  %s",instruc_aux_nose23);
    }

    //char *instruc_aux_nose = list_get(instrucciones_del_pcb,0);
   //log_info(logger_memoria,"el instruccion es  %s",instruc_aux_nose);
   // char *instruc_aux_nose2 = list_get(instrucciones_del_pcb,1);
    //log_info(logger_memoria,"el instruccion es  %s",instruc_aux_nose2);
    //char *instruc_aux_nose3 = list_get(instrucciones_del_pcb,2);
    //log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose3);
    return instrucciones_del_pcb;

}
void enviar_instrucciones(int cliente_fd){
    t_list * lista = recibir_paquete(cliente_fd);
    int* pc_recibido = list_get(lista,0);
    int* pid_recibido = list_get(lista,1);
    bool encontrar_instrucciones(void * instruccion){
        t_instrucciones* un_instruccion = (t_instrucciones*)instruccion;
            int valor_comparar =un_instruccion->pid;
            return valor_comparar == *pid_recibido;
    }
    t_instrucciones* instrucciones = list_find(lista_instrucciones, encontrar_instrucciones);

    if (instrucciones != NULL) {
        char*valor_obtenido = list_get(instrucciones->instrucciones,*pc_recibido);
        enviar_mensaje_instrucciones(valor_obtenido,cliente_fd,INSTRUCCIONES_A_MEMORIA);
    } else {
        log_info(logger_memoria, "No se encontraron instrucciones para el PID %i", *pid_recibido);
    }
}

