#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "./cpu.config";

	config = cargar_config(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
	instruccion_a_realizar = malloc(sizeof(t_instruccion));
    //iniciar configuraciones
	obtener_configuracion();
	iniciar_recurso();
	//iniciar_consola();
	//log_info(logger, "se inicio el servidor\n");
	pthread_t servidor_interrupt;
	pthread_t servidor_dispatch;
	pthread_create(&servidor_interrupt,NULL,(void*)iniciar_servidor_interrupt,(void *) puerto_escucha_interrupt);
	pthread_create(&servidor_dispatch,NULL,(void*)iniciar_servidor_cpu,NULL);
	//iniciar_servidor_cpu();

	pthread_join(servidor_dispatch,NULL);
	pthread_join(servidor_interrupt, NULL);
	//terminar_programa(conexion_memoria, logger, config);
    return 0;
}

void iniciar_recurso(){
	sem_init(&contador_marco_obtenido,0,0);
	sem_init(&contador_respuesta,0,0);
	pthread_mutex_init(&contador_marco_obtenido,NULL);
	pthread_mutex_init(&sem_resize,NULL);
	//ptheard_mutex_init(&respuesta_ok,NULL);

	pthread_mutex_lock(&contador_marco_obtenido);
	pthread_mutex_lock(&sem_resize);
	//ptheard_mutex_lock(&respuesta_ok);


	tlb = list_create();
	contador_fifo =0;
	iniciar_entrada_tlb();
}

void iniciar_entrada_tlb(){
	for(int i = 0; i < cantidad_entrada_tlb; i++){
		t_estructura_tlb* entrada = malloc(sizeof(t_estructura_tlb));
		entrada->pid = -1;
		entrada->marco = -1;
		entrada->is_free = true;
		entrada->llegada_fifo = -1;
		entrada->last_time_lru = -1;
		list_add(tlb, entrada);
	}
}

