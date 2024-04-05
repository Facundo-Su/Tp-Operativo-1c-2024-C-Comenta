#include "global.h"

sem_t sem_new;
sem_t sem_ready;
sem_t sem_grado_multiprogramacion;
pthread_mutex_t sem_detener;
pthread_mutex_t sem_exec;
t_cola * cola_new;
t_cola * cola_ready;
t_cola * cola_blocked;
int quantum;
int grado_multiprogramacion_ini;
int contador_pcb;
char ** recursos_config;
char ** instancias_recursos_config;
t_list * lista_recursos;
t_list * lista_recursos_pcb;
t_planificador * planificador;


void obtener_configuracion(){
    char * puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    char * ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    char * puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    char * ip_cpu = config_get_string_value(config, "IP_CPU");
    char * puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    char * puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    char * algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignar_algoritmo(algoritmo);
    quantum = config_get_int_value(config, "QUANTUM");
    recursos_config = config_get_array_value(config, "RECURSOS");
    instancias_recursos_config = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion_ini = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
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
    contador_pcb = 0;
    cola_new = inicializar_cola();
    cola_ready = inicializar_cola();
    cola_blocked = inicializar_cola();
    sem_init(&sem_new, 0, 0);
    sem_init(&sem_ready, 0, 0);
    
    sem_init(&sem_grado_multiprogramacion, 0, grado_multiprogramacion_ini);
    pthread_mutex_init(&sem_detener, NULL);
    pthread_mutex_init(&sem_exec, NULL);
    lista_recursos = list_create();
    lista_recursos_pcb = list_create();
    
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
	contador_pid++;
	//log_pcb_info(pcb);
	log_info(logger_consola,"Se crea el proceso %i en NEW",pcb->pid);
	agregar_a_cola_new(pcb);

}
t_contexto_ejecucion * crear_contexto(){
    t_contexto_ejecucion * contexto = malloc(sizeof(t_contexto_ejecucion));
    contexto->pid = contador_pcb;
    contexto->pc =0;
    return contexto;
}

void terminar_proceso(t_pcb * pcb){
	char* estado_anterior = estado_a_string(pcb->estado);
	pcb->estado = TERMINATED;
	log_info(logger,"PID: %i - Estado Anterior: %s - Estado Actual: TERMINATED",pcb->contexto->pid,estado_anterior);
	liberar_recursos(pcb->pid);
	enviar_pcb(pcb,conexion_memoria,FINALIZAR);
	sem_post(&sem_ready);
	pthread_mutex_unlock(&sem_exec);
	sem_post(&sem_grado_multiprogramacion);
}
char* estado_a_string(t_estado * estado) {
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