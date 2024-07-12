#include "entradasalida.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

char* palabra_usuario;
t_list* metadatas;

int main(int argc, char* argv[]) {
    metadatas=list_create();
    logger = log_create("./entradasalida.log", "I/O", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el I/O!");
    obtener_configuracion("teclado.config");
    levantar_archivo_bloques();
    levantarBitMap();
    
    //inicializar_lista();
    //crear_archivo_metadata("fnatic");
    //crear_archivo_metadata("eurocopa");
    //crear_archivo_metadata("vamos9z");
    
    //funcion_prueba_lista();
    //truncar_archivo("eurocopa",850);
    //borrar_archivo("eurocopa");
    //escribir_archivo_bloque(64*4,"eurocopa","HAKUNA MATATA");
    
    //pthread_mutex_init(&mutex_respuesta_stdout_write, NULL);
    //pthread_mutex_lock(&mutex_respuesta_stdout_write);

	iniciar_consola();	

    return EXIT_SUCCESS;
}

void listFiles(const char *path) {

    char* ruta1 = "./dialfs/";
    int bloque_inicial_config;
    int tamanio_bloque_config;

    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    printf("Archivos en el directorio %s:\n", path);
    while ((entry = readdir(dp))) {
        // Ignorar . y ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Copiar el nombre del archivo
        char name[256];
        strncpy(name, entry->d_name, sizeof(name));
        name[sizeof(name) - 1] = '\0';  // Asegurar que la cadena esté terminada en '\0'

        // Encontrar la última ocurrencia de '.'
        char *dot = strrchr(name, '.');
        if (dot && strcmp(dot, ".txt") == 0) {
            *dot = '\0';  // Truncar la cadena en el punto para eliminar la extensión
        }

        char* extension = "txt";
        char* path_archivo = string_new();

        string_append_with_format(&path_archivo, "%s/%s.%s", path_base_dialfs, name,extension);
        log_error(logger, "%s", path_archivo);
        t_config* archivo = config_create(path_archivo);
        bloque_inicial_config =config_get_int_value(archivo, "BLOQUE_INICIAL");
        tamanio_bloque_config = config_get_int_value(archivo, "TAMANIO_ARCHIVO");

        t_metadata * nueva_metadata = malloc(sizeof(t_metadata));
        nueva_metadata->nombre=name;
        nueva_metadata->tamanio_archivo=tamanio_bloque_config;
        nueva_metadata->bloq_inicial_archivo=bloque_inicial_config;
        list_add(metadatas, nueva_metadata);

        

        log_error(logger,"el nombre del archivo es : %s", nueva_metadata->nombre);
        log_error(logger, "tamanio del archivo es : %i", nueva_metadata->tamanio_archivo);
        log_error(logger, "bloque inicial del archivo es : %i", nueva_metadata->bloq_inicial_archivo);


        log_error(logger, " ====================================================================\n");
        //nueva_metadata->nombre=name;
        //list_add(metadatas,nueva_metadata);
        printf("%s\n", name);
    }

        for(int j=0; j<list_size(metadatas); j++){
            t_metadata* metadata2 = list_get(metadatas, j);
            log_error(logger, "el nombre del archivo es : %s", metadata2->nombre);
            log_error(logger, "tamanio del archivo es : %i", metadata2->tamanio_archivo);
            log_error(logger, "bloque inicial del archivo es : %i", metadata2->bloq_inicial_archivo);
        }


}


void processFile(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Eliminar el salto de línea al final de la línea
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Dividir la línea en nombre de variable y valor
        char *name = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (name != NULL && value != NULL) {
            printf("Variable: %s, Valor: %s\n", name, value);
        }
    }

    fclose(file);
}