void iniciar_servidor_cpu(){

	int cpu_fd = iniciar_servidor(puerto_escucha_dispatch);
	log_info(logger, "Servidor listo para recibir al cliente");
	generar_conexion_memoria();
	//log_info(logger, "genere conexion con memoria");
	//enviar_mensaje_instrucciones("genere conexion con memoria",conexion_memoria,MENSAJE);

	while(1){
		log_error(logger, "Esperando cliente");
	    int cliente_fd = esperar_cliente(cpu_fd);
		pthread_t atendiendo_cpu;
		pthread_create(&atendiendo_cpu,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		pthread_detach(atendiendo_cpu);
	}
}





void iniciar_servidor_interrupt(char * puerto){
	int cpu_interrupt_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");

	int cliente_fd = esperar_cliente(cpu_interrupt_fd);
		t_list* lista;
		while (1) {
			int cod_op = recibir_operacion(cliente_fd);
			switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(cliente_fd);
				break;
			case PAQUETE:
				lista = recibir_paquete(cliente_fd);
				//log_info(logger, "Me llegaron los siguientes valores:\n");
				//list_iterate(lista, (void*) iterator);
				break;
			case ENVIAR_DESALOJAR:
				log_error(logger, "Instruccion DESALOJAR");
				//recibir_mensaje(cliente_fd);
				lista = recibir_paquete(cliente_fd);
				int* pid_aux = list_get(lista,0);
				log_warning(logger,"los valores que comparo son: %d y %d",pcb->pid,*pid_aux);
				if(*pid_aux == pcb->pid){
					hay_desalojo = true;

				}else{
					log_error(logger, "No hay un proceso con ese PID por ende no desalojo");
				}
				
				//log_error(logger, "Instruccion DESALOJAR");
				break;
			case ENVIAR_FINALIZAR:
				lista = recibir_paquete(cliente_fd);
				int* pid_fin = list_get(lista,0);
				log_error(logger, "Instruccion FINALIZAR el pid es: %d",*pid_fin);
				hay_finalizar= true;
				break;
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando servidor");
				close(cliente_fd);
				return;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}
		return;
	}

//funcion para contar cuanto tiempo fue utilizado la siguiente funcion


void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	int* enteros;
	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		t_list* paquete;
		t_pcb* pcb_aux;
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			//log_info(logger,"hola como estas capo");
			//enviar_mensaje("hola che",cliente_fd);
			break;
		case INSTRUCCIONES_A_MEMORIA:
			char* auxiliar =recibir_instruccion(cliente_fd);
			//log_info(logger,"me llego la siguiente instruccion %s",auxiliar);
			transformar_en_instrucciones(auxiliar);
//			hayInterrupcion = false;
			recibi_archivo=true;
			//log_info(logger, "Recibi las instrucciones");
			sem_post(&contador_instruccion);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			//log_info(logger, "Me llegaron los siguientes valores:\n");
			//list_iterate(lista, (void*) iterator);
			break;
		case ENVIARREGISTROCPU:
			lista= recibir_paquete(cliente_fd);
			break;
			//TODO
			//preguntar porque si lo meto dentro de una funcion no me reconoce
		case RECIBIR_PCB:
			lista = recibir_paquete(cliente_fd);
			pcb = desempaquetar_pcb(lista);
			//recibir_pcb(cliente_fd);
			hayInterrupcion = false;
			hay_finalizar = false;
			hay_desalojo= false;
			ejecutar_ciclo_de_instruccion(cliente_fd);

			break;
		case OBTENER_MARCO:
			lista = recibir_paquete(cliente_fd);
			int *auxiliar2 = list_get(lista,0);
			marco_obtenido = *auxiliar2;
			pthread_mutex_unlock(&contador_marco_obtenido);

			break;

		case MANDAME_PAGINA:
			lista= recibir_paquete(cliente_fd);
			enteros= list_get(lista,0);
			tamanio_pagina= *enteros;
			//log_warning(logger, "El tamanio de la pagina es %i",tamanio_pagina);
			break;
		case RESIZE_TAM:
			lista= recibir_paquete(cliente_fd);
			enteros = list_get(lista,0);
			valor_retorno_resize = *enteros;
			pthread_mutex_unlock(&sem_resize);
			break;
		case ENVIO_MOV_IN:
			lista = recibir_paquete(cliente_fd);
			enteros = list_get(lista,0);
			registro_por_mov = *enteros;
			sem_post(&contador_respuesta);
			//pthread_mutex_unlock(&respuesta_ok);
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


void ejecutar_ciclo_de_instruccion(int cliente_fd){
	instruccion_ejecutando= true;
//pide a memoria

	while(!hayInterrupcion){
		if(hay_finalizar){
			enviar_pcb(pcb,cliente_fd,ENVIAR_FINALIZAR);
			return;
		}

		if(hay_desalojo){
			enviar_pcb(pcb,cliente_fd,ENVIAR_DESALOJAR);
			log_error(logger,"En desalojo el proceso pid %i",pcb->pid);
			return;
		}
		fetch(cliente_fd);
		log_error(logger,"En ejecucion");
	}


}

void fetch(int cliente_fd){
	int pc = pcb->pc;
	int pid = pcb->pid;
	log_info(logger, "PID: %i - FETCH - Program Counter: %i.",pid,pc);

	solicitar_instruccion_ejecutar_segun_pc(pc, pid);
	sem_wait(&contador_instruccion);
	pcb->pc+=1;
	decode(instruccion_a_realizar,cliente_fd);
	

}

void solicitar_instruccion_ejecutar_segun_pc(int pc,int pid){
	t_paquete* paquete = crear_paquete(INSTRUCCIONES_A_MEMORIA);
	agregar_a_paquete(paquete, &pc, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	//log_info(logger,"Solicite instrucciones");

}


void decode(t_instruccion* instrucciones,int cliente_fd){
	t_estrucutra_cpu registro_aux;
	t_estrucutra_cpu registro_aux2;
	t_estrucutra_cpu registro_aux3;
	char * recurso ="";
	char* parametro="";
	char* parametro2="";
	char* parametro3="";
	char* parametro4="";
	char* parametro5="";
	uint32_t valor_uint1;
	uint32_t valor_uint2;
	uint32_t valor_uint3;
	uint32_t valor_destino;
	uint32_t valor_origen;
	uint32_t resultado;
	int valor_int;
	//log_warning(logger,"%i",instrucciones->nombre);
	switch(instrucciones->nombre){
	case SET:

		parametro2= list_get(instrucciones->parametros,1);
		parametro= list_get(instrucciones->parametros,0);
		log_info(logger,"PID: %i - Ejecutando SET: %s - %s",pcb->pid,parametro,parametro2);
		valor_uint1 = strtoul(parametro2, NULL, 10);
		registro_aux = devolver_registro(parametro);
		setear(registro_aux,valor_uint1);
		//usleep(1000*1000);
		break;
	case SUB:

		parametro= list_get(instrucciones->parametros,0);
		parametro2= list_get(instrucciones->parametros,1);
		log_info(logger,"PID: %i - Ejecutando SUB: %s - %s",pcb->pid,parametro,parametro2);
		registro_aux = devolver_registro(parametro);
		registro_aux2 = devolver_registro(parametro2);
		//restar(pcb,registro_aux ,registro_aux2);
		    if (registro_aux == AX) {
		    	valor_destino = pcb->registros->ax;
		    } else if (registro_aux == BX) {
		    	valor_destino = pcb->registros->bx;
			} else if (registro_aux == CX) {
				valor_destino = pcb->registros->cx;
			}else if (registro_aux == DX) {
				valor_destino = pcb->registros->dx;
			}else if(registro_aux == EAX) {
				valor_destino = pcb->registros->eax;
			}else if(registro_aux == EBX) {
				valor_destino = pcb->registros->ebx;
			}else if (registro_aux == ECX){
				valor_destino = pcb->registros->ecx;
			}else if (registro_aux == EDX) {
				valor_destino = pcb->registros->edx;
			}else if (registro_aux == SI) {
				valor_destino = pcb->registros->si;
			}else{
				valor_destino =pcb->registros->di;
			}
			
		    if (registro_aux2 == AX) {
		    	valor_origen = pcb->registros->ax;
		    } else if (registro_aux2 == BX) {
		    	valor_origen = pcb->registros->bx;
			} else if (registro_aux2 == CX) {
				valor_origen = pcb->registros->cx;
			}else if (registro_aux2 == DX) {
				valor_origen = pcb->registros->dx;
			}else if(registro_aux2 == EAX) {
				valor_origen = pcb->registros->eax;
			}else if(registro_aux2 == EBX) {
				valor_origen = pcb->registros->ebx;
			}else if (registro_aux2 == ECX){
				valor_origen = pcb->registros->ecx;
			}else if (registro_aux2 == EDX) {
				valor_origen = pcb->registros->edx;
			}else if (registro_aux2 == SI) {
				valor_origen = pcb->registros->si;
			}else{
				valor_origen =pcb->registros->di;
			}


		    resultado = valor_destino - valor_origen;
		    if (registro_aux == AX) {
				pcb->registros->ax = resultado;
			}else if (registro_aux == BX) {
				pcb->registros->bx = resultado;
			}else if (registro_aux == CX) {
				pcb->registros->cx = resultado;
			}else if(registro_aux == DX) {
				pcb->registros->dx = resultado;
			}else if(registro_aux == EAX) {
				pcb->registros->eax = resultado;
			}else if(registro_aux == EBX) {
				pcb->registros->ebx = resultado;
			}else if(registro_aux == ECX) {
				pcb->registros->ecx = resultado;
			}else if(registro_aux == EDX) {
				pcb->registros->edx = resultado;
			}else if (registro_aux == SI) {
				pcb->registros->si = resultado;
			}else{
				pcb->registros->di = resultado;
			}
			


		break;
	case SUM:
		parametro= list_get(instrucciones->parametros,0);
		parametro2= list_get(instrucciones->parametros,1);
		log_info(logger,"PID: %i - Ejecutando SUM: %s - %s",pcb->pid,parametro,parametro2);
		registro_aux = devolver_registro(parametro);
		registro_aux2 = devolver_registro(parametro2);

		uint32_t valor_destino;
		uint32_t valor_origen;

		valor_destino = obtener_valor_registro(registro_aux);
		valor_origen = obtener_valor_registro(registro_aux2);

		resultado = valor_destino + valor_origen;
		asignar_valor_registro(resultado,registro_aux);
		break;
	case JNZ:
		parametro = list_get(instrucciones->parametros,0);
		parametro2 =list_get(instrucciones->parametros,1);
		log_info(logger,"PID: %i - Ejecutando JNZ: %s - %s",pcb->pid,parametro,parametro2);
		registro_aux = devolver_registro(parametro);
		valor_uint1 = obtener_valor(registro_aux);
		if(valor_uint1 !=0){
			int valorEntero = atoi(parametro2);
			pcb->pc =valorEntero;
		}
		break;
   case WAIT:
	    hayInterrupcion =true;
		recurso= list_get(instrucciones->parametros,0);
		log_info(logger,"PID: %i - Ejecutando WAIT: %s",pcb->pid,recurso);
		recurso = strtok(recurso, "\n");
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_recurso_a_kernel(recurso,EJECUTAR_WAIT,cliente_fd);
		break;
	case SIGNAL:
	    hayInterrupcion =true;
		recurso = list_get(instrucciones->parametros,0);
		log_info(logger,"PID: %i - Ejecutando SIGNAL: %s",pcb->pid,recurso);
		recurso = strtok(recurso, "\n");
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_recurso_a_kernel(recurso,EJECUTAR_SIGNAL,cliente_fd);
		break;
	case RESIZE:
		parametro = list_get(instrucciones->parametros,0);
		log_info(logger,"PID: %i - Ejecutando RESIZE: %s -",pcb->pid,parametro);
		int tamanio_modificar = atoi(parametro);
		enviar_memoria_ajustar_tam(tamanio_modificar);
		pthread_mutex_lock(&sem_resize);
		if(valor_retorno_resize == -1){
			hayInterrupcion=true;
			enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
			enviar_pcb(pcb,cliente_fd,OUT_OF_MEMORY);
		}
		break;
	case MOV_IN:
		parametro= list_get(instrucciones->parametros,0);//REGISTROS  DATOS
		parametro2= list_get(instrucciones->parametros,1);// REGISTROS DIRECCION
		
		registro_aux2 = devolver_registro(parametro2);
 		valor_int= obtener_valor(registro_aux2);

		t_traduccion* traducido = mmu_traducir(valor_int);

		log_info(logger,"PID: %i - Ejecutando MOV_IN: %s-%s",pcb->pid,parametro,parametro2);

		log_info(logger,"PID: %i - OBTENER MARCO - Página: %i - Marco: %i.",pcb->pid,traducido->nro_pagina,traducido->marco);
		valor_uint1 = obtener_el_valor_de_memoria(traducido);
		registro_aux = devolver_registro(parametro);

		valor_uint1 = (uint32_t) registro_por_mov;

		setear(registro_aux, valor_uint1);
		int dir_fisica = traducido->marco* tamanio_pagina + traducido->desplazamiento;
		log_info(logger, "PID: %i - Acción: LEER - Dirección Física: %i - Valor: %u",pcb->pid,dir_fisica,valor_uint1);
		break;
	case MOV_OUT:
		parametro= list_get(instrucciones->parametros,0);//REGISTROS  DATOS
		parametro2= list_get(instrucciones->parametros,1);// REGISTROS DIRECCION

		registro_aux = devolver_registro(parametro);
		valor_uint1 = obtener_valor(registro_aux);//valor direccion
		
		registro_aux2 = devolver_registro(parametro2);
 		valor_int= obtener_valor(registro_aux2);//datos

		t_traduccion* traducido2 = mmu_traducir(valor_uint1);
		log_info(logger,"PID: %i - Ejecutando MOV_OUT: %s-%s",pcb->pid,parametro,parametro2);
		log_info(logger,"PID: %i - OBTENER MARCO - Página: %i - Marco: %i.",pcb->pid,traducido2->nro_pagina,traducido2->marco);
		int dir_fisica2 = traducido2->marco* tamanio_pagina + traducido2->desplazamiento;
		log_info(logger, "PID: %i - Acción: ESCRIBIR - Dirección Física: %i - Valor: %u",pcb->pid,dir_fisica2,valor_uint1);
		enviar_traduccion_mov_out(traducido2, ENVIO_MOV_OUT, valor_int);
		
		break;
	case COPY_STRING:
	// MMU
		parametro = list_get(instrucciones->parametros,0);
		log_info(logger,"PID: %i - Ejecutando COPY_STRING: %s -",pcb->pid,parametro);
		int tamanio_copy_string = atoi(parametro);
		t_traduccion * traducido_copy_string_origen = mmu_traducir(pcb->registros->si);
		t_traduccion * traducido_copy_string_destino = mmu_traducir(pcb->registros->di);
		enviar_a_memoria_copy_string(tamanio_copy_string,traducido_copy_string_origen,traducido_copy_string_destino);
		break;
	
	case IO_GEN_SLEEP:
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0);
		parametro2 = list_get(instrucciones->parametros,1);
		log_info(logger,"PID: %i - Ejecutando IO_GEN_SLEEP: %s-%s",pcb->pid,parametro,parametro2);
		char * nombre_io_sleep = strtok(parametro, "\n");
		int entero_sleep = atoi(parametro2);
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_IO_SLEEP(nombre_io_sleep,entero_sleep,cliente_fd);
		break;
	case IO_STDIN_READ:
		//TODO
		
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0);//nombre interfaz
		parametro2 = list_get(instrucciones->parametros,1);// valor de registro direccion
		parametro3 = list_get(instrucciones->parametros,2); //registro tamanio

		log_info(logger,"PID: %i - Ejecutando IO_STDIN_READ: %s-%s-%s",pcb->pid,parametro,parametro2, parametro3);

		registro_aux = devolver_registro(parametro2);
		valor_uint1 = obtener_valor(registro_aux);

		t_traduccion* aux = mmu_traducir(valor_uint1);

		registro_aux2 = devolver_registro(parametro3);
		valor_uint2 = obtener_valor(registro_aux2);
		//log_info(logger,"ENVIO %u %u",valor_uint1, valor_uint2);
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_stdin_read(parametro,aux,valor_uint2,cliente_fd);

		break;
	case IO_STDOUT_WRITE:

		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0);
		parametro2 = list_get(instrucciones->parametros,1);
		parametro3 = list_get(instrucciones->parametros,2);

		log_info(logger,"PID: %i - Ejecutando IO_STDOUT_WRITE: %s-%s-%s",pcb->pid,parametro,parametro2, parametro3);

		registro_aux = devolver_registro(parametro2);
		valor_uint1 = obtener_valor(registro_aux);

		t_traduccion* aux2 = mmu_traducir(valor_uint1);

		registro_aux2 = devolver_registro(parametro3);
		valor_uint2 = obtener_valor(registro_aux2);

		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_stdout_write(parametro,aux2,valor_uint2,cliente_fd);
		break;
	case IO_FS_CREATE:
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0);
		parametro2 = list_get(instrucciones->parametros,1);
		log_info(logger,"PID: %i - Ejecutando IO_FS_CREATE: %s-%s",pcb->pid,parametro,parametro2);

		parametro2 =strtok(parametro2, "\n");
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_fs_create(parametro,parametro2,cliente_fd);

		break;
	case IO_FS_DELETE:
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0);
		parametro2 = list_get(instrucciones->parametros,1);
		log_info(logger,"PID: %i - Ejecutando IO_FS_DELETE: %s-%s",pcb->pid,parametro,parametro2);
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_fs_delete(parametro,parametro2,cliente_fd);
		break;
	case IO_FS_TRUNCATE:
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0);//nombre interfaz
		parametro2 = list_get(instrucciones->parametros,1);//nombre archivo
		parametro3 = list_get(instrucciones->parametros,2);// tamanio
		log_info(logger,"PID: %i - Ejecutando IO_FS_TRUNCATE: %s-%s-%s",pcb->pid,parametro,parametro2, parametro3);
		registro_aux = devolver_registro(parametro3);
		valor_uint1 = obtener_valor(registro_aux);
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_fs_truncate(parametro,parametro2,valor_uint1,cliente_fd);
		break;
	case IO_FS_WRITE:
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0); //nombre interfaz
		parametro2 = list_get(instrucciones->parametros,1);//nombre archivo
		parametro3 = list_get(instrucciones->parametros,2);//valor de registro direccion
		parametro4 = list_get(instrucciones->parametros,3);//valor de registro tamanio
		parametro5 = list_get(instrucciones->parametros,4);//valor de puntero archivo
		log_info(logger,"PID: %i - Ejecutando IO_FS_WRITE: %s-%s-%s-%s-%s",pcb->pid,parametro,parametro2, parametro3, parametro4, parametro5);
		
		registro_aux= devolver_registro(parametro3);
		registro_aux2= devolver_registro(parametro4);
		registro_aux3= devolver_registro(parametro5);
		valor_uint1 = obtener_valor(registro_aux);
		valor_uint2 = obtener_valor(registro_aux2);
		valor_uint3 = obtener_valor(registro_aux3);

		t_traduccion* traducido_f_write = mmu_traducir(valor_uint1);

		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_fs_write(parametro,parametro2,traducido_f_write,valor_uint2,valor_uint3,cliente_fd);

		break;
	
	case IO_FS_READ:
		hayInterrupcion = true;
		parametro = list_get(instrucciones->parametros,0); //nombre interfaz
		parametro2 = list_get(instrucciones->parametros,1);//nombre archivo
		parametro3 = list_get(instrucciones->parametros,2);//valor de registro direccion
		parametro4 = list_get(instrucciones->parametros,3);//valor de registro tamanio
		parametro5 = list_get(instrucciones->parametros,4);//valor de puntero archivo
		log_info(logger,"PID: %i - Ejecutando IO_FS_READ: %s-%s-%s-%s-%s",pcb->pid,parametro,parametro2, parametro3, parametro4, parametro5);
		
		registro_aux= devolver_registro(parametro3);
		registro_aux2= devolver_registro(parametro4);
		registro_aux3= devolver_registro(parametro5);
		valor_uint1 = obtener_valor(registro_aux);
		valor_uint2 = obtener_valor(registro_aux2);
		valor_uint3 = obtener_valor(registro_aux3);

		t_traduccion* traducido_f_read = mmu_traducir(valor_uint1);
		log_error(logger,"el valor traducido es %i",traducido_f_read->marco);
		enviar_pcb(pcb,cliente_fd,RECIBIR_PCB);
		enviar_io_fs_read(parametro,parametro2,traducido_f_read,valor_uint2,valor_uint3,cliente_fd);

		break;

	case EXIT:
		hayInterrupcion = true;
		log_info(logger,"PID: %i - Ejecutando EXIT:",pcb->pid);
		//imprimir_valores_registros(pcb->contexto->registros_cpu);
		enviar_pcb(pcb,cliente_fd,FINALIZAR);
		hay_desalojo = false;
		break;
	}
	recibi_archivo = false;
	instruccion_ejecutando= false;
}

