#include "entradasalida.h"


char* palabra_usuario;

int main(int argc, char* argv[]) {
    logger = log_create("./entradasalida.log", "I/O", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el I/O!");
    pthread_mutex_init(&mutex_respuesta_stdout_write, NULL);
    pthread_mutex_lock(&mutex_respuesta_stdout_write);

	iniciar_consola();	

    return EXIT_SUCCESS;
}


void iniciar_consola(){ 
	t_log * logger_consola = log_create("./entradaSalidaConsole.log", "consola", 1, LOG_LEVEL_INFO);
	char* path_configuracion;

        log_info(logger_consola,"Ingrese el nombre de la interfaz");
        interfaz_name = readline(">");
        interfaz_name = strtok(interfaz_name, "\n");
        if(strcmp(interfaz_name, "GENERICA")){
            path_configuracion = "generica.config";
            obtener_configuracion(path_configuracion);
            iniciar_interfaz_generica();
        }else if(strcmp(interfaz_name, "MONITOR")){
            path_configuracion = "monitor.config";
            obtener_configuracion(path_configuracion);
            iniciar_interfaz_stdout();
        }
        else if(strcmp(interfaz_name, "TECLADO")){
            path_configuracion = "teclado.config";
            obtener_configuracion(path_configuracion);
            iniciar_interfaz_stdin();
        }else{
            log_error(logger_consola,"Interfaz desconocidad");
        }
  

        /*
        log_info(logger_consola,"Ingrese la ubicacion del archivo de configuracion");
        path_configuracion = readline(">");
        //path_configuracion = "entradasalida.config";//sacar esto despues
        obtener_configuracion(path_configuracion);//que pasa si las configs no son iguales?
        


        if (strcmp(tipo_interfaz, "GENERICA") == 0) 
        {
            iniciar_interfaz_generica();
        }
        else if (strcmp(tipo_interfaz, "STDIN") == 0)
        {


        	iniciar_interfaz_stdin();
            //log_error(logger_consola, "no tan rapido vaquero, eso es para la entrega 3");
        }
        else if (strcmp(tipo_interfaz, "STDOUT") == 0)
        {
            //log_error(logger_consola,"Error interfaz: no conocida");
            iniciar_interfaz_stdout();
        }
        else if (strcmp(tipo_interfaz, "DIALFS") == 0)
        {
            log_error(logger_consola,"Para la ultima entrega");
            //iniciar_interfaz_dialfs();
        }else{

        	log_error(logger_consola,"Interfaz desconocidad");
        }*/
    
}

void iniciar_interfaz_generica() {
    log_info(logger, "Ingreso a la interfaz GENERICA");
	generar_conexion_con_kernel();
}
void iniciar_interfaz_stdin() {
    log_info(logger, "Ingreso a la interfaz STDIN");
	generar_conexion_con_kernel();
    
}
void iniciar_interfaz_stdout() {
    log_info(logger, "Ingreso a la interfaz STDOUT");
	generar_conexion_con_kernel();
}

void obtener_configuracion(char *path_configuration){
	config = cargar_config(path_configuration);
    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config, "BLOCK_SIZE");
    block_count = config_get_int_value(config, "BLOCK_COUNT");
	retraso_compactacion = config_get_int_value(config, "RETRASO_COMPACTACION");
}
/*
void obtener_configuracion_STDIN(char *path_configuration){
    config = cargar_config(path_configuration);
    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");    
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");  
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
}
void obtener_configuracion_STDOUT(char *path_configuration){
    config = cargar_config(path_configuration);
    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA"); 
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

}*/

void generar_conexion_con_kernel(){

	
    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);


    pthread_create(&conexion_kernel_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_kernel);
    //pthread_detach(conexion_kernel_hilo);
    pthread_join(conexion_kernel_hilo, NULL);
}

/*void generar_conexion_con_memoria(){
    pthread_t conexion_memoria_hilo;
	
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);


    pthread_create(&conexion_memoria_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_memoria);
    pthread_detach(conexion_memoria_hilo);
}*/

void procesar_conexion(void *conexion_ptr){
    int conexion =  *(int *)conexion_ptr;
	int cliente_fd = conexion;
	t_contexto_ejecucion * contexto;
	t_paquete * paquete;

    t_paquete* paquete2 = crear_paquete(CONEXION_INTERFAZ);
    agregar_a_paquete(paquete2, interfaz_name, strlen(interfaz_name)+1);
	agregar_a_paquete(paquete2, tipo_interfaz, strlen(interfaz_name)+1);
	enviar_paquete(paquete2, cliente_fd);

    while (1) {
        int cod_op = recibir_operacion(conexion);
        //log_warning(logger, "recibi una instruccion %i",cod_op);
        switch (cod_op) {
        case MENSAJE:
            recibir_mensaje(conexion);
            break;
        case EJECUTAR_IO_SLEEP:
			paquete = recibir_paquete(cliente_fd);
            int *pid = list_get(paquete, 0);
            log_error(logger, "%i",*pid);
			int *amount = list_get(paquete, 1);
			usleep(*amount * tiempo_unidad_trabajo*10000);

            t_paquete* paquete_finalizar_sleep = crear_paquete(EJECUTAR_IO_SLEEP);
            agregar_a_paquete(paquete_finalizar_sleep, pid, sizeof(int));

            enviar_paquete(paquete_finalizar_sleep, conexion_kernel);
            log_warning(logger, "ya envie a kernel con el pid %i",*pid);
            break;
        case EJECUTAR_STDIN_READ:
    
            //Recibo a travez de una lista todo lo que me envia KERNEL.
        	paquete = recibir_paquete(cliente_fd);
        	int *pid_stdin = list_get(paquete, 0);
            int *marco_stdin = list_get(paquete, 1);
            int *desplazamiento_stdin = list_get(paquete, 2);
        	int *tamanio_stdin = list_get(paquete, 3);
        	log_info(logger,"Ingrese una palabra por teclado:");
            palabra_usuario= readline(">");
        	//log_info(logger_consola	, "PID: < %i > - Operacion: < IO_STDIN_READ >",*pid_stdin);//esta bien que la operacion sea asi?
            log_warning(logger,"el tamanio es %i",*tamanio_stdin);
        	conexion_memoria= crear_conexion(ip_memoria, puerto_memoria);

            
        	char* palabra_A_enviar = string_substring_until(palabra_usuario,*tamanio_stdin);// de las commons,ajusto la palabra 
            log_info(logger, "envio la palabra: %s",palabra_A_enviar);
            enviar_stdin_memoria(*pid_stdin,*marco_stdin,*desplazamiento_stdin,*tamanio_stdin,palabra_A_enviar,conexion_memoria);//pid_stdin es el PID para el log de memoria cuando escribe.
        	int cop;
			recv(conexion_memoria, &cop, sizeof(cop), 0);//tendria que recibir un ok de memoria?
            //log_info(logger_consola,"me llego confirmacion de Memoria, llego un %i",cop);//1=OK
            
			close(conexion_memoria);
        	//sem_wait(&sem_cont_lectura);
        	enviar_kernel_ok_stdin(cliente_fd,*pid_stdin);//aviso q termine accion.

        	break;
        case EJECUTAR_STDOUT_WRITE:
        	paquete = recibir_paquete(cliente_fd);
         	int *pid_stdout = list_get(paquete, 0);
            int *marco_stdout = list_get(paquete, 1);
            int *desplazamiento_stdout = list_get(paquete, 2);
        	int *tamanio_stdout = list_get(paquete, 3);
            log_warning(logger,"RECIBI PID %i, marco %i, desplamiento %i, tamanio %i",*pid_stdout, *marco_stdout ,*desplazamiento_stdout,*tamanio_stdout);
        	conexion_memoria= crear_conexion(ip_memoria, puerto_memoria);
            enviar_direccion_memoria(*pid_stdout,*marco_stdout,*desplazamiento_stdout,*tamanio_stdout,conexion_memoria);//agrego pid por si memoria lo necesita para sus logs en acceso de lectura.
            void* informacion = malloc(*tamanio_stdout);
            int cop2;
            recv(conexion_memoria, &cop2, sizeof(cop), 0);
			log_info(logger,"recibi el codigo de operacion %i",cop2);
			t_list* lista2=list_create();
			log_error(logger,"el codigo socket es %i",cliente_fd);
			lista2= recibir_paquete(conexion_memoria);
			void* auxiliar = list_get(lista2,0);
			//paquete2 = recibir_paquete(conexion_memoria);
            //void* informacion = malloc(R_tamanio);
            //pthread_mutex_lock(&mutex_respuesta_stdout_write);
            //log_warning(logger,"PASEEEEEEEE");//recibe la informacion, no es necesario empaquetar porque ya sabemos el tamanio?
			//usleep(tiempo_unidad_trabajo*1000);
            close(conexion_memoria);
			log_info(logger	, "El resultado de lo buscado en memoria es: < %s >",auxiliar);
			//log_info(logger_consola	, "PID: < %i > - Operacion: < IO_STDOUT_WRITE >",*pid);
            log_warning(logger,"el pid es %i",*pid_stdout);
            enviar_kernel_ok_stdout(cliente_fd,*pid_stdout);
            free(informacion);//aviso q termine accio
        	break;
       /*case RESPUESTA_STDOUT_WRITE:
            log_warning(logger,"llegueeeeeee");
            paquete = recibir_paquete(cliente_fd);
            informacion = list_get(paquete, 0);
            log_warning(logger,"recibi EL DATO  %s",informacion);
            pthread_mutex_unlock(&mutex_respuesta_stdout_write);
            break;*/
        case EJECUTAR_IO_FS_CREATE:
        	//TO-DO
        	break;
        case EJECUTAR_IO_FS_DELETE:
        	//TO-DO
        	break;
        case EJECUTAR_IO_FS_TRUNCATE:
        	//TO-DO
        	break;
        case EJECUTAR_IO_FS_WRITE:
        	//TO-DO
        	break;
        case EJECUTAR_IO_FS_READ:
        	//TO-DO
        	break;
            
        default:
            //(logger, "Operación desconocida. Revisar el protocolo de comunicación.");
            break;
        }
    }

}