void obtener_configuracion_metadata(char *path_configuration){
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


void iniciar_consola(){ 
	t_log * logger_consola = log_create("./entradaSalidaConsole.log", "consola", 1, LOG_LEVEL_INFO);
	char* path_configuracion;

        log_info(logger_consola,"Ingrese el nombre de la interfaz");
        interfaz_name = readline(">");
        interfaz_name = strtok(interfaz_name, "\n");
        if(strcmp(interfaz_name, "GENERICA")==0){
            path_configuracion = "generica.config";
            obtener_configuracion(path_configuracion);

            iniciar_interfaz_generica();
        }
        log_error(logger_consola,"Interfaz generica llegue hasta aca");
        if(strcmp(interfaz_name, "MONITOR")==0){
            path_configuracion = "monitor.config";
            obtener_configuracion(path_configuracion);
            log_error(logger_consola,"Interfaz generica llegue hasta aca");
            iniciar_interfaz_stdout();
        }
        if (strcmp(interfaz_name, "FS") == 0)
        {
            path_configuracion = "fs.config";
            obtener_configuracion(path_configuracion);
            iniciar_interfaz_fs();
        }
  

        if(strcmp(interfaz_name, "TECLADO")==0){
            path_configuracion = "teclado.config";
            obtener_configuracion(path_configuracion);
            iniciar_interfaz_stdin();
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

void  iniciar_interfaz_fs() {
    log_info(logger, "Ingreso a la interfaz FS");
    listFiles("./dialfs");
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
    t_paquete* paquetre2 = crear_paquete(CONEXION_INTERFAZ);
    char* nombre_interfaz = strtok(interfaz_name, "\n");
    char* tipo_interfaz_usado = strtok(tipo_interfaz, "\n");
    log_error(logger, "Interfaz: %s tipo: %s", nombre_interfaz, tipo_interfaz_usado);
    agregar_a_paquete(paquetre2, interfaz_name, strlen(interfaz_name)+1);
	agregar_a_paquete(paquetre2, tipo_interfaz_usado, strlen(tipo_interfaz_usado)+1);
    
	enviar_paquete(paquetre2, cliente_fd);
    log_error(logger, "ya envie a kernel");
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
			int *amount = list_get(paquete, 1);
            log_info(logger, "PID: < %i > - Operacion: < IO_STDIN_READ >",*pid);
			usleep(*amount * tiempo_unidad_trabajo*10000);

            t_paquete* paquete_finalizar_sleep = crear_paquete(EJECUTAR_IO_SLEEP);
            agregar_a_paquete(paquete_finalizar_sleep, pid, sizeof(int));
            agregar_a_paquete(paquete_finalizar_sleep, nombre_interfaz, strlen(nombre_interfaz)+1);

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
        	log_info(logger, "PID: < %i > - Operacion: < IO_STDIN_READ >",*pid_stdin);
            //log_warning(logger,"el tamanio es %i",*tamanio_stdin);
        	conexion_memoria= crear_conexion(ip_memoria, puerto_memoria);

            
        	char* palabra_A_enviar = string_substring(palabra_usuario,0,*tamanio_stdin);
            void* palabra_stdin_read =(void*) palabra_A_enviar;
            //log_info(logger, "envio la palabra: %s",palabra_A_enviar);
            enviar_stdin_memoria(*pid_stdin,*marco_stdin,*desplazamiento_stdin,*tamanio_stdin,palabra_stdin_read,conexion_memoria,EJECUTAR_STDIN_READ);//pid_stdin es el PID para el log de memoria cuando escribe.
        	free(palabra_stdin_read);
            int cop;
			recv(conexion_memoria, &cop, sizeof(cop), 0);//tendria que recibir un ok de memoria?
            //log_info(logger_consola,"me llego confirmacion de Memoria, llego un %i",cop);//1=OK
            
			close(conexion_memoria);
        	//sem_wait(&sem_cont_lectura);
        	enviar_kernel_ok_stdin(cliente_fd,*pid_stdin,nombre_interfaz);//aviso q termine accion.

        	break;
        case EJECUTAR_STDOUT_WRITE:
        	paquete = recibir_paquete(cliente_fd);
         	int *pid_stdout = list_get(paquete, 0);
            int *marco_stdout = list_get(paquete, 1);
            int *desplazamiento_stdout = list_get(paquete, 2);
        	int *tamanio_stdout = list_get(paquete, 3);
            //log_warning(logger,"RECIBI PID %i, marco %i, desplamiento %i, tamanio %i",*pid_stdout, *marco_stdout ,*desplazamiento_stdout,*tamanio_stdout);
        	conexion_memoria= crear_conexion(ip_memoria, puerto_memoria);
            enviar_direccion_memoria(*pid_stdout,*marco_stdout,*desplazamiento_stdout,*tamanio_stdout,conexion_memoria,EJECUTAR_STDOUT_WRITE);//agrego pid por si memoria lo necesita para sus logs en acceso de lectura.
            void* informacion = malloc(*tamanio_stdout);
            int cop2;
            recv(conexion_memoria, &cop2, sizeof(cop), 0);
			//log_info(logger,"recibi el codigo de operacion %i",cop2);
			t_list* lista2=list_create();
			//log_error(logger,"el codigo socket es %i",cliente_fd);
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
            //log_warning(logger,"el pid es %i",*pid_stdout);
            enviar_kernel_ok_stdout(cliente_fd,*pid_stdout,nombre_interfaz);
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
        	paquete=recibir_paquete(cliente_fd);
            char* nombre_archivo=list_get(paquete,0);
            int* pid_f_create = list_get(paquete,1);


			log_info(logger, "PID: %i Crear Archivo: <%s>", *pid_f_create, nombre_archivo);
			crear_archivo_metadata(nombre_archivo);
            enviar_respuesta_crear_archivo(cliente_fd,*pid_f_create,nombre_interfaz);
        	break;
        case EJECUTAR_IO_FS_DELETE:
        	paquete=recibir_paquete(cliente_fd);
            char* nombre_arch=list_get(paquete,0);
            int* pid_f_delete = list_get(paquete,1);

			log_info(logger, "Crear Archivo: <%s>",nombre_arch);
			borrar_archivo(nombre_arch);
            enviar_respuesta_borrar_archivo(cliente_fd,*pid_f_delete,nombre_interfaz);
        	break;
        case EJECUTAR_IO_FS_TRUNCATE:
            paquete=recibir_paquete(cliente_fd);
            char* nombre_archivo_truncar = list_get(paquete,0);
            int* tamanio_truncar = list_get(paquete,1);
            int* pid_f_truncate = list_get(paquete,2);
            log_info(logger, "Truncar Archivo: <%s>",nombre_archivo_truncar);
            log_warning(logger,"el tamanio es %i",*tamanio_truncar);
            truncar_archivo(nombre_archivo_truncar,*tamanio_truncar);
            enviar_respuesta_truncar_archivo(cliente_fd,*pid_f_truncate,nombre_interfaz);
            break;
        case EJECUTAR_IO_FS_WRITE:
            paquete = recibir_paquete(cliente_fd);
            char* nomre_archivo_write = list_get(paquete,0);
            int* marco_write = list_get(paquete,1);
            int* desplazamiento_write = list_get(paquete,2);
            int* tamanio_write = list_get(paquete,3);
            int* puntero_write = list_get(paquete,4);
            int* pid_f_write = list_get(paquete,5);
            log_warning(logger,"el puntero es %i",*puntero_write);
            conexion_memoria= crear_conexion(ip_memoria, puerto_memoria);
            enviar_direccion_memoria(*pid_f_write,*marco_write,*desplazamiento_write,*tamanio_write,conexion_memoria,EJECUTAR_IO_FS_WRITE);//agrego pid por si memoria lo necesita para sus logs en acceso de lectura.
           // void* informacion = malloc(*tamanio_write);
            int cop4;
            recv(conexion_memoria, &cop4, sizeof(cop), 0);
			log_info(logger,"recibi el codigo de operacion %i",cop2);
			t_list* lista3=list_create();
			log_error(logger,"el codigo socket es %i",cliente_fd);
			lista3= recibir_paquete(conexion_memoria);
			void* auxiliar_10 = list_get(lista3,0);
			//paquete2 = recibir_paquete(conexion_memoria);
            //void* informacion = malloc(R_tamanio);
            //pthread_mutex_lock(&mutex_respuesta_stdout_write);
            //log_warning(logger,"PASEEEEEEEE");//recibe la informacion, no es necesario empaquetar porque ya sabemos el tamanio?
			//usleep(tiempo_unidad_trabajo*1000);
            close(conexion_memoria);
            log_warning(logger,"el puntero es %i",*puntero_write);
            escribir_archivo_bloque(*puntero_write,nomre_archivo_write,*tamanio_write,auxiliar_10);

            enviar_respuesta_escribir_archivo(cliente_fd,*pid_f_write,nombre_interfaz);
            break;
        case EJECUTAR_IO_FS_READ:
            paquete = recibir_paquete(cliente_fd);
            char* nomre_archivo_read = list_get(paquete,0);
            int* marco_read = list_get(paquete,1);
            int* desplazamiento_read = list_get(paquete,2);
            int* tamanio_read = list_get(paquete,3);
            int* puntero_read = list_get(paquete,4);
            int* pid_f_read = list_get(paquete,5);
            
            void* auxiliar_read = leer_archivo_bloque(*puntero_read,nomre_archivo_read,*tamanio_read);

            conexion_memoria= crear_conexion(ip_memoria, puerto_memoria);
            enviar_stdin_memoria(*pid_f_read,*marco_read,*desplazamiento_read,*tamanio_read,auxiliar_read,conexion_memoria,EJECUTAR_IO_FS_READ);//pid_stdin es el PID para el log de memoria cuando escribe.
            //void* informacion = malloc(*tamanio_read);
            free(auxiliar_read);
            int cop3;
            recv(conexion_memoria, &cop3, sizeof(cop), 0);
			//paquete2 = recibir_paquete(conexion_memoria);
            //void* informacion = malloc(R_tamanio);
            //pthread_mutex_lock(&mutex_respuesta_stdout_read);
            //log_warning(logger,"PASEEEEEEEE");//recibe la informacion, no es necesario empaquetar porque ya sabemos el tamanio?
			//usleep(tiempo_unidad_trabajo*1000);
            close(conexion_memoria);
            enviar_respuesta_leer_archivo(cliente_fd,*pid_f_read,nombre_interfaz);
            break;

        }
    }
}

void enviar_stdin_memoria(int pid, int marco, int desplazamiento, int tamanio,void* palabra_A_enviar, int conexion_memoria ,op_code codigo_op){//Se agrega PID para el log de memoria cuando escribe.
	t_paquete* paquete=crear_paquete(codigo_op);
    agregar_a_paquete(paquete, &pid, sizeof(int));
	agregar_a_paquete(paquete,&marco,sizeof(int));
    agregar_a_paquete(paquete,&desplazamiento,sizeof(int));
    agregar_a_paquete(paquete,&tamanio,sizeof(int));
	agregar_a_paquete(paquete,palabra_A_enviar,tamanio);//+1 para agregar centinela pero no lo quiero
	enviar_paquete(paquete, conexion_memoria);
    //log_warning(logger,"el palabra que envie es %s",palabra_A_enviar);
	eliminar_paquete(paquete);
}

void enviar_direccion_memoria(int pid, int marco, int desplazamiento, int tamanio, int conexion_memoria , op_code codigo_op){
	t_paquete* paquete=crear_paquete(codigo_op);
	agregar_a_paquete(paquete, &pid, sizeof(int));
	agregar_a_paquete(paquete,&marco,sizeof(int));
    agregar_a_paquete(paquete,&desplazamiento,sizeof(int));
    agregar_a_paquete(paquete,&tamanio,sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
}

void enviar_kernel_ok_stdin(int cliente_fd,int pid,char *nombre_interfaz){
	t_paquete *paquete = crear_paquete(EJECUTAR_STDIN_READ);//agregar a utils..
	agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_kernel_ok_stdout(int cliente_fd, int pid,char *nombre_interfaz){
	t_paquete *paquete = crear_paquete(EJECUTAR_STDOUT_WRITE);
	agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_respuesta_crear_archivo(int cliente_fd,int pid,char *nombre_interfaz){
	t_paquete *paquete = crear_paquete(RESPUESTA_CREAR_ARCHIVO);
	agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
	enviar_paquete(paquete,cliente_fd);
    log_warning(logger, "Se ha creado el archivo con PID: %d y fue enviado CON EL CODIGO DE CLIENTE: %i", pid, cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_respuesta_borrar_archivo(int cliente_fd,int pid,char *nombre_interfaz){
    t_paquete *paquete = crear_paquete(RESPUESTA_BORRAR_ARCHIVO);
	agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
	enviar_paquete(paquete,cliente_fd);
	eliminar_paquete(paquete);
}

void enviar_respuesta_escribir_archivo(int cliente_fd,int pid,char *nombre_interfaz){
    t_paquete *paquete = crear_paquete(RESPUESTA_ESCRIBIR_ARCHIVO);
    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
    enviar_paquete(paquete,cliente_fd);
    eliminar_paquete(paquete);
}

void enviar_respuesta_leer_archivo(int cliente_fd,int pid, char* nombre_interfaz){
    t_paquete *paquete = crear_paquete(RESPUESTA_LEER_ARCHIVO);
    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
    enviar_paquete(paquete,cliente_fd);
    eliminar_paquete(paquete);
}

void  enviar_respuesta_truncar_archivo(int cliente_fd,int pid_f_truncate,char* nombre_interfaz){

    t_paquete *paquete = crear_paquete(RESPUESTA_TRUNCAR_ARCHIVO);
    agregar_a_paquete(paquete, &pid_f_truncate, sizeof(int));
    agregar_a_paquete(paquete, nombre_interfaz, strlen(nombre_interfaz)+1);
    enviar_paquete(paquete,cliente_fd);
    eliminar_paquete(paquete);
}