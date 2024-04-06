#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "./cpu.config";

	config = cargar_config(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy la cpu!");

    //iniciar configuraciones
	obtener_configuracion();
	//iniciar_recurso();
	//iniciar_consola();
	log_info(logger, "se inicio el servidor\n");
	//pthread_t servidor_interrupt;
	//pthread_t servidor_dispatch;
	//pthread_create(&servidor_interrupt,NULL,(void*)iniciar_servidor_interrupt,(void *) puerto_escucha_interrupt);
	//pthread_create(&servidor_dispatch,NULL,(void*)iniciar_servidor_cpu,NULL);
	iniciar_servidor_cpu();

	//pthread_join(servidor_dispatch,NULL);
	//thread_join(servidor_interrupt, NULL);
	//terminar_programa(conexion_memoria, logger, config);
    return 0;
}

void iniciar_servidor_cpu(){

	int cpu_fd = iniciar_servidor(puerto_escucha_dispatch);
	log_info(logger, "Servidor listo para recibir al cliente");
	generar_conexion_memoria();
	log_info(logger, "genere conexion con memoria");
	enviar_mensaje_instrucciones("genere conexion con memoria",conexion_memoria,MENSAJE);

	while(1){
	    int cliente_fd = esperar_cliente(cpu_fd);
		pthread_t atendiendo_cpu;
		pthread_create(&atendiendo_cpu,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		pthread_detach(atendiendo_cpu);
	}
}


void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	t_list* lista;
	int* auxiliar2;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);

		t_pcb* pcb_aux;
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			log_info(logger,"hola como estas capo");
			//enviar_mensaje("hola che",cliente_fd);
			break;
		case INSTRUCCIONES_A_MEMORIA:
			char* auxiliar =recibir_instruccion(cliente_fd);
			//log_info(logger_consola_cpu,"me llego la siguiente instruccion %s",auxiliar);
			//transformar_en_instrucciones(auxiliar);
//			hayInterrupcion = false;
			recibi_archivo=true;
			sem_post(&contador_instruccion);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			//log_info(logger, "Me llegaron los siguientes valores:\n");
			//list_iterate(lista, (void*) iterator);
			break;
		case ENVIARREGISTROCPU:
			t_list* valores_cpu = list_create();
			valores_cpu= recibir_paquete(cliente_fd);
			//log_info(logger, "ME LLEGARON");
			break;
			//TODO
			//preguntar porque si lo meto dentro de una funcion no me reconoce
		case RECIBIR_PCB:
			log_info(logger, "Estoy por recibir un PCB");
			t_list * paquete = recibir_paquete(cliente_fd);
			//contexto_ejecucion = desempaquetar_contexto_ejecucion(paquete);
			//recibir_pcb(cliente_fd);
			hayInterrupcion = false;
			hay_finalizar = false;
			hay_desalojo= false;
			//log_pcb_info(pcb);
			//ejecutar_ciclo_de_instruccion(cliente_fd);

			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			close(cliente_fd);
			return;
		default:
			//log_info(logger,"hola pepe");
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}

void generar_conexion_memoria(){
    log_info(logger, "generar conexion con memoria\n");
	pthread_t conexion_memoria_hilo_cpu;
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	pthread_create(&conexion_memoria_hilo_cpu,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
    pthread_detach(conexion_memoria_hilo_cpu);
	//enviar_mensaje_instrucciones("mandame las instrucciones plz ",conexion_memoria,MANDAME_PAGINA);
}


void obtener_configuracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha_dispatch = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	puerto_escucha_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
}

