#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

#include "memoria.h"

int main(int argc, char* argv[]) {
	obtener_configuraciones();
	inicializar_estructuras();
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
				case CREAR_PROCESO:
					crear_proceso(cliente_fd);
					break;
	    		case INSTRUCCIONES_A_MEMORIA:
	    			usleep(retardo_respuesta);
					enviar_instrucciones(cliente_fd);
					break;
				case FINALIZAR:
					t_list* paquete = recibir_paquete(cliente_fd);
					t_contexto_ejecucion* pcb = desempaquetar_pcb(paquete);
					finalizar_proceso(pcb->pid);
					break;
				case OBTENER_MARCO:
	    			mandar_marco(cliente_fd);
	            	break;
				case MANDAME_PAGINA:
					recibir_mensaje(cliente_fd);
					enviar_tam_pagina(tam_pagina, cliente_fd);
					break;
				case ENVIO_RESIZE:
					envio_resize(cliente_fd);
					break;
				case ENVIO_MOV_IN:
					envio_mov_in(cliente_fd);
					break;
				case ENVIO_MOV_OUT:
					void *valor_leido2 = malloc(20);
    				memcpy(valor_leido2, memoria->espacio_usuario+16, 20);
					log_warning(logger,"EL DATO ante de ejecutar ES %s",valor_leido2);
					envio_mov_out(cliente_fd);
					void *valor_leido3 = malloc(20);
    				memcpy(valor_leido3, memoria->espacio_usuario+16, 20);
					log_error(logger,"EL DATO despues de ejecutar ES %s",valor_leido3);
					break;
				case COPY_STRING_MEMORIA:
					copy_string(cliente_fd);
					break;
				case EJECUTAR_STDIN_READ:
					stdin_read(cliente_fd);
					void *valor_leido = malloc(20);
    				memcpy(valor_leido, memoria->espacio_usuario+16, 20);
					log_info(logger,"EL DATO ES %s",valor_leido);
					break;
				case EJECUTAR_STDOUT_WRITE:
					stdout_write(cliente_fd);
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

void enviar_tam_pagina(int tam , int cliente_fd){
	t_paquete* paquete = crear_paquete(MANDAME_PAGINA);
	agregar_a_paquete(paquete, &tam, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}


void iterator(char* value) {
    log_info(logger_memoria, "%s", value);
}
