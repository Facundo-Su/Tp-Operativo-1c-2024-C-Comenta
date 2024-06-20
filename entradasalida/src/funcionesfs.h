#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

#include <utils/hello.h>
#include <commons/collections/list.h> 
#include <fcntl.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <math.h>

extern int block_count;
extern int block_size;
extern char* path_base_dialfs;
extern char* rutita_prueba;
//char* rutadial ="./dialfs";
typedef struct{
    char* nombre;
    uint32_t tamanio_archivo;
    uint32_t bloq_inicial_archivo;
    //t_config* config; lo necesitaria?
}t_metadata; 
//estructura que podria necesitar para administrar los metadatas?
extern t_list* metadatas;

void levantarBitMap();
void levantar_archivo_bloques();
void crear_archivo_metadata(char* nombre_archivo);
int proximoBitDisponible();
void asignarProximoBitDisponible();
void ocupar_un_bloque_incio(int bloque);
void borrar_archivo(char* nombre_archivo);
void inicializar_lista();
void funcion_prueba_lista();
void truncar_archivo(char *nombre, int nuevo_tamanio_bytes);
t_metadata* devolver_metadata(char *nombre);
void ampliar_tam_archivo(t_metadata* meta, int tamanio_nuevo_bytes);
void reducir_tam_archivo(t_metadata* meta, int tamanio_nuevo_bytes);
int calcular_bloq_necesarios(int bytes_nuevos_necesarios);
bool hay_bloques_libres_contiguos(int cant_nuevos_bloques,int ultimo_bloque_Actual);
void compactar(); 
void asignarBits(int cant_nuevos_bits,int ultimo_bit);
void liberarBits(int cant_bloq_a_liberar,int ultimo_bloque_Actual);
void modificar_config_tam(char* nombre_archivo,int tamanio_nuevo_bytes);

#endif