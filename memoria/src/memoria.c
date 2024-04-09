#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

#include "memoria.h"

int main(int argc, char* argv[]) {

	//Se declara e inicializa el logger de memoria
    logger_memoria = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);
	logger = log_create("./memoria_principal.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger_memoria, "Soy la Memoria!");
	lista_instrucciones = list_create();
	//Se levanta el archivo de configuracion y se guardan sus datos
	char *rutaConfig = "./memoria.config";
	config = cargar_config(rutaConfig);
    obtener_configuraciones();

    //iniciar_recursos();
    //inicializar_memoria();
    //iniciar_consola();
	iniciar_servidor_memoria(puerto_escucha);

    return EXIT_SUCCESS;
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
				case CREAR_PCB:
					lista = recibir_paquete(cliente_fd);
					//t_contexto_ejecucion* contexto = desempaquetar_pcb(lista);
					char * aux =list_get(lista,0);
					int* pid = list_get(lista,1);
					char *ruta = obtener_ruta(aux);
					log_info(logger_memoria,"%s", aux);
					//mostrar_contexto_ejecucion(contexto);
					cargar_lista_instruccion(ruta,*pid);
					break;
	    		case INSTRUCCIONES_A_MEMORIA:
					log_info(logger_memoria, "LLegue hasta aca");
	    			//usleep(retardo_respuesta);
	    			lista = recibir_paquete(cliente_fd);
	    			int* pc_recibido = list_get(lista,0);
	    			int* pid_recibido = list_get(lista,1);
//	    			log_info(logger_consola_memoria,"me llegaron el siguiente pc %i",*pc_recibido);
//	    			log_info(logger_consola_memoria,"me llegaron el siguiente pid %i",*pid_recibido);
	    		    bool encontrar_instrucciones(void * instruccion){
	    		          t_instrucciones* un_instruccion = (t_instrucciones*)instruccion;
	    		          int valor_comparar =un_instruccion->pid;
	    		          return valor_comparar == *pid_recibido;
	    		    }
	    		    t_instrucciones* instrucciones = list_find(lista_instrucciones, encontrar_instrucciones);

	    		    if (instrucciones != NULL) {
	    		        // Se encontró un elemento que cumple con la condición
//	    		        log_info(logger, "Se encontraron instrucciones para el PID %i", *pid_recibido);
//	    		        log_info(logger, "Cantidad de instrucciones: %i", list_size(instrucciones->instrucciones));
	    		        char*valor_obtenido = list_get(instrucciones->instrucciones,*pc_recibido);
//	    		        log_info(logger, "el instruccion que se envio es  %s", valor_obtenido);
		    			enviar_mensaje_instrucciones(valor_obtenido,cliente_fd,INSTRUCCIONES_A_MEMORIA);
	    		        // Luego puedes realizar las operaciones que necesites con 'instrucciones'
	    		    } else {
	    		        // No se encontraron instrucciones que cumplan con la condición
							log_info(logger_memoria, "No se encontraron instrucciones para el PID %i", *pid_recibido);
	    		    }
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
