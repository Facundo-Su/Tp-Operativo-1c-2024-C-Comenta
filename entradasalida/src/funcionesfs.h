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

void levantarBitMap();
void levantar_archivo_bloques();
void crear_archivo_metadata(char* nombre_archivo);
int proximoBitDisponible();
void asignarProximoBitDisponible();

#endif