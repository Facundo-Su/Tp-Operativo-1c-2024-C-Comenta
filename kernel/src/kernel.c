#include "kernel.h"

int main(int argc, char* argv[]) {
    //iniciar_recurso();
	iniciar();
	
	generar_conexion();

    iniciar_consola();
	inciar_planificadores();
    return EXIT_SUCCESS;
}


void iniciar_consola(){
	t_log * logger_consola = log_create("./kernelConsola.log", "consola", 1, LOG_LEVEL_INFO);
	char* variable;
	while(1){
		log_info(logger_consola,"ingrese la operacion que deseas realizar"
				"\n 1. Ejecutar script"
				"\n 2. Iniciar proceso "
				"\n 3. Finalizar proceso"
				"\n 4. Detener Planificacion"
				"\n 5. Iniciar Planificacion"
				"\n 6. Listar Procesos por Estado"
				"\n 7. Terminar Programa");
		variable = readline(">");

		switch (*variable) {
			case '1':
				enviar_mensaje_instrucciones("hola",conexion_memoria,MENSAJE);
				break;
			case '2':

				t_pcb* pcb_aux = retorno_pcb();
				enviar_pcb(pcb_aux,conexion_memoria,CREAR_PCB);
				break;
			case '3':
				log_info(logger_consola, "Ingrese el pid de proceso a finalizar");
				char* valor = readline(">");
				int pid_finalizar = atoi(valor);
				eliminar_pcb(pid_finalizar);
				break;
			case '4':
				log_info(logger,"PAUSA DE LA PLANIFICACIÓN");
				detener = true;
				break;
			case '5':
				log_info(logger,"INICIO DE LA PLANIFICACIÓN");
				pthread_mutex_unlock(&sem_detener);
				
				break;
			case '6':
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







void generar_conexion(){

	pthread_create(&hilo_conexion_memoria,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
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