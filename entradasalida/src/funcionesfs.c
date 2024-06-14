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

void levantar_archivo_bloques(){
    //ver que ruta le defino
    int file_descrip_bloques = open(ruta_bloques, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (file_descrip_bloques == -1) {
        perror("Error al abrir el archivo de bloques");
        exit(EXIT_FAILURE);
    }
    int tamanio = block_count*block_size;

    if (ftruncate(file_descrip_bloques, tamanio) == -1) {
        perror("Error al establecer el tamaño del archivo de bloques");
        close(file_descrip_bloques);
        exit(EXIT_FAILURE);
    }

    void *bloq = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED,file_descrip_bloques, 0);

    if (bloq != MAP_FAILED) {
        log_info(logger,"El archivo bloques se ha mapeado correctamente en la memoria.");

    } else {
        perror("Error al mapear el archivo de bloques");
        close(file_descrip_bloques);
        exit(EXIT_FAILURE);
    }

    // Cerrar el archivo después de asignar el mapeo
    close(file_descrip_bloques);
}

void crear_archivo_metadata(char* nombre_archivo){

    FILE* archivo_MD;
    char* extension = "txt";
    char* path_archivo = string_new();
    t_config* archivo;
    string_append_with_format(&path_archivo, "%s/%s.%s", path_base_dialfs, nombre_archivo,extension);
        
    int primerBloqueLibre = proximoBitDisponible();//probar funcion
    asignarProximoBitDisponible();
    char* bloqueInicialEnChar = itoa(primerBloqueLibre);

    archivo_MD = fopen(path_archivo, "w");

    
    archivo = config_create(path_archivo);
    //aca uso las commons del config?
    config_set_value(path_archivo, "TAMANIO_ARCHIVO", "0");
    config_set_value(path_archivo, "BLOQUE_INICIAL", bloqueInicialEnChar);
    config_save(path_archivo);

    config_destroy(path_archivo);

    fclose(archivo_MD);
}

int proximoBitDisponible(){
    int fd = open(ruta_bitmap,O_RDONLY);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ ,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    int i = 0;
    while(bitarray_test_bit(bitmap,i)==1){
        i++;
    }
    close(fd);
    bitarray_destroy(bitmap);
    return i;
}

void asignarProximoBitDisponible(){ //talvez se pueda achicar aun mas el codigo.
    int fd = open(ruta_bitmap,O_RDWR);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    int i=0;
    while(bitarray_test_bit(bitmap,i)==1){
        i++;
    }
    bitarray_set_bit(bitmap,i);
    msync(bitarray,block_count / 8,MS_SYNC);//eso o fd? luego ver
    close(fd);
    bitarray_destroy(bitmap);
}

*/

