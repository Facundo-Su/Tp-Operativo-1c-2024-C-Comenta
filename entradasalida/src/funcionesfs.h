#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

#include <utils/hello.h>

#include <fcntl.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <sys/mman.h>

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
}t_metadata; //estructura que podria necesitar para administrar los metadatas?

void levantarBitMap();
void levantar_archivo_bloques();
void crear_archivo_metadata(char* nombre_archivo);
int proximoBitDisponible();
void asignarProximoBitDisponible();
void ocupar_un_bloque_incio(int bloque);

#endif