#include <utils/hello.h>


int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);
	// Ahora que tenemos el socket, vamos a conectarlo
	if (setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	    error("setsockopt(SO_REUSEADDR) failed");
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	//paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void enviar_mensaje_instrucciones(char* mensaje, int socket_cliente,op_code operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = operacion;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);

	eliminar_paquete(paquete);
}

// void enviar_interrupciones(int socket_cliente,op_code operacion)
// {
// 	t_paquete* paquete = crear_paquete(operacion);

// 	enviar_paquete(paquete, socket_cliente);
// 	eliminar_paquete(paquete);
// }

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_code cod_op)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = cod_op;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}



int iniciar_servidor(char *puerto)
{

	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
	// Asociamos el socket a un puerto


	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");
	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{

	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}
char* recibir_instruccion(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	return buffer;
}
char* obtener_mensaje(int socket_cliente)
{

	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	//log_info(logger, "Me llego el mensaje %s", buffer);
	return buffer;
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

t_config* cargar_config(char *ruta){
	t_config* config = config_create(ruta);
    if (config == NULL) {
        printf("No se encontró el archivo de configuración");
         exit(1);
    }
    return config;
}

//------------------------------------------------------------------------------------------------------------

void empaquetar_pcb(t_paquete* paquete, t_contexto_ejecucion* contexto){

	agregar_a_paquete(paquete, &(contexto->pid), sizeof(int));
	agregar_a_paquete(paquete, &(contexto->pc), sizeof(int));
	empaquetar_registro(paquete, contexto->registros);
}

void empaquetar_registro(t_paquete* paquete, t_registros_pcb* registros){
	agregar_a_paquete(paquete,&(registros->ax), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->bx), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->cx), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->dx), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->eax), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->ebx), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->ecx), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->edx), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->si), sizeof(uint32_t));
	agregar_a_paquete(paquete,&(registros->di), sizeof(uint32_t));
}
//==========================================================================================================

t_contexto_ejecucion* desempaquetar_pcb(t_list* paquete){

	t_contexto_ejecucion* contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
	int * pid = list_get(paquete, 0);
	contexto_ejecucion->pid = *pid;
	int posicion = 1;
	int *puntero_posicion = &posicion;
	int *pc = list_get(paquete, (*puntero_posicion)++);
	contexto_ejecucion->pc = *pc;
	t_registros_pcb* registros = desempaquetar_registros(paquete, puntero_posicion);
	contexto_ejecucion->registros = registros;
	return contexto_ejecucion;
}

t_registros_pcb* desempaquetar_registros(t_list* paquete, int* posicion) {
    t_registros_pcb* registro = malloc(sizeof(t_registros_pcb));

    registro->ax = *((uint32_t*)list_get(paquete, (*posicion)++));
    registro->bx = *((uint32_t*)list_get(paquete, (*posicion)++));
    registro->cx = *((uint32_t*)list_get(paquete, (*posicion)++));
    registro->dx = *((uint32_t*)list_get(paquete, (*posicion)++));
	registro->eax = *((uint32_t*)list_get(paquete, (*posicion)++));
	registro->ebx = *((uint32_t*)list_get(paquete, (*posicion)++));
	registro->ecx = *((uint32_t*)list_get(paquete, (*posicion)++));
	registro->edx = *((uint32_t*)list_get(paquete, (*posicion)++));
	registro->si = *((uint32_t*)list_get(paquete, (*posicion)++));
	registro->di = *((uint32_t*)list_get(paquete, (*posicion)++));
    return registro;
}

void enviar_pcb(t_contexto_ejecucion* pcb, int conexion,op_code operacion){
	t_paquete* paquete = crear_paquete(operacion);
	empaquetar_pcb(paquete, pcb);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}
//mostrar el contexto_ejecucion
void mostrar_contexto_ejecucion(t_contexto_ejecucion* contexto_ejecucion){
	log_info(logger, "PID: %d", contexto_ejecucion->pid);
	log_info(logger, "PC: %d", contexto_ejecucion->pc);
	log_info(logger, "AX: %d", contexto_ejecucion->registros->ax);

}