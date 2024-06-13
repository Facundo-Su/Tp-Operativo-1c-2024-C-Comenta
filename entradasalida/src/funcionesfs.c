#include "funcionesfs.h"

/*
void levantarBitMap(){//crea y inicializa en 0 el bitmap
    //CREAR ARCHIVO BITMAP
    if(fopen(ruta,"rb") == NULL){
    void* punterobitmap = calloc( block_count/ 8,1);//1 es  un byte y inicializa en 0
    FILE* archivoBM = fopen(configFS.ruta_bitmap,"wb");
        if(archivoBM == NULL)
    {
        log_error(logger,"no se pudo crear el archivo de bitmap");
    }
    fwrite(punterobitmap,1,block_count / 8,archivoBM);
    fclose(archivoBM);
    free(punterobitmap);
    }
    //MAPEAR BITMAP
    int fd = open(ruta,O_RDWR);
    char* bitarray = malloc(block_count / 8);//CHAR?
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    msync(bitarray,block_count / 8,MS_SYNC); //estan bien los parametros de msync) o 
    log_info(logger,"se creo bitmap");
    close(fd);
    bitarray_destroy(bitmap);
}

void levantar_archivos_bloque(){
    TO-DO
}

void crear_archivos_metadata(){
    TO-DO
}
*/

