#include "kernel.h"

int main(int argc, char* argv[]) {
    //iniciar_recurso();
	iniciar();
	//char * e = planificador_a_string(planificador);
	//log_info(logger, "%s",e);
	log_info(logger,"%i", quantum);
	generar_conexion();
	inciar_planificadores();
	pthread_t iniciando_server;
	pthread_create(&iniciando_server,NULL,(void*)iniciar_servidor_kernel,NULL);
	pthread_detach(iniciando_server);
    iniciar_consola();
    return EXIT_SUCCESS;
}


void iniciar_servidor_kernel(){
	int kernel_fd = iniciar_servidor(puerto_escucha);
	log_info(logger, "Servidor listo para recibir al cliente");
	//generar_conexion_memoria();
	//log_info(logger, "genere conexion con memoria");

	while(1){
	    int cliente_fd = esperar_cliente(kernel_fd);
		pthread_t atendiendo_kernel;
		pthread_create(&atendiendo_kernel,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		pthread_detach(atendiendo_kernel);
	}
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
			
				//enviar_mensaje_instrucciones("hola",conexion_memoria,MENSAJE);
				
				break;
			case '2':
				log_info(logger_consola, "Ingrese la ruta");
				char* ruta = readline(">");
				char * tamaño_string = readline(">");
				int tamaño = atoi(tamaño_string);
				t_pcb* pcb = retorno_pcb();
				t_paquete* paquete =crear_paquete(CREAR_PROCESO);
				int pid = pcb->contexto->pid;
				agregar_a_paquete(paquete, ruta, strlen(ruta) + 1);
				agregar_a_paquete(paquete, &(pid), sizeof(int));
				agregar_a_paquete(paquete,&(tamaño), sizeof(int));
				enviar_paquete(paquete, conexion_memoria);
				eliminar_paquete(paquete);
				agregar_cola_new(pcb);
				if(primero == true){
					pthread_mutex_unlock(&sem_exec);
				}
				primero = false;
				//enviar_pcb(pcb_aux,conexion_memoria,CREAR_PROCESO);
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
				detener = false;
				pthread_mutex_unlock(&sem_detener_largo);
				pthread_mutex_unlock(&sem_detener);
				pthread_mutex_unlock(&sem_detener_conexion);
				
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
	pthread_detach(hilo_conexion_memoria);
	//conexion_cpu = crear_conexion(ip_cpu, puerto_cpu_dispatch);
	pthread_create(&hilo_conexion_cpu,NULL,(void*) procesar_conexion,(void *)&conexion_cpu);
	pthread_detach(hilo_conexion_cpu);
	//conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
	pthread_create(&hilo_conexion_cpu_interrupt, NULL, (void*) procesar_conexion, (void *)&conexion_cpu_interrupt);
	pthread_detach(hilo_conexion_cpu_interrupt);
}




void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	t_contexto_ejecucion * contexto;
	t_paquete * paquete;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		
		case CONEXION_INTERFAZ:
			paquete = recibir_paquete(cliente_fd);
			char* nombre_interfaz = list_get(paquete,0);
			int conexion_obtenido = cliente_fd;
			agregar_interfaces(nombre_interfaz, conexion_obtenido);
			//interfaz =list_get(lista_interfaces,0);
			//log_info(logger, "Se agrego la interfaz %s", interfaz->nombre_interface);
			break;	
		case RECIBIR_PCB:
			paquete = recibir_paquete(cliente_fd);
			contexto= desempaquetar_pcb(paquete);
			running->contexto = contexto;
			
			recv(cliente_fd,&cod_op,sizeof(op_code),0);
			if (detener == true)
			{
				pthread_mutex_lock(&sem_detener_conexion);
			}
			switch(cod_op){
				case EJECUTAR_WAIT:
					paquete = recibir_paquete(cliente_fd);
					char *nombre_recurso_wait =list_get(paquete,0);
					log_error(logger,"%s", nombre_recurso_wait);
					
					ejecutar_wait(nombre_recurso_wait,running);
					break;
				case EJECUTAR_SIGNAL:
					paquete = recibir_paquete(cliente_fd);
					char * nombre_recurso_signal =list_get(paquete,0);
					log_error(logger,"%s", nombre_recurso_signal);
					ejecutar_signal(nombre_recurso_signal,running);
					break;
				case IO_SLEEP:
					paquete = recibir_paquete(cliente_fd);
					char * nombre_de_interfaz_sleep =list_get(paquete,0);
					int *unidad_trabajo_sleep = list_get(paquete,1);
					log_error(logger,"el valor de pcb %s", nombre_de_interfaz_sleep);
					log_error(logger,"el valor de unidad es %i", *unidad_trabajo_sleep);
					//ejecutar_io_sleep(nombre_de_interfaz_sleep,*unidad_trabajo_sleep,running);
					agregar_cola_ready(running);
					break;
				case EJECUTAR_IO_SLEEP:
					paquete = recibir_paquete(cliente_fd);
					int *pid_a_sacar_sleep = list_get(paquete,0);
					sacar_meter_en_ready(*pid_a_sacar_sleep);
				break;
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
//		break;
		case ENVIAR_DESALOJAR:
			paquete = recibir_paquete(cliente_fd);
			contexto= desempaquetar_pcb(paquete);
			running->contexto = contexto;
			//log_pcb_info(pcb_aux);
			agregar_cola_ready(running);
			pthread_mutex_unlock(&sem_exec);
			break;

		case OUT_OF_MEMORY:
			paquete = recibir_paquete(cliente_fd);
			contexto = desempaquetar_pcb(paquete);
			running->contexto = contexto;
			finalizar_pcb(running);
			log_warning(logger, "Finaliza el proceso %i - Motivo: OUT OF MEMORY",running->contexto->pid);
			break;

		case FINALIZAR:
			paquete = recibir_paquete(cliente_fd);
			contexto = desempaquetar_pcb(paquete);
			running->contexto = contexto;
			finalizar_pcb(running);
			log_warning(logger, "Finaliza el proceso %i - Motivo: SUCCES",running->contexto->pid);
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

