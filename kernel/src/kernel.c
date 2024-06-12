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
				char* ruta_script = readline(">");
				ejecutar_script(ruta_script);
				//enviar_mensaje_instrucciones("hola",conexion_memoria,MENSAJE);
				
				break;
			case '2':
				log_info(logger, "Ingrese la ruta");
				char* ruta = readline(">");
				iniciar_proceso(ruta);
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
	t_pcb * exit;

	uint32_t *valor_uin32t;
	uint32_t *valor_uin32t2;
	uint32_t *valor_uin32t3;
	int* valor_entero;
	int* valor_entero2;
	char* valor_char;
	char* valor_char2;

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		
		case CONEXION_INTERFAZ:
			paquete = recibir_paquete(cliente_fd);
			char*n = list_get(paquete,0);
			int conexion_obtenido = cliente_fd;
			char * nombre_interfaz = strtok(n, "\n");
			agregar_interfaces(nombre_interfaz, conexion_obtenido);
			t_interfaz* interfaz =list_get(lista_interfaces,0);
			log_info(logger, "Se agrego la interfaz %s", interfaz->nombre_interface);
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
				case IO_SLEEP:
					paquete = recibir_paquete(cliente_fd);
					char * nombre_de_interfaz_sleep =list_get(paquete,0);
					log_info(logger,"%s", nombre_de_interfaz_sleep);
					int *unidad_trabajo_sleep = list_get(paquete,1);
					ejecutar_io_sleep(nombre_de_interfaz_sleep,*unidad_trabajo_sleep,running);
					//agregar_cola_ready(running);
					break;

				case EJECUTAR_WAIT:
					paquete = recibir_paquete(cliente_fd);
					char *nombre_recurso_wait =list_get(paquete,0);
					ejecutar_wait(nombre_recurso_wait,running);
					break;
				case EJECUTAR_SIGNAL:
					paquete = recibir_paquete(cliente_fd);
					char * nombre_recurso_signal =list_get(paquete,0);
					ejecutar_signal(nombre_recurso_signal,running);
					break;
				case EJECUTAR_IO_STDIN_READ:
					paquete = recibir_paquete(cliente_fd);
					valor_char =list_get(paquete,0);// nombre
					valor_entero =list_get(paquete,1); // nro marco
					valor_uin32t2 =list_get(paquete,2); //tamanio

					valor_entero2 = (int) valor_uin32t2;
					log_info(logger,"el nombre de interfaz es %s",valor_char);
					ejecutar_io_stdin_read(valor_char,*valor_entero,*valor_entero2,running);
					break;
				case IO_STDOUT_WRITE:
					paquete = recibir_paquete(cliente_fd);
					break;

				case IO_FS_CREATE:
					paquete = recibir_paquete(cliente_fd);

					break;
				case IO_FS_DELETE:
					paquete = recibir_paquete(cliente_fd);
					break;

				case IO_FS_TRUNCATE:
					paquete = recibir_paquete(cliente_fd);
					break;
				case IO_FS_WRITE:
					paquete = recibir_paquete(cliente_fd);
					break;
				case IO_FS_READ:
					paquete = recibir_paquete(cliente_fd);		
					break;
				default:
					//log_error(logger, "che no se que me mandaste");
				break;
			}
			break;
		case EJECUTAR_IO_SLEEP:
			//log_error(logger, "AAAAAAAAAAAA");
			paquete = recibir_paquete(cliente_fd);
			int *pid_a_sacar_sleep = list_get(paquete,0);
			//log_error(logger, "%i",*pid_a_sacar_sleep);
			io_sleep_ready(*pid_a_sacar_sleep);
		break;
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
			exit = running;
			finalizar_pcb(exit);
			log_warning(logger, "Finaliza el proceso %i - Motivo: OUT OF MEMORY",exit->contexto->pid);
			break;

		case FINALIZAR:
			paquete = recibir_paquete(cliente_fd);
			contexto = desempaquetar_pcb(paquete);
			running->contexto = contexto;
			exit= running;
			finalizar_pcb(exit);
			log_warning(logger, "Finaliza el proceso %i - Motivo: SUCCES",exit->contexto->pid);
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}