void enviar_traduccion_mov_out(t_traduccion* traducido,op_code operacion,uint32_t valor_int){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &(traducido->marco), sizeof(int));
	agregar_a_paquete(paquete, &(traducido->desplazamiento), sizeof(int));
	agregar_a_paquete(paquete, &(traducido->nro_pagina), sizeof(int));
	agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
	agregar_a_paquete(paquete, &valor_int, sizeof(uint32_t));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}

uint32_t obtener_el_valor_de_memoria(t_traduccion* traducido){
	enviar_traduccion(traducido, ENVIO_MOV_IN);
	sem_wait(&contador_respuesta);
	//pthread_mutex_lock(&respuesta_ok);
	uint32_t valor_obtenido = registro_por_mov;
	return valor_obtenido;
}

void enviar_traduccion(t_traduccion* traducido,op_code operacion){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &(traducido->marco), sizeof(int));
	agregar_a_paquete(paquete, &(traducido->desplazamiento), sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}


void enviar_io_fs_read(char* nombre_interfaz,char* nombre_archivo,t_traduccion* traducido,uint32_t tamanio,uint32_t puntero,int cliente_fd){
	t_paquete* paquete = crear_paquete(IO_FS_READ);
	agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
	agregar_a_paquete(paquete, nombre_archivo, strlen(nombre_archivo)+1);
	agregar_a_paquete(paquete, &(traducido->marco), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(traducido->desplazamiento), sizeof(uint32_t));
	agregar_a_paquete(paquete, &tamanio, sizeof(uint32_t));
	agregar_a_paquete(paquete, &puntero, sizeof(uint32_t));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_io_fs_write(char* nombre_interfaz,char* nombre_archivo,t_traduccion* traducido,uint32_t tamanio,uint32_t puntero,int cliente_fd){
	t_paquete* paquete = crear_paquete(IO_FS_WRITE);
	agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
	agregar_a_paquete(paquete, nombre_archivo, strlen(nombre_archivo)+1);
	agregar_a_paquete(paquete, &(traducido->marco), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(traducido->desplazamiento), sizeof(uint32_t));
	agregar_a_paquete(paquete, &tamanio, sizeof(uint32_t));
	agregar_a_paquete(paquete, &puntero, sizeof(uint32_t));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_io_fs_truncate(char* parametro,char* parametro2,uint32_t parametro3,int cliente_fd){
	t_paquete* paquete = crear_paquete(IO_FS_TRUNCATE);
	agregar_a_paquete(paquete, parametro, strlen(parametro)+1);
	agregar_a_paquete(paquete, parametro2, strlen(parametro2)+1);
	agregar_a_paquete(paquete, &parametro3, sizeof(uint32_t));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_io_fs_delete(char* parametro,char* parametro2,int cliente_fd){
	t_paquete* paquete = crear_paquete(IO_FS_DELETE);
	agregar_a_paquete(paquete, parametro, strlen(parametro)+1);
	agregar_a_paquete(paquete, parametro2, strlen(parametro2)+1);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_io_fs_create(char* parametro,char* parametro2,int cliente_fd){
	t_paquete* paquete = crear_paquete(IO_FS_CREATE);
	agregar_a_paquete(paquete, parametro, strlen(parametro)+1);
	agregar_a_paquete(paquete, parametro2, strlen(parametro2)+1);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_io_stdout_write(char* parametro,t_traduccion* traducido,uint32_t parametro3,int cliente_fd){
	//log_error(logger, "ENVIO FWRITE nombre %s,marco %i, despĺazamiento %i, tamanio %i",parametro,traducido->marco,traducido->desplazamiento,parametro3);
	t_paquete* paquete = crear_paquete(IO_STDOUT_WRITE);
	agregar_a_paquete(paquete, parametro, strlen(parametro)+1);
	agregar_a_paquete(paquete, &(traducido->marco), sizeof(int));
	agregar_a_paquete(paquete,&(traducido->desplazamiento), sizeof(int));
	agregar_a_paquete(paquete, &parametro3, sizeof(uint32_t));
	enviar_paquete(paquete, cliente_fd);
}

void enviar_io_stdin_read(char* parametro,t_traduccion* traducido,uint32_t parametro3,int cliente_fd){

	t_paquete* paquete = crear_paquete(EJECUTAR_IO_STDIN_READ);
	agregar_a_paquete(paquete, parametro, strlen(parametro)+1);
	agregar_a_paquete(paquete, &(traducido->marco), sizeof(int));
	agregar_a_paquete(paquete,&(traducido->desplazamiento), sizeof(int));
	agregar_a_paquete(paquete, &parametro3, sizeof(uint32_t));
	log_error(logger, "enviando paquete %s,marco %i, despĺazamiento %i, tamanio %i",parametro,traducido->marco,traducido->desplazamiento,parametro3);
	enviar_paquete(paquete, cliente_fd);
}

int obtener_valor_registro(t_estrucutra_cpu registros_aux) {
    switch (registros_aux) {
        case AX:
            return pcb->registros->ax;
        case BX:
            return pcb->registros->bx;
        case CX:
            return pcb->registros->cx;
        case DX:
            return pcb->registros->dx;
        case EAX:
            return pcb->registros->eax;
        case EBX:
            return pcb->registros->ebx;
        case ECX:
            return pcb->registros->ecx;
        case EDX:
            return pcb->registros->edx;
        case SI:
            return pcb->registros->si;
        default:
            return pcb->registros->di;
    }
}

void asignar_valor_registro(int valor, t_estrucutra_cpu registros_aux) {
    switch (registros_aux) {
        case AX:
            pcb->registros->ax = (uint8_t)valor;
            break;
        case BX:
            pcb->registros->bx = (uint8_t)valor;
            break;
        case CX:
            pcb->registros->cx = (uint8_t)valor;
            break;
        case DX:
            pcb->registros->dx = (uint8_t)valor;
            break;
        case EAX:
            pcb->registros->eax = valor;
            break;
        case EBX:
            pcb->registros->ebx = valor;
            break;
        case ECX:
            pcb->registros->ecx = valor;
            break;
        case EDX:
            pcb->registros->edx = valor;
            break;
        case SI:
            pcb->registros->si = valor;
            break;
        default:
            pcb->registros->di = valor;
            break;
    }
}


void enviar_IO_SLEEP(char* parametro,int parametro2,int cliente_fd){
	t_paquete* paquete = crear_paquete(IO_SLEEP);
	agregar_a_paquete(paquete, parametro, strlen(parametro)+1);
	agregar_a_paquete(paquete, &parametro2, sizeof(int));
	//log_warning(logger,"PID: %i - Ejecutando IO_SLEEP: %s - %i",pcb->pid,parametro,parametro2);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_a_memoria_copy_string(int tamanio,t_traduccion* traducido_origen,t_traduccion* traducido_destino){
	t_paquete* paquete = crear_paquete(COPY_STRING_MEMORIA);
	agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
	agregar_a_paquete(paquete, &tamanio, sizeof(int));
	agregar_a_paquete(paquete,&(traducido_origen->marco),sizeof(int));
	agregar_a_paquete(paquete,&(traducido_origen->desplazamiento),sizeof(int));
	agregar_a_paquete(paquete,&(traducido_destino->marco),sizeof(int));
	agregar_a_paquete(paquete,&(traducido_destino->desplazamiento),sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}

void enviar_memoria_ajustar_tam(int tamanio_modificar){
	t_paquete* paquete = crear_paquete(ENVIO_RESIZE);
	agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
	agregar_a_paquete(paquete, &tamanio_modificar, sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}

t_traduccion* mmu_traducir(int dir_logica){

	t_traduccion* traducido= malloc(sizeof(t_traduccion));
	int nro_pagina =  floor(dir_logica / tamanio_pagina);
	int marco_encontrado;
	log_warning(logger,"PID: %i - Ejecutando MMU TRADUCIR: %i",pcb->pid,dir_logica);
	log_warning(logger,"PID: %i - Tamanio Pagina: %i",pcb->pid,tamanio_pagina);
	log_warning(logger,"PID: %i - Pagina: %i",pcb->pid,nro_pagina);
	if(cantidad_entrada_tlb>0){
		marco_encontrado = consultar_tlb(nro_pagina, pcb->pid);
		if(marco_encontrado == -1){
			log_info(logger,"PID: %i - TLB MISS - Pagina: %i",pcb->pid,nro_pagina);
			obtener_el_marco(nro_pagina,OBTENER_MARCO);
			pthread_mutex_lock(&contador_marco_obtenido);
			insertar_tlb(pcb->pid,nro_pagina);
			marco_encontrado = marco_obtenido;
		}else{
			log_info(logger,"PID: %i - TLB HIT - Pagina: %i",pcb->pid,nro_pagina);
		}
	}else{

		obtener_el_marco(nro_pagina,OBTENER_MARCO);
		pthread_mutex_lock(&contador_marco_obtenido);
		marco_encontrado = marco_obtenido;
	}

	int desplazamiento = dir_logica - nro_pagina * tamanio_pagina;
	traducido->marco= marco_encontrado;
	traducido->desplazamiento= desplazamiento;
	traducido->nro_pagina = nro_pagina;


	return traducido;
}

void obtener_el_marco(int nro_pagina,op_code operacion){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
	agregar_a_paquete(paquete, &nro_pagina, sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}


void enviar_recurso_a_kernel(char* recurso,op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, recurso, strlen(recurso)+1);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}


void generar_conexion_memoria(){
    log_info(logger, "generar conexion con memoria\n");
	pthread_t conexion_memoria_hilo_cpu;
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	pthread_create(&conexion_memoria_hilo_cpu,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
	//log_info(logger, "mandame intrucciones\n");
	enviar_mensaje_instrucciones("mandame las instrucciones plz ",conexion_memoria,MANDAME_PAGINA);
}


void obtener_configuracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha_dispatch = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	puerto_escucha_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
	cantidad_entrada_tlb = config_get_int_value(config,"CANTIDAD_ENTRADAS_TLB");
	char * alg_tlb = config_get_string_value(config,"ALGORITMO_TLB");

	if(strcmp(alg_tlb,"FIFO")==0){
		algoritmo= FIFO;
	}else if(strcmp(alg_tlb,"LRU")==0){
		algoritmo= LRU;
	}
}


void transformar_en_instrucciones(char* auxiliar){
	instruccion_a_realizar->parametros= list_create();
	int cantidad_parametros=0;
	char** instruccion_parseada = parsear_instruccion(auxiliar);
			//log_warning(logger,"%s",instruccion_parseada[0]);
	        if (strcmp(instruccion_parseada[0], "SET") == 0) {
	        	instruccion_a_realizar->nombre = SET;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "SUB") == 0) {
	        	instruccion_a_realizar->nombre = SUB;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "SUM") == 0) {
	        	instruccion_a_realizar->nombre = SUM;
	            cantidad_parametros = 2;
	        }

	        if (strcmp(instruccion_parseada[0], "JNZ") == 0) {
	        	instruccion_a_realizar->nombre = JNZ;
	            cantidad_parametros = 2;
	        }
			if(strcmp(instruccion_parseada[0], "RESIZE") == 0){
				instruccion_a_realizar->nombre = RESIZE;
				cantidad_parametros = 1;
			}
			if(strcmp(instruccion_parseada[0], "COPY_STRING") == 0){
				instruccion_a_realizar->nombre = COPY_STRING;
				cantidad_parametros = 1;
			}
	        if (strcmp(instruccion_parseada[0], "WAIT") == 0) {
	        	instruccion_a_realizar->nombre = WAIT;
	            cantidad_parametros = 1;
	        }
	        if (strcmp(instruccion_parseada[0], "SIGNAL") == 0) {
	        	instruccion_a_realizar->nombre = SIGNAL;
	            cantidad_parametros = 1;
	        }
			if(strcmp(instruccion_parseada[0], "IO_GEN_SLEEP") == 0){
				instruccion_a_realizar->nombre = IO_GEN_SLEEP;
				cantidad_parametros = 2;
			}
			if(strcmp(instruccion_parseada[0], "IO_STDIN_READ") == 0){
				instruccion_a_realizar->nombre = IO_STDIN_READ;
				cantidad_parametros = 3;
			}
			if(strcmp(instruccion_parseada[0], "IO_STDOUT_WRITE") == 0){
				instruccion_a_realizar->nombre = IO_STDOUT_WRITE;
				cantidad_parametros = 3;
			}
			if(strcmp(instruccion_parseada[0], "IO_FS_CREATE") == 0){
				instruccion_a_realizar->nombre = IO_FS_CREATE;
				cantidad_parametros = 2;
			}
			if(strcmp(instruccion_parseada[0], "IO_FS_DELETE") == 0){
				instruccion_a_realizar->nombre = IO_FS_DELETE;
				cantidad_parametros = 2;
			}
			if(strcmp(instruccion_parseada[0], "IO_FS_TRUNCATE") == 0){
				instruccion_a_realizar->nombre = IO_FS_TRUNCATE;
				cantidad_parametros = 3;
			}
			if(strcmp(instruccion_parseada[0], "IO_FS_WRITE") == 0){
				instruccion_a_realizar->nombre = IO_FS_WRITE;
				cantidad_parametros = 5;
			}
			if(strcmp(instruccion_parseada[0], "IO_FS_READ") == 0){
				instruccion_a_realizar->nombre = IO_FS_READ;
				cantidad_parametros = 5;
			}
	        if (strcmp(instruccion_parseada[0], "MOV_IN") == 0) {
	        	instruccion_a_realizar->nombre = MOV_IN;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "MOV_OUT") == 0) {
	        	instruccion_a_realizar->nombre = MOV_OUT;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "EXIT") == 0) {
	        	instruccion_a_realizar->nombre = EXIT;
	            cantidad_parametros = 0;
	        }
	    	t_list* parametros = list_create();

	        for(int i=1;i<cantidad_parametros+1;i++){
				//log_error(logger, "%s", instruccion_parseada[i]);
	            list_add(parametros,instruccion_parseada[i]);
	        }

	        list_add_all(instruccion_a_realizar->parametros,parametros);
}


char** parsear_instruccion(char* instruccion){

    // Parseo la instruccion
    char** instruccion_parseada = string_split(instruccion, " ");

    // Retorno la instruccion parseada
    return instruccion_parseada;
}

t_estrucutra_cpu devolver_registro(char* registro){
	t_estrucutra_cpu v;
    if(strcmp(registro,"AX")==0 || strcmp(registro,"AX\n")==0){
        v = AX;
    } else if(strcmp(registro,"BX")==0|| strcmp(registro,"BX\n")==0){
        v = BX;
    } else if(strcmp(registro,"CX")==0|| strcmp(registro,"CX\n")==0){
        v = CX;
    } else if(strcmp(registro,"DX")==0|| strcmp(registro,"DX\n")==0){
        v = DX;
	} else if(strcmp(registro,"EAX")==0|| strcmp(registro,"EAX\n")==0){
		v = EAX;
	} else if(strcmp(registro,"EBX")==0|| strcmp(registro,"EBX\n")==0){
		v = EBX;
	} else if(strcmp(registro,"ECX")==0|| strcmp(registro,"ECX\n")==0){
		v = ECX;
	} else if(strcmp(registro,"EDX")==0|| strcmp(registro,"EDX\n")==0){
		v = EDX;
	} else if(strcmp(registro,"SI")==0|| strcmp(registro,"SI\n")==0){
		v = SI;
	} else if(strcmp(registro,"DI")==0|| strcmp(registro,"DI\n")==0){
		v= DI;
	}else {
		v = PC;
	}
    return v;
}


void setear(t_estrucutra_cpu pos, uint32_t valor) {

    switch(pos) {
        case AX: pcb->registros->ax =(uint8_t) valor; break;
        case BX: pcb->registros->bx =(uint8_t) valor; break;
        case CX: pcb->registros->cx =(uint8_t) valor; break;
        case DX: pcb->registros->dx =(uint8_t) valor; break;
		case EAX: pcb->registros->eax = valor; break;
		case EBX: pcb->registros->ebx = valor; break;
		case ECX: pcb->registros->ecx = valor; break;
		case EDX: pcb->registros->edx = valor; break;
		case SI: pcb->registros->si = valor; break;
		case DI: pcb->registros->di = valor; break;
		case PC: pcb->pc = valor; break;
    }
}

uint32_t obtener_valor(t_estrucutra_cpu pos) {
	uint32_t valor_retorno=0;
    switch(pos) {
        case AX:
        	valor_retorno =pcb->registros->ax;
        	break;
        case BX:
        	valor_retorno =pcb->registros->bx;
        	break;
        case CX:
        	valor_retorno =pcb->registros->cx;
        	break;
        case DX:
        	valor_retorno =pcb->registros->dx;
        	break;
		case EAX:
			valor_retorno =pcb->registros->eax;
			break;
		case EBX:
			valor_retorno =pcb->registros->ebx;
			break;
		case ECX:
			valor_retorno =pcb->registros->ecx;
			break;
		case EDX:
			valor_retorno =pcb->registros->edx;
			break;
		case SI:
			valor_retorno =pcb->registros->si;
			break;
		case DI:
			valor_retorno =pcb->registros->di;
			break;
        default:
        	return 0;

    }
	return valor_retorno;
}


int consultar_tlb(int nro_pagina,int pid) {

	for (int i = 0; i < cantidad_entrada_tlb; i++) {
		t_estructura_tlb *tlb_aux = list_get(tlb, i);
		if (tlb_aux->pid == pid && tlb_aux->pagina == nro_pagina) {
			tlb_aux->last_time_lru =0;
			actualizar_algortimos();
			return tlb_aux->marco;
		}
	}
	return -1;
}

int encontrar_tlb_libre() {
    int i;
	for(i=0;i<cantidad_entrada_tlb;i++) {
    	t_estructura_tlb *tlb_aux = list_get(tlb, i);
		if(tlb_aux->is_free) {
            return i;
		}
	}
    return -1;
}

void insertar_tlb(int pid, int nro_pagina) {
	int i = encontrar_tlb_libre();

	if(i!=-1) {
		t_estructura_tlb *tlb_aux = list_get(tlb, i);
		tlb_aux->pid = pid;
		tlb_aux->pagina = nro_pagina;
		tlb_aux->is_free = false;
		tlb_aux->marco = marco_obtenido;
		tlb_aux->llegada_fifo = contador_fifo;
		tlb_aux->last_time_lru =0;
	}else{
		log_info(logger, "TLB llena");
		int aux = ejecutar_algoritmo();
		t_estructura_tlb *tlb_aux = list_get(tlb, aux);
		tlb_aux->pid = pid;
		tlb_aux->pagina = nro_pagina;
		tlb_aux->is_free = false;
		tlb_aux->marco = marco_obtenido;
		tlb_aux->llegada_fifo = contador_fifo;
		tlb_aux->last_time_lru =0;
	}
	actualizar_algortimos();
}


int ejecutar_algoritmo(){
	switch(algoritmo){
	case FIFO:
		return ejecutar_fifo();
		break;
	case LRU:
		return ejecutar_lru();
		break;
	default:
		return -1;
	break;
	}
}

int ejecutar_fifo(){
	t_list_iterator* iterador = list_iterator_create(tlb);
	int llegada = INT_MAX;
	int numero;
	int i=0;
	while(list_iterator_has_next(iterador)){
		t_estructura_tlb *aux= (t_estructura_tlb*)list_iterator_next(iterador);
		if(!aux->is_free && (aux->llegada_fifo < llegada)){
			llegada = aux->llegada_fifo;
			numero = i;
		}
		i++;
	}
	list_iterator_destroy(iterador);
	return numero;
}
int ejecutar_lru(){
	t_list_iterator* iterador = list_iterator_create(tlb);
	int tiempo = 0;
	int numero;
	int i=0;
	while(list_iterator_has_next(iterador)){
		t_estructura_tlb *aux= (t_estructura_tlb*)list_iterator_next(iterador);
		//log_error(logger, "a comparar marco %i tiempo lru : %i ",marco->num_marco,marco->last_time_lru);
		if(!aux->is_free && (aux->last_time_lru > tiempo)){
			//log_error(logger, "marco %i tiempo lru : %i ",marco->num_marco,marco->last_time_lru);
			tiempo = aux->last_time_lru;
			numero = i;
			//log_warning(logger, "llegue hasta aca porque encontre algo");
		}
		i++;
	}
	list_iterator_destroy(iterador);
	return numero;
}

void actualizar_algortimos(){
	switch(algoritmo){
	case FIFO:
		contador_fifo++;
		break;
	case LRU:
		 actualizar_lru();
		break;
	default:
		//log_info(logger,"ERROR Planificador")
	break;
	}
}

void actualizar_lru(){
	t_list_iterator* iterador = list_iterator_create(tlb);
	while(list_iterator_has_next(iterador)){
		t_estructura_tlb *aux= (t_estructura_tlb*)list_iterator_next(iterador);
		if(!aux->is_free){
			aux->last_time_lru ++;
			//log_error(logger,"ACTUALICE LRU");
		}
	}
	list_iterator_destroy(iterador);
}
