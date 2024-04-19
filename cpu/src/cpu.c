#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "./cpu.config";

	config = cargar_config(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy la cpu!");
	instruccion_a_realizar = malloc(sizeof(t_instruccion));
    //iniciar configuraciones
	obtener_configuracion();
	//iniciar_recurso();
	//iniciar_consola();
	log_info(logger, "se inicio el servidor\n");
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

void iniciar_servidor_cpu(){

	int cpu_fd = iniciar_servidor(puerto_escucha_dispatch);
	log_info(logger, "Servidor listo para recibir al cliente");
	generar_conexion_memoria();
	log_info(logger, "genere conexion con memoria");
	//enviar_mensaje_instrucciones("genere conexion con memoria",conexion_memoria,MENSAJE);

	while(1){
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
				recibir_mensaje(cliente_fd);
				hay_desalojo = true;
				log_error(logger, "Instruccion DESALOJAR");
				break;
			case ENVIAR_FINALIZAR:
				recibir_mensaje(cliente_fd);
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
	t_list* lista;
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
			log_info(logger,"me llego la siguiente instruccion %s",auxiliar);
			transformar_en_instrucciones(auxiliar);
//			hayInterrupcion = false;
			recibi_archivo=true;
			log_info(logger, "Recibi las instrucciones");
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
			pcb = desempaquetar_pcb(paquete);
			//recibir_pcb(cliente_fd);
			hayInterrupcion = false;
			hay_finalizar = false;
			hay_desalojo= false;
			//log_pcb_info(pcb);
			ejecutar_ciclo_de_instruccion(cliente_fd);

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
			return;
		}
		fetch(cliente_fd);

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
	log_info(logger,"Solicite instrucciones");

}


void decode(t_instruccion* instrucciones,int cliente_fd){
	t_estrucutra_cpu registro_aux;
	t_estrucutra_cpu registro_aux2;
	char * recurso ="";
	char* parametro="";
	char* parametro2="";
	uint32_t valor_uint1;
	//uint32_t valor_uint2;
	uint32_t valor_destino;
	uint32_t valor_origen;
	uint32_t resultado;
	//int valor_int;
	//log_warning(logger,"%i",instrucciones->nombre);
	switch(instrucciones->nombre){
	case SET:

		parametro2= list_get(instrucciones->parametros,1);
		parametro= list_get(instrucciones->parametros,0);
		log_info(logger,"PID: %i - Ejecutando SET: %s - %s",pcb->pid,parametro,parametro2);
		valor_uint1 = strtoul(parametro2, NULL, 10);
		registro_aux = devolver_registro(parametro);
		setear(registro_aux,valor_uint1);
		//ADormir(x segundo);
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


		    resultado = valor_destino + valor_origen;
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
    pthread_detach(conexion_memoria_hilo_cpu);
	//enviar_mensaje_instrucciones("mandame las instrucciones plz ",conexion_memoria,MANDAME_PAGINA);
}


void obtener_configuracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha_dispatch = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	puerto_escucha_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
}


void transformar_en_instrucciones(char* auxiliar){
	instruccion_a_realizar->parametros= list_create();
	int cantidad_parametros=0;
	log_error(logger,"el valor recibido es %s",auxiliar);
	char** instruccion_parseada = parsear_instruccion(auxiliar);
			log_warning(logger,"%s",instruccion_parseada[0]);
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
	        // if (strcmp(instruccion_parseada[0], "SLEEP") == 0) {
	        // 	instruccion_a_realizar->nombre = SLEEP;
	        //     cantidad_parametros = 1;
	        // }
	        if (strcmp(instruccion_parseada[0], "WAIT") == 0) {
	        	instruccion_a_realizar->nombre = WAIT;
	            cantidad_parametros = 1;
	        }
	        if (strcmp(instruccion_parseada[0], "SIGNAL") == 0) {
	        	instruccion_a_realizar->nombre = SIGNAL;
	            cantidad_parametros = 1;
	        }
	        // if (strcmp(instruccion_parseada[0], "MOV_IN") == 0) {
	        // 	instruccion_a_realizar->nombre = MOV_IN;
	        //     cantidad_parametros = 2;
	        // }
	        // if (strcmp(instruccion_parseada[0], "MOV_OUT") == 0) {
	        // 	instruccion_a_realizar->nombre = MOV_OUT;
	        //     cantidad_parametros = 2;
	        // }
	        // if (strcmp(instruccion_parseada[0], "F_OPEN") == 0) {
	        // 	instruccion_a_realizar->nombre = F_OPEN;
	        //     cantidad_parametros = 2;
	        // }
	        // if (strcmp(instruccion_parseada[0], "F_CLOSE") == 0) {
	        // 	instruccion_a_realizar->nombre = F_CLOSE;
	        //     cantidad_parametros = 1;
	        // }
	        // if (strcmp(instruccion_parseada[0], "F_SEEK") == 0) {
	        // 	instruccion_a_realizar->nombre = F_SEEK;
	        //     cantidad_parametros = 2;
	        // }
	        // if (strcmp(instruccion_parseada[0], "F_READ") == 0) {
	        // 	instruccion_a_realizar->nombre = F_READ;
	        //     cantidad_parametros = 2;
	        // }
	        // if (strcmp(instruccion_parseada[0], "F_WRITE") == 0) {
	        // 	instruccion_a_realizar->nombre = F_WRITE;
	        //     cantidad_parametros = 2;
	        // }
	        // if (strcmp(instruccion_parseada[0], "F_TRUNCATE") == 0) {
	        // 	instruccion_a_realizar->nombre = F_TRUNCATE;
	        //     cantidad_parametros = 2;
	        // }
	        if (strcmp(instruccion_parseada[0], "EXIT") == 0) {
	        	instruccion_a_realizar->nombre = EXIT;
	            cantidad_parametros = 0;
	        }
	        // if (strcmp(instruccion_parseada[0], "F_TRUNCATE") == 0) {
	        // 	instruccion_a_realizar->nombre = F_TRUNCATE;
	        //     cantidad_parametros = 2;
	        // }
						

	    	t_list* parametros = list_create();

	        for(int i=1;i<cantidad_parametros+1;i++){
				log_error(logger, "%s", instruccion_parseada[i]);
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
	} else{
		v=DI;
	}
    return v;
}


void setear(t_estrucutra_cpu pos, uint32_t valor) {

    switch(pos) {
        case AX: pcb->registros->ax = valor; break;
        case BX: pcb->registros->bx = valor; break;
        case CX: pcb->registros->cx = valor; break;
        case DX: pcb->registros->dx = valor; break;
		case EAX: pcb->registros->eax = valor; break;
		case EBX: pcb->registros->ebx = valor; break;
		case ECX: pcb->registros->ecx = valor; break;
		case EDX: pcb->registros->edx = valor; break;
		case SI: pcb->registros->si = valor; break;
		case DI: pcb->registros->di = valor; break;
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
        default: //log_info(logger, "Registro no reconocido");
        	return 0;

    }
	return valor_retorno;
}