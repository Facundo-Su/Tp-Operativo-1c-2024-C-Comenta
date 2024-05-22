#include "entradasalida.h"

int main(int argc, char* argv[]) {
    logger = log_create("./entradasalida.log", "I/O", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el I/O!");
	iniciar_consola();	

    return EXIT_SUCCESS;
}


void iniciar_consola(){ 
	t_log * logger_consola = log_create("./entradaSalidaConsole.log", "consola", 1, LOG_LEVEL_INFO);
	char* path_configuracion;

    while (1)
    {
        log_info(logger_consola,"Ingrese el nombre de la interfaz");
        interfaz_name = readline(">");
        log_info(logger_consola,"Ingrese la ubicacion del archivo de configuracion");
        path_configuracion = readline(">");
        obtener_configuracion(path_configuracion);

        if (strcmp(tipo_interfaz, "GENERICA") == 0) 
        {
            iniciar_interfaz_generica();
        }
        else if (strcmp(tipo_interfaz, "STDIN") == 0)
        {
            log_error(logger_consola, "no tan rapido vaquero, eso es para la entrega 3");
        }
        else
        {
            log_error(logger_consola,"Error interfaz: no conocida");
        }
    }
    
}

void iniciar_interfaz_generica() {
    log_info(logger, "Ingreso a la interfaz GENERICA");
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

void generar_conexion_con_kernel(){
    pthread_t conexion_kernel_hilo;
	
    // * info: este seria mi socket_cliente. seria el cliente_fd
    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);


    pthread_create(&conexion_kernel_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_kernel);
    pthread_detach(conexion_kernel_hilo);
}


void procesar_conexion(void *conexion_ptr){
    int conexion =  *(int *)conexion_ptr;
	int cliente_fd = conexion;
	t_contexto_ejecucion * contexto;
	t_paquete * paquete;

    t_paquete* paquete2 = crear_paquete(CONEXION_INTERFAZ);
    agregar_a_paquete(paquete2, interfaz_name, sizeof(char*));
	agregar_a_paquete(paquete2, tipo_interfaz, sizeof(char*));
	enviar_paquete(paquete2, cliente_fd);

    while (1) {
        int cod_op = recibir_operacion(conexion);
        switch (cod_op) {
        case MENSAJE:
            recibir_mensaje(conexion);
            break;
        case EJECUTAR_IO_SLEEP:
			paquete = recibir_paquete(cliente_fd);
            int *pid = list_get(paquete, 0);
			int *amount = list_get(paquete, 1);
			sleep(*amount * tiempo_unidad_trabajo);

            t_paquete* paquete_finalizar_sleep = crear_paquete(EJECUTAR_IO_SLEEP);
            agregar_a_paquete(paquete_finalizar_sleep, &pid, sizeof(int));
            enviar_paquete(paquete_finalizar_sleep, cliente_fd);
        case -1:
            log_error(logger, "Ocurrio un error al conectarse al servidor.");
            return NULL;
        default:
            log_warning(logger, "Operación desconocida. Revisar el protocolo de comunicación.");
            break;
        }
    }
    return NULL;
}
