#include "kernel.h"

int main(int argc, char* argv[]) {


	char *rutaConfig = "kernel.config";
	config = cargar_config(rutaConfig);

    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el Kernel!");
    obtener_configuracion();
    //iniciar_recurso();
	generar_conexion();

    iniciar_consola();

    return EXIT_SUCCESS;
}


void iniciar_consola(){
	t_log * logger_consola = log_create("./kernelConsola.log", "consola", 1, LOG_LEVEL_INFO);
	char* variable;
	while(1){
		log_info(logger_consola,"ingrese la operacion que deseas realizar"
				"\n 1. Iniciar Proceso"
				"\n 2. Finalizar proceso"
				"\n 3. Iniciar Planificacion"
				"\n 4. Detener Planificacion"
				"\n 5. Modificar Grado Multiprogramacion"
				"\n 6. Listar Procesos por Estado"
				"\n 7. Terminar Programa");
		variable = readline(">");

		switch (*variable) {
			case '1':
				enviar_mensaje("hola",conexion_memoria);
				break;
			case '2':
				log_info(logger_consola, "Ingrese pid");
				char* valor = readline(">");
				int valorNumero = atoi(valor);
				break;
			case '3':
				log_info(logger,"INICIO DE PLANIFICACIÓN");
				//iniciar_planificacion();

				break;
			case '4':
				log_info(logger,"PAUSA DE PLANIFICACIÓN");
				//detener_planificacion_corto_largo();
				//detenido = true;
				break;
			case '5':
				char* valor2 = readline(">");
				int nuevo_grado = atoi(valor2);
				log_info(logger,"Grado Anterior: %i - Grado Actual: %i",grado_multiprogramacion_ini,nuevo_grado);
				//grado_multiprogramacion_ini = nuevo_grado;
				//sem_destroy(&grado_multiprogramacion);
				//sem_init(&grado_multiprogramacion, 0, grado_multiprogramacion_ini);
				//modificar_grado_multiprogramacion();
				break;
			case '6':
				//listar_proceso_estado();
				//listar_proceso_estado();
				break;
			case '7':
				//finalizar_programa();
				break;
			default:
				break;
		}
	}

}

void obtener_configuracion(){

    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignar_algoritmo(algoritmo);
    quantum = config_get_int_value(config, "QUANTUM");
    recursos = config_get_array_value(config, "RECURSOS");
    instancia_recurso = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion_ini = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
}

void asignar_algoritmo(char *algoritmo){
	if (strcmp(algoritmo, "FIFO") == 0) {
		planificador = FIFO;
	} else if (strcmp(algoritmo, "RR") == 0) {
		planificador = RR;
	}else if(strcmp(algoritmo, "VRR")==0){
		planificador = VRR;
	}else{
		log_error(logger, "El algoritmo no es valido");
	}
}



void generar_conexion(){
	pthread_t conexion_memoria_hilo;
	//pthread_t conexion_file_system_hilo;
	pthread_t conexion_cpu_hilo;
	pthread_t conexion_cpu_interrupt_hilo;

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	pthread_create(&conexion_memoria_hilo,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
	pthread_detach(conexion_memoria_hilo);
	// conexion_cpu = crear_conexion(ip_cpu, puerto_cpu_dispatch);
	// pthread_create(&conexion_cpu_hilo,NULL,(void*) procesar_conexion,(void *)&conexion_cpu);
	// pthread_detach(conexion_cpu_hilo);
	// conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
	// pthread_create(&conexion_cpu_interrupt_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_cpu_interrupt);
	// pthread_detach(conexion_cpu_interrupt_hilo);
}




void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case RECIBIR_PCB:
			recv(cliente_fd,&cod_op,sizeof(op_code),0);
			switch(cod_op){
			default:
				//log_error(logger, "che no se que me mandaste");
				break;
			}
			break;
		// case RESPUESTA_ABRIR_ARCHIVO:
		// 	paquete = recibir_paquete(cliente_fd);
		// 	int* tam_archivo_recibido = list_get(paquete,0);
		// 	tam_archivo = *tam_archivo_recibido;
		// 	sem_post(&contador_bloqueado_fs_fopen);

		// 	break;
		// case OK_PAG_CARGADA:
		// 	t_pcb * pcb_2 = queue_pop(list_bloqueado_page_fault);
		// 	agregar_a_cola_ready(pcb_2);
		// 	sem_post(&contador_cola_ready);
		// 	break;
//		case OK_TRUNCAR_ARCHIVO:
//			log_error(logger,"cantidad de elemento que hay en bloqueado fs es %i",queue_size(cola_bloqueado_fs));
//			t_pcb * pcb_3 = queue_pop(cola_bloqueado_fs);
//			agregar_a_cola_ready(pcb_3);
//			sem_post(&contador_cola_ready);
//			break;
//		case OK_FREAD:
//			t_pcb * pcb_4 = queue_pop(cola_bloqueado_fs);
//			agregar_a_cola_ready(pcb_4);
//			sem_post(&contador_cola_ready);
//			break;
//		case OK_FWRITE:
//			log_error(logger,"el pcb esta la respuesta");
//			t_pcb * pcb_5 = queue_pop(cola_bloqueado_fs);
//			agregar_a_cola_ready(pcb_5);
//			sem_post(&contador_cola_ready);
//			break;
//		case RESPUESTA_CREAR_ARCHIVO:
//			paquete = recibir_paquete(cliente_fd);
//			int* tam_archivo_recibido_creado = list_get(paquete,0);
//			tam_archivo = *tam_archivo_recibido_creado;
//			log_error(logger,"llegue a respuesta crear archivo");
//			sem_post(&sem_ok_archivo_creado);
//			break;
		case FINALIZAR:

			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return;
		default:
			//log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}