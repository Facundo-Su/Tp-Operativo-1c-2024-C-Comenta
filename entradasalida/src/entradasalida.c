#include "entradasalida.h"

int main(int argc, char* argv[]) {


	char *rutaConfig = "entradasalida.config";
	config = cargar_config(rutaConfig);

    logger = log_create("./entradasalida.log", "I/O", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el I/O!");
    obtener_configuracion();
	generar_conexion();

    // iniciar_consola();

    return EXIT_SUCCESS;
}


void iniciar_consola(){
	t_log * logger_consola = log_create("./entradaSalidaConsole.log", "consola", 1, LOG_LEVEL_INFO);
	char* variable;
	while(1){
		log_info(
            logger_consola,
            "ingrese la operacion que deseas realizar"
            "\n 1. Iniciar Conexion con memoria"
            "\n 2. Iniciar Conexion con kernel"
        );
		variable = readline(">");

        switch (*variable) {
			case '1':
                log_info(logger_consola,"conexion iniciada con memoria");
				enviar_mensaje("hola",conexion_memoria);
				break;
            case '2':
                log_info(logger_consola,"conexion iniciada con kernel");
                enviar_mensaje("hola",conexion_kernel);
            default:
                log_info(logger_consola,"Operacion no reconocida");
        };
	}

}

void obtener_configuracion(){

    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config, "BLOCK_SIZE");
    block_count = config_get_int_value(config, "BLOCK_COUNT");
}

void generar_conexion(){
	pthread_t conexion_memoria_hilo;
    pthread_t conexion_kernel_hilo;

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	pthread_create(&conexion_memoria_hilo,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
	pthread_detach(conexion_memoria_hilo);

    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);
	pthread_create(&conexion_kernel_hilo,NULL,(void*) procesar_conexion,(void *)&conexion_kernel);
	pthread_detach(conexion_kernel_hilo);
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
				log_error(logger, "che no se que me mandaste");
				break;
			}
			break;
		case FINALIZAR:

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