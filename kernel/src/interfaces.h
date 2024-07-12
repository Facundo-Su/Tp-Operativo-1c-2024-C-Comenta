#ifndef INTERFACES_H_
#define INTERFACES_H_
#include "global.h"
#include "utils.h"
#include "planificadores.h"

void ejecutar_io_sleep(char * ,int ,t_pcb * );
void enviar_dormir(int ,int ,int );
void io_sleep_ready(int );
t_interfaz * buscar_interfaz_por_nombre(char* , t_list * );
void ejecutar_io_sleep2(char * ,int ,t_pcb * );
void agregar_cola_bloqueados_interfaces(t_interfaz * , t_pcb * );
void * quitar_cola_bloqueados_interfaces(t_interfaz * );
void agregar_interfaces(char * ,int,t_tipo_fs);
t_interfaz * buscar_interfaz_por_pid(int pid, t_list * lista);
void ejecutar_io_stdin_read(char* nombre_interfaz, int marco,int desplazamiento,int tamanio,t_pcb* pcb);
void ejecutar_io_stdin_write(char* nombre_interfaz, int marco,int desplazamiento,int tamanio,t_pcb* pcb);
void io_stdout_write_ready(int pid,char* nombre_interfaz);
void ejecutar_io_fs_create(char *nombre_interfaz,char* nombre_archivo_f_create,t_pcb* pcb);
void ejecutar_io_fs_delete(char *nombre_interfaz,char* nombre_archivo_f_delete,t_pcb* pcb);
void enviar_a_io_f_delete(char *nombre_interfaz,char* nombre_archivo_f_delete,t_pcb* pcb,int codigo_cliente);
void enviar_a_io_f_create(char *nombre_interfaz,char* nombre_archivo_f_create,t_pcb* pcb,int codigo_cliente);
void io_fs_delete_ready(int pid);
void io_fs_read_ready(int pid);
void io_fs_write_ready(int pid);
void io_fs_truncate_ready(int pid);
void io_sleep_ready2(int pid,char* nombre_interfaz);
void ejecutar_io_fs_write(char *nombre_interfaz,char* nombre_archivo_f_write,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb);
void enviar_a_io_fs_write(char *nombre_interfaz,char* nombre_archivo_f_write,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb,int codigo_cliente);

void ejecutar_io_fs_read(char *nombre_interfaz,char* nombre_archivo_f_read,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb);
void enviar_a_io_fs_read(char *nombre_interfaz,char* nombre_archivo_f_read,int marco,int desplazamiento,int tamanio,int puntero,t_pcb* pcb,int codigo_cliente);

void ejecutar_io_fs_truncate(char *nombre_interfaz,char* nombre_archivo_f_truncate,int tamanio,t_pcb* pcb);
void enviar_a_io_f_truncate(char *nombre_interfaz,char* nombre_archivo_f_truncate,int tamanio,t_pcb* pcb,int codigo_cliente);

void io_stdin_read_ready(int pid, char* nombre_interfaz);

void control_acceso(t_interfaz * interfaz);
void dial_fs_ready(char* nombre_interfaz);
int obtener_tiempo_vrr(t_pcb * pcb);
#endif