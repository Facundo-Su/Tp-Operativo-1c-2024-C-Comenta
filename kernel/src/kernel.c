#include "kernel.h"

int main(int argc, char* argv[]) {
    //iniciar_recurso();
	iniciar();
	//char * e = planificador_a_string(planificador);
	//log_info(logger,"%i", quantum);
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
	//log_info(logger, "Servidor listo para recibir al cliente");
	//generar_conexion_memoria();

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
				"\n 7. Cambiar Grado De Multiprogramacion");
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
				log_warning(logger, "Finaliza el proceso %i - Motivo: INTERRUPTED_BY_USER",pid_finalizar);
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
				listar_proceso_por_estado();
				break;
			case '7':
				char* valor2 = readline(">");
				int nuevo_grado = atoi(valor2);
				log_info(logger,"Grado Anterior: %i - Grado Actual: %i",grado_multiprogramacion_ini,nuevo_grado);
				//sem_destroy(&sem_grado_multiprogramacion);

				//sem_init(&sem_grado_multiprogramacion, 0, nuevo_grado);
				if(grado_multiprogramacion_ini <nuevo_grado){
					int cantidad_en_espera= list_size(cola_ready->cola->elements)+1;
					int cantidad_proceso_grado_multiprogramacion = nuevo_grado-grado_multiprogramacion_ini;

					for(int i=0;i<cantidad_proceso_grado_multiprogramacion;i++){
						sem_post(&sem_grado_multiprogramacion);
					}
					//int contador_aux = list_size(cola_new->cola->elements)+1;

					//log_error(logger,"cantidad de contador aux es %i, cantidad de cantidad esperada es %i", contador_aux, cantidad_en_espera);
					int cantidad_proceso_agregar = contador_aux-cantidad_en_espera;

					// for(int i=0;i<cantidad_proceso_agregar;i++){
					// 	sem_post(&sem_new);
					// 	//sem_post(&sem_grado_multiprogramacion);
						
					// }
					
				}else{
					for(int i=0;i<grado_multiprogramacion_ini-nuevo_grado;i++){
						sem_wait(&sem_grado_multiprogramacion);
					}
				}
				grado_multiprogramacion_ini = nuevo_grado;
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
			char* tipo_interfaz =list_get(paquete,1);
			int conexion_obtenido = cliente_fd;
			char * nombre_interfaz = strtok(n, "\n");
			char *tipo_interfaz_limpio = strtok(tipo_interfaz, "\n");
			//log_error(logger,"el tipo de interfaz es %s",tipo_interfaz_limpio);
			t_tipo_fs tipo_enum = obtener_el_tipo_fs(tipo_interfaz_limpio);
			//log_error(logger, "Se ha conectado la interfaz %s y el tipo es %s", nombre_interfaz , tipo_interfaz_limpio);
			agregar_interfaces(nombre_interfaz, conexion_obtenido,tipo_enum);

			break;
		case CREAR_PROCESO:
			paquete = recibir_paquete(cliente_fd);
			pthread_mutex_unlock(&sem_memoria);
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
				case OUT_OF_MEMORY:
					paquete = recibir_paquete(cliente_fd);
					exit = running;
					finalizar_pcb(exit);
					pthread_mutex_unlock(&sem_exec);
					log_warning(logger, "Finaliza el proceso %i - Motivo: OUT OF MEMORY",exit->contexto->pid);
					//pthread_mutex_unlock(&sem_exec);
					break;
				case IO_SLEEP:
					paquete = recibir_paquete(cliente_fd);
					char * nombre_de_interfaz_sleep =list_get(paquete,0);
					//log_info(logger,"%s", nombre_de_interfaz_sleep);
					int *unidad_trabajo_sleep = list_get(paquete,1);
					t_pcb* pcb_sleep = running;
					ejecutar_io_sleep2(nombre_de_interfaz_sleep,*unidad_trabajo_sleep,pcb_sleep);
					pthread_mutex_unlock(&sem_exec);
					//agregar_cola_ready(running);
					break;

				case EJECUTAR_WAIT:
					paquete = recibir_paquete(cliente_fd);
					char *nombre_recurso_wait =list_get(paquete,0);
					//pthread_mutex_unlock(&sem_exec);
					ejecutar_wait(nombre_recurso_wait,running);
					
					break;
				case EJECUTAR_SIGNAL:
					paquete = recibir_paquete(cliente_fd);
					char * nombre_recurso_signal =list_get(paquete,0);
					//pthread_mutex_unlock(&sem_exec);
					ejecutar_signal(nombre_recurso_signal,running);
					break;
				case EJECUTAR_IO_STDIN_READ:
					paquete = recibir_paquete(cliente_fd);
					valor_char =list_get(paquete,0);// nombre
					valor_entero =list_get(paquete,1); // nro marco
					int *desplzazamiento_stdin_read = list_get(paquete,2);
					valor_uin32t2 =list_get(paquete,3); //tamanio
					int valor_entero_io_stdin_read = (int) *valor_uin32t2;
					t_pcb* pcb_io_stdin_read = running;
					/*log_warning(logger,"zxczxczxczxczxczxczxczxczxczxc");
					log_info(logger,"el nombre de la interfaz es %s",valor_char);
					log_info(logger,"el valor de marco es %i",*valor_entero);
					log_info(logger,"el valor de desplazamiento es %i",*desplzazamiento_stdin_read);
					log_info(logger,"el valor de tamanio es %i",valor_entero_io_stdin_read);

					log_info(logger,"el nombre de interfaz es %s, marco %i, desplazamiento %i, tamanio %i",valor_char,*valor_entero,*desplzazamiento_stdin_read,valor_entero_io_stdin_read);
					*/
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_stdin_read(valor_char,*valor_entero,*desplzazamiento_stdin_read,valor_entero_io_stdin_read,pcb_io_stdin_read);
					break;
				case IO_STDOUT_WRITE:
					paquete = recibir_paquete(cliente_fd);
					valor_char =list_get(paquete,0);// nombre
					valor_entero =list_get(paquete,1); // nro marco
					int *desplzazamiento_stdout_write = list_get(paquete,2);//desplazamiento
					valor_uin32t2 =list_get(paquete,3); //tamanio
					int valor_entero_io_stdin_write = (int) *valor_uin32t2;
					t_pcb* pcb_io_stdout_write = running;
					//log_info(logger,"el nombre de interfaz es %s, marco %i, desplazamiento %i, tamanio %i",valor_char,*valor_entero,*desplzazamiento_stdout_write,*valor_entero2);
					//log_warning(logger,"el pid es %i",running->contexto->pid);
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_stdin_write(valor_char,*valor_entero,*desplzazamiento_stdout_write,valor_entero_io_stdin_write,pcb_io_stdout_write);
					break;

				case IO_FS_CREATE:
					paquete = recibir_paquete(cliente_fd);
					char* nombre_interfaz_f_create = list_get(paquete,0);
					char* nombre_archivo_f_create = list_get(paquete,1);
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_fs_create(nombre_interfaz_f_create,nombre_archivo_f_create,running);

					break;
				case IO_FS_DELETE:
					paquete = recibir_paquete(cliente_fd);
					char* nombre_interfaz_f_delete = list_get(paquete,0);
					char* nombre_archivo_f_delete = list_get(paquete,1);
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_fs_delete(nombre_interfaz_f_delete,nombre_archivo_f_delete,running);
					break;

				case IO_FS_TRUNCATE:
					paquete = recibir_paquete(cliente_fd);
					char* nombre_interfaz_f_truncate = list_get(paquete,0);
					char* nombre_archivo_f_truncate = list_get(paquete,1);
					int *tamanio_f_truncate = list_get(paquete,2);
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_fs_truncate(nombre_interfaz_f_truncate,nombre_archivo_f_truncate,*tamanio_f_truncate,running);
					break;
				case IO_FS_WRITE:
					paquete = recibir_paquete(cliente_fd);
					char* nombre_interfaz_f_write = list_get(paquete,0);
					char* nombre_archivo_f_write = list_get(paquete,1);
					int *marco_f_write = list_get(paquete,2);
					int *desplazamiento_f_write = list_get(paquete,3);
					int *tamanio_f_write = list_get(paquete,4);
					int *puntero_f_write = list_get(paquete,5);
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_fs_write(nombre_interfaz_f_write,nombre_archivo_f_write,*marco_f_write,*desplazamiento_f_write,*tamanio_f_write,*puntero_f_write,running);
					break;
				case IO_FS_READ:
					paquete = recibir_paquete(cliente_fd);
					char* nombre_interfaz_f_read = list_get(paquete,0);
					char* nombre_archivo_f_read = list_get(paquete,1);
					int *marco_f_read = list_get(paquete,2);
					int *desplazamiento_f_read = list_get(paquete,3);
					int *tamanio_f_read = list_get(paquete,4);
					int *puntero_f_read = list_get(paquete,5);
					pthread_mutex_unlock(&sem_exec);
					ejecutar_io_fs_read(nombre_interfaz_f_read,nombre_archivo_f_read,*marco_f_read,*desplazamiento_f_read,*tamanio_f_read,*puntero_f_read,running);
					break;
				default:
					//log_error(logger, "che no se que me mandaste");
				break;
			}
			break;
		case EJECUTAR_IO_SLEEP:
			paquete = recibir_paquete(cliente_fd);
			int *pid_a_sacar_sleep = list_get(paquete,0);
			char* nombre_interfaz_sleep = list_get(paquete,1);
			//log_warning(logger, "me ha llegado ejecutar de vuelta a cpu el pid es %i",*pid_a_sacar_sleep);

			//los elementos en la lista bloqueados son
			for(int i=0;i<list_size(lista_bloqueado_io);i++){
				t_pcb* pcb_aux = list_get(lista_bloqueado_io,i);
				//log_error(logger, "el pid es %i",pcb_aux->contexto->pid);
			}
			//log_error(logger, "%i",*pid_a_sacar_sleep);
			io_sleep_ready2(*pid_a_sacar_sleep, nombre_interfaz_sleep);
		break;
		case EJECUTAR_STDIN_READ:
			paquete = recibir_paquete(cliente_fd);
			int *pid_stdin_read = list_get(paquete,0);
			char* nombre_interfaz_stdin_read = list_get(paquete,1);
			io_stdin_read_ready(*pid_stdin_read,nombre_interfaz_stdin_read);
			break;

		case EJECUTAR_STDOUT_WRITE:
			paquete = recibir_paquete(cliente_fd);
			int *pid_stdin_write = list_get(paquete,0);
			char *nombre_interfaz_stdin_write = list_get(paquete,1);
			//log_warning(logger, "el pid es %i",*pid_stdin_write);
			io_stdout_write_ready(*pid_stdin_write,nombre_interfaz_stdin_write);
			break;

		case ENVIAR_DESALOJAR:
			paquete = recibir_paquete(cliente_fd);
			contexto= desempaquetar_pcb(paquete);
			running->contexto = contexto;
			running->contexto->quantum=0;
			log_info(logger,"PID: %i - Desalojado por fin de Quantum", running->contexto->pid);
			log_info(logger, "PID: %i - Estado Anterior: RUNNING - Estado Actual: READY",running->contexto->pid);
			//log_error(logger, "el pid llegea desalojar es %i",running->contexto->pid);
			//log_pcb_info(pcb_aux);
			agregar_cola_ready(running);
			pthread_mutex_unlock(&sem_exec);
			break;
		case RESPUESTA_CREAR_ARCHIVO:
			paquete = recibir_paquete(cliente_fd);
			int *pid_crear_archivo = list_get(paquete,0);
			char*nombre_interfaz_crear_archivo = list_get(paquete,1);
			//log_warning(logger, "el pid de crear archivo es %i",*pid_crear_archivo);
			dial_fs_ready(nombre_interfaz_crear_archivo);
			break;
		case RESPUESTA_BORRAR_ARCHIVO:
			paquete = recibir_paquete(cliente_fd);
			int *pid_eliminar_archivo = list_get(paquete,0);
			char* nombre_interfaz_eliminar_archivo = list_get(paquete,1);
			//log_warning(logger, "el pid de crear archivo es %i",*pid_crear_archivo);
			//io_fs_delete_ready(*pid_eliminar_archivo);
			dial_fs_ready(nombre_interfaz_eliminar_archivo);
			break;

		case RESPUESTA_ESCRIBIR_ARCHIVO:
			paquete = recibir_paquete(cliente_fd);
			int *pid_escribir_archivo = list_get(paquete,0);
			//log_warning(logger, "el pid de crear archivo es %i",*pid_crear_archivo);
			//io_fs_write_ready(*pid_escribir_archivo);
			dial_fs_ready(nombre_interfaz_crear_archivo);
			break;
		case RESPUESTA_LEER_ARCHIVO:
			paquete = recibir_paquete(cliente_fd);
			int *pid_leer_archivo = list_get(paquete,0);
			char* nombre_interfaz_leer_archivo = list_get(paquete,1);
			//log_warning(logger, "el pid de crear archivo es %i",*pid_crear_archivo);
			//io_fs_read_ready(*pid_leer_archivo);
			dial_fs_ready(nombre_interfaz_leer_archivo);
			break;

		case RESPUESTA_TRUNCAR_ARCHIVO:
			paquete = recibir_paquete(cliente_fd);
			int *pid_truncar_archivo = list_get(paquete,0);
			char* nombre_interfaz_truncar_archivo = list_get(paquete,1);
			//log_warning(logger, "el pid de crear archivo es %i",*pid_crear_archivo);
			//io_fs_truncate_ready(*pid_truncar_archivo);
			dial_fs_ready(nombre_interfaz_truncar_archivo);
			break;
		case ENVIAR_FINALIZAR:
			paquete = recibir_paquete(cliente_fd);
			contexto = desempaquetar_pcb(paquete);
			running->contexto = contexto;
			exit= running;
			//inalizar_pcb(exit);
			//enviar_memoria_finalizar(exit->contexto->pid);
			//log_warning(logger, "Finaliza el proceso %i - Motivo: INTERRUPTED_BY_USER",exit->contexto->pid);
			break;
		
		case FINALIZAR:
			paquete = recibir_paquete(cliente_fd);
			contexto = desempaquetar_pcb(paquete);
			running->contexto = contexto;
			exit= running;
			finalizar_pcb(exit);
			//pthread_mutex_unlock(&sem_exec);
			log_warning(logger, "Finaliza el proceso %i - Motivo: SUCCESS",exit->contexto->pid);
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

t_tipo_fs obtener_el_tipo_fs(char* tipo){

	if(strcmp(tipo,"GENERICA")==0){
		return GENERICA;
	}
	else if(strcmp(tipo,"STDIN")==0){
		return STDIN;
	}
	else if(strcmp(tipo,"STDOUT")==0){
		return STDOUT;
	}else if (strcmp(tipo,"DIALFS")==0)
	{
		return DIALFS;
	}
	
}