void enviar_stdin_memoria(int pid, int marco, int desplazamiento, int tamanio,char* palabra_A_enviar, int conexion_memoria ){//Se agrega PID para el log de memoria cuando escribe.
	t_paquete* paquete=crear_paquete(EJECUTAR_STDIN_READ);
    agregar_a_paquete(paquete, &pid, sizeof(int));
	agregar_a_paquete(paquete,&marco,sizeof(int));
    agregar_a_paquete(paquete,&desplazamiento,sizeof(int));
    agregar_a_paquete(paquete,&tamanio,sizeof(int));
	agregar_a_paquete(paquete,palabra_A_enviar,strlen(palabra_A_enviar)+1);//+1 para agregar centinela pero no lo quiero
	enviar_paquete(paquete, conexion_memoria);
    log_warning(logger,"el palabra que envie es %s",palabra_A_enviar);
	eliminar_paquete(paquete);
}

void enviar_direccion_memoria(int pid, int marco, int desplazamiento, int tamanio, int conexion_memoria ){
	t_paquete* paquete=crear_paquete(EJECUTAR_STDOUT_WRITE);
	agregar_a_paquete(paquete, &pid, sizeof(int));
	agregar_a_paquete(paquete,&marco,sizeof(int));
    agregar_a_paquete(paquete,&desplazamiento,sizeof(int));
    agregar_a_paquete(paquete,&tamanio,sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}

void enviar_kernel_ok_stdin(int cliente_fd,int pid){
	t_paquete *paquete = crear_paquete(EJECUTAR_STDIN_READ);//agregar a utils..
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_kernel_ok_stdout(int cliente_fd, int pid){
	t_paquete *paquete = crear_paquete(EJECUTAR_STDOUT_WRITE);
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}