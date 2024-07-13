#include "global.h"

sem_t sem_new;
sem_t sem_ready;
sem_t sem_grado_multiprogramacion;
pthread_mutex_t sem_detener;
pthread_mutex_t sem_exec;
pthread_mutex_t sem_detener_largo;
pthread_mutex_t sem_interrupcion;
pthread_mutex_t sem_detener_conexion;
pthread_mutex_t sem_vrr;
pthread_mutex_t sem_quantum;
pthread_mutex_t sem_lista_bloqueado_interfaces;
pthread_mutex_t sem_memoria;


t_cola * cola_new;
t_cola * cola_ready;
t_cola * cola_blocked;
t_cola * cola_vrr;
t_cola * cola_rr;

t_list* lista_interfaces;
t_list* lista_bloqueado_io;

int quantum;
int grado_multiprogramacion_ini;
int contador_pcb;
char ** recursos_config;
char ** instancias_recursos_config;
int conexion_memoria;
int conexion_cpu;
int conexion_cpu_interrupt;
char *puerto_escucha;
bool detener;
bool primero;
bool sigue;
t_list * lista_recursos;
t_list * lista_recursos_pcb;
t_planificador planificador;
pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;
pthread_t hilo_conexion_memoria;
pthread_t hilo_conexion_cpu;
pthread_t hilo_conexion_cpu_interrupt;
t_temporal* inicio_vrr;
int contador_aux;

t_config* config;
t_log* logger;
t_pcb * running;
void obtener_configuracion(){
    char *ruta_config = "kernel.config";
	config = cargar_config(ruta_config);
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    char * ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    char * puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    char * ip_cpu = config_get_string_value(config, "IP_CPU");
    char * puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    char * puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    char * algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    quantum = config_get_int_value(config, "QUANTUM");
    recursos_config = config_get_array_value(config, "RECURSOS");
    instancias_recursos_config = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion_ini = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    asignar_algoritmo(algoritmo);
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    conexion_cpu = crear_conexion(ip_cpu, puerto_cpu_dispatch);
    conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);

}
void asignar_algoritmo(char *algoritmo){
	if (strcmp(algoritmo, "FIFO") == 0) {
		planificador = FIFO;
	} else if (strcmp(algoritmo, "RR") == 0) {
		planificador = RR;
	}else if(strcmp(algoritmo, "VRR")==0){
		planificador = VRR;
	}else{
		log_error(logger, "El algoritmo no es valido");
	}
}
void inicializar_estructuras(){
    running = NULL;
    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    detener = false;
    primero = true;
    sigue = true;
    contador_pcb = 0;
    cola_new = inicializar_cola();
    cola_ready = inicializar_cola();
    cola_blocked = inicializar_cola();
    cola_vrr = inicializar_cola();
    sem_init(&sem_new, 0, 0);
    sem_init(&sem_ready, 0, 0);
    sem_init(&sem_grado_multiprogramacion, 0, grado_multiprogramacion_ini);
    pthread_mutex_init(&sem_detener, NULL);
    pthread_mutex_init(&sem_detener_largo, NULL);
    pthread_mutex_init(&sem_exec, NULL);
    pthread_mutex_init(&sem_interrupcion, NULL);
    pthread_mutex_init(&sem_vrr, NULL);
    pthread_mutex_init(&sem_quantum, NULL);
    pthread_mutex_init(&sem_detener_conexion, NULL);
    pthread_mutex_init(&sem_lista_bloqueado_interfaces, NULL);
    pthread_mutex_init(&sem_memoria, NULL);
    pthread_mutex_lock(&sem_interrupcion);
    pthread_mutex_lock(&sem_detener);
    pthread_mutex_lock(&sem_detener_largo);
    pthread_mutex_lock(&sem_detener_conexion);
    pthread_mutex_lock(&sem_vrr);
    pthread_mutex_lock(&sem_quantum);
    pthread_mutex_lock(&sem_memoria);
    lista_interfaces = list_create();
    lista_recursos = list_create();
    lista_recursos_pcb = list_create();
    lista_bloqueado_io = list_create();
    contador_aux=0;
    
}


void inicializar_recursos(){
    int i = 0;
    while(recursos_config[i]!= NULL){
        t_recurso* recurso = malloc(sizeof(t_recurso));
    	recurso->nombre = recursos_config[i];
    	recurso->instancias = atoi(instancias_recursos_config[i]);
        recurso->cola_bloqueados = inicializar_cola();
        list_add(lista_recursos, recurso);
        i++;
    }
}
t_cola * inicializar_cola(){
    t_cola *cola = malloc(sizeof(t_cola));
    cola->cola = queue_create();
    pthread_mutex_init(&(cola->sem_mutex), NULL);
    return cola;
}

void crear_proceso(){
    t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->estado =NEW;
    pcb->contexto = crear_contexto();
	contador_pcb++;
	//log_pcb_info(pcb);
	log_info(logger,"Se crea el proceso %i en NEW",pcb->contexto->pid);
	//agregar_cola_new(pcb);

}

t_pcb* retorno_pcb(){
    t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->estado =NEW;
    pcb->contexto = crear_contexto();
	contador_pcb++;
	//log_pcb_info(pcb);
	log_info(logger,"Se crea el proceso %i en NEW",pcb->contexto->pid);
	//agregar_cola_new(pcb);
    return pcb;
}


t_contexto_ejecucion * crear_contexto(){
    t_contexto_ejecucion * contexto = malloc(sizeof(t_contexto_ejecucion));
    contexto->pid = contador_pcb;
    //log_info(logger,"%i", contexto->pid);
    contexto->pc =0;
    t_registros_pcb* registro = crear_registro();
    contexto->registros = registro;
    return contexto;
}
t_registros_pcb* crear_registro(){
    t_registros_pcb* reg = malloc(sizeof(t_registros_pcb));
    reg->ax = 0;
    reg->bx = 0;
    reg->cx = 0;
    reg->dx = 0;
    reg->eax = 0;
    reg->ebx = 0;
    reg->ecx = 0;
    reg->edx = 0;
    reg->si = 0;
    reg->di = 0;
    return reg;
}

void liberar_proceso(t_pcb * pcb){
	char* estado_anterior = estado_a_string(pcb->estado);
	pcb->estado = TERMINATED;
	log_info(logger,"PID: %i - Estado Anterior: %s - Estado Actual: TERMINATED",pcb->contexto->pid,estado_anterior);
	//liberar_recursos(pcb->contexto->pid);
	//enviar_pcb(pcb,conexion_memoria,FINALIZAR);
	pthread_mutex_unlock(&sem_exec);
	sem_post(&sem_grado_multiprogramacion);
}
char* estado_a_string(t_estado estado) {
    switch (estado) {
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case RUNNING:
            return "RUNNING";
        case WAITING:
            return "WAITING";
        case TERMINATED:
            return "TERMINATED";
        default:
            return "Estado desconocido";
    }
}
char* planificador_a_string(t_planificador planificador) {
    switch (planificador) {
        case VRR:
            return "VRR";
        case RR:
            return "RR";
        case FIFO:
            return "FIFO";
        default:
            return "Planificador desconocido";
    }
}
void iniciar(){
    obtener_configuracion();
    inicializar_estructuras();
    inicializar_recursos();
}

