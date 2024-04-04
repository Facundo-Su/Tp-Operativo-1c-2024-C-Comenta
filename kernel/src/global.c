#include "global.h"
sem_t sem_new;
sem_t sem_ready;
sem_t sem_grado_multiprogramacion;
t_cola * cola_new;
t_cola * cola_ready;
t_cola * cola_blocked;

void obtener_configuracion(){
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignar_algoritmo(algoritmo);
    quantum = config_get_int_value(config, "QUANTUM");
    recursos = config_get_array_value(config, "RECURSOS");
    instancia_recurso = config_get_array_value(config, "INSTANCIAS_RECURSOS");
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
