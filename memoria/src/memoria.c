#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

#include "memoria.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "./memoria.config";
    logger = log_create("memoria2.log", "Memoria", 1, LOG_LEVEL_DEBUG);

	config = cargar_config(rutaConfig);

    obtener_configuraciones();

    //iniciar_recursos();
    //inicializar_memoria();

    logger_memoria = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

    log_info(logger_memoria, "Soy el Memoria!");

    //iniciar_consola();
	iniciar_servidor_memoria(puerto_escucha);
    // Limpieza y terminación

    return EXIT_SUCCESS;
}



void obtener_configuraciones() {
    puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    path_instrucciones =config_get_string_value(config,"PATH_INSTRUCCIONES");
    tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    strcat(path_instrucciones,"/");
    auxiliar = auxiliar* 1000;
    retardo_respuesta = (useconds_t) auxiliar;
}


void iniciar_servidor_memoria(char *puerto) {

    int memoria_fd = iniciar_servidor(puerto);
    log_info(logger_memoria, "Servidor listo para recibir al cliente");

    while (1) {
        int cliente_fd = esperar_cliente(memoria_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		pthread_detach(atendiendo);

    }
}


void procesar_conexion(void* socket){
	int *conexion = (int*)socket;
	int cliente_fd = *conexion;
	while (1) {
	            int cod_op = recibir_operacion(cliente_fd);
	            t_list * lista;
	            usleep(retardo_respuesta);

	            switch (cod_op) {
	            case MENSAJE:
	                recibir_mensaje(cliente_fd);
	                log_info(logger_memoria,"hola como estas capo");
					enviar_mensaje_instrucciones("recibi el mensaje", cliente_fd, MENSAJE);
	                break;
	            case PAQUETE:
	                lista = recibir_paquete(cliente_fd);
	                log_info(logger_memoria, "Me llegaron los siguientes valores:\n");
	                list_iterate(lista, (void*) iterator);
	                break;

	            case -1:
	                log_error(logger_memoria, "El cliente se desconectó. Terminando servidor");
	                close(cliente_fd);
	                return; // Salir del bucle interno para esperar un nuevo cliente
	            default:
	                log_warning(logger_memoria, "Operación desconocida. No quieras meter la pata");
	                break;
	            }
	        }
}

void iterator(char* value) {
    log_info(logger_memoria, "%s", value);
}