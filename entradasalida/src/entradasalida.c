#include "entradasalida.h"

int main(int argc, char* argv[]) {


	char *rutaConfig = "entradasalida.config";
	config = cargar_config(rutaConfig);

    logger = log_create("./entradasalida.log", "I/O", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el I/O!");
    obtener_configuracion();
	iniciar_consola();

    return EXIT_SUCCESS;
}


void iniciar_consola(){ 
	t_log * logger_consola = log_create("./entradaSalidaConsole.log", "consola", 1, LOG_LEVEL_INFO);
	char* interfaz;
	char* path_configuracion;

	log_info(
		logger_consola,
		"Seleccione el tipo de interfaz"
		"\n 1. Generica"
		"\n 2. STDIN"
		"\n 3. STDOUT"
		"\n 4 DialFS"
		"\n 5 enviar mensaje a kernel"
	);
	interfaz = readline(">");
	log_info(logger_consola,"Ingrese la ubicacion del archivo de configuracion");
	path_configuracion = readline(">");

	switch (*interfaz)
	{
		case '1':
			iniciar_interfaz_generica();
			break;
		case '2':
			iniciar_interfaz_stdin();
			break;
		case '3':
			iniciar_interfaz_stdout();
			break;
		case '4':
			iniciar_interfaz_dialfs();
			break;
		case '5':
			generar_conexion();
			enviar_mensaje_instrucciones("hola soy entrada salida",conexion_kernel,MENSAJE);
			break;
		default:
			log_error(logger_consola,"Error interfaz: no conocida");
			break;
	}
}

void iniciar_interfaz_generica() {
    log_info(logger, "Ingreso a la interfaz Generica");
	int interfaz_fd = iniciar_servidor(8005);

	while (1) {
        int cliente_fd = esperar_cliente(interfaz_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		pthread_detach(atendiendo);

    }
}

void iniciar_interfaz_stdin() {
    log_info(logger, "Ingreso a la interfaz STDIN");
	generar_conexion();
}

void iniciar_interfaz_stdout() {
    log_info(logger, "Ingreso a la interfaz STDOUT");
	generar_conexion();
}

void iniciar_interfaz_dialfs() {
    log_info(logger, "Ingreso a la interfaz DialFs");
	generar_conexion();
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
	retraso_compactacion = config_get_int_value(config, "RETRASO_COMPACTACION");
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