#include "funcionesfs.h"

//path_base_dialfs
void *archivo_de_bloques;
//const t_list* metadatas=list_create();

void levantarBitMap(){//crea y inicializa en 0 el bitmap
    //CREAR ARCHIVO BITMAP
    
    if(fopen(rutita_prueba,"rb") == NULL){
    
        void* punterobitmap = calloc( block_count/ 8,1);//1 es  un byte y inicializa en 0
        
        FILE* archivoBM = fopen(rutita_prueba,"wb");
        
        if(archivoBM == NULL)
        {
            log_error(logger,"no se pudo crear el archivo de bitmap");
            exit(EXIT_FAILURE);
        }
        //log_warning(logger,"block_count %i, ruta es %s", block_count,rutita_prueba);
        fwrite(punterobitmap,1,block_count / 8,archivoBM);
        //log_warning(logger,"block_count %i, ruta es %s", block_count,rutita_prueba);
        fclose(archivoBM);
        free(punterobitmap);
    }
    //MAPEAR BITMAP
    int fd = open(rutita_prueba,O_RDWR);
    log_warning(logger,"fd %i", fd);
    char* bitarray = malloc(block_count / 8);//CHAR?
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
        exit(EXIT_FAILURE);
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    //log_info(logger,"se creo bitmap y %s",bitarray);
    msync(bitarray,block_count / 8,MS_SYNC); //estan bien los parametros de msync) o 
    //log_info(logger,"se creo bitmap y %s",bitarray);
    close(fd);
    bitarray_destroy(bitmap);
}

void levantar_archivo_bloques(){
    //ver que ruta le defino
    //luego decidir en que ruta ponemos el bloques.dat por ahora en el raiz
    int file_descrip_bloques = open("bloques.dat", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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

    archivo_de_bloques = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED,file_descrip_bloques, 0);

    if (archivo_de_bloques != MAP_FAILED) {
        log_info(logger,"El archivo bloques se ha mapeado correctamente en la memoria.");

    } else {
        perror("Error al mapear el archivo de bloques");
        close(file_descrip_bloques);
        exit(EXIT_FAILURE);
    }

    // Cerrar el archivo después de asignar el mapeo
    close(file_descrip_bloques);
}
/*void inicializar_lista(){
    metadatas=list_create();
}*/
void funcion_prueba_lista(){
    log_warning(logger,"la lista tiene  %i archivos metadata",list_size(metadatas));
    //t_metadata* aux = (t_metadata*) malloc(sizeof(t_metadata));
    t_metadata* aux=list_get(metadatas,2);
    log_warning(logger,"el nombre de metadata es %s",aux->nombre);
    if (aux != NULL) {
        //cambiame por log_warning
        log_warning(logger, "Nombre del archivo: %s", aux->nombre);
        log_warning(logger, "Tamaño del archivo: %u", aux->tamanio_archivo);
        log_warning(logger, "Bloque inicial del archivo: %u", aux->bloq_inicial_archivo);
    } else {
        log_warning(logger, "La lista está vacía o el índice es inválido.");
    }
 
}
void crear_archivo_metadata(char* nombre_archivo){
    char* escribo_key = string_new();
    FILE* archivo_MD;
    char* extension = "txt";
    char* path_archivo = string_new();
    t_metadata* nueva_metadata = (t_metadata*) malloc(sizeof(t_metadata));
    t_config* archivo;

    string_append_with_format(&path_archivo, "%s/%s.%s", path_base_dialfs, nombre_archivo,extension);
    //log_warning(logger,"la path es  %s",path_archivo);  
    int primerBloqueLibre = proximoBitDisponible();//probar funcion
    //log_warning(logger,"bit libre %i",primerBloqueLibre); 
    asignarProximoBitDisponible();
    //OCUPAR NO ES NECESARIO
    ocupar_un_bloque_incio(primerBloqueLibre);
    char* bloqueInicialEnChar = string_itoa(primerBloqueLibre);
    log_warning(logger,"el nombre de archivo es%s",path_archivo);
    archivo_MD = fopen(path_archivo, "w");

    string_append_with_format(&escribo_key, "%s=%s\n", "TAMANIO_ARCHIVO", "0"); //inicializo
    string_append_with_format(&escribo_key, "%s=%s\n", "BLOQUE_INICIAL", "10");//inicializo

    fwrite(escribo_key, strlen(escribo_key), 1, archivo_MD); 

    fclose(archivo_MD);

    nueva_metadata->nombre=nombre_archivo;
    nueva_metadata->tamanio_archivo=0;//inician con 0 tamanio;
    nueva_metadata->bloq_inicial_archivo=primerBloqueLibre;
    
    list_add(metadatas,nueva_metadata);

    //lo meto en una lista administrativa de metadatas
    archivo = config_create(path_archivo);
    //aca uso las commons del config?
    config_set_value(archivo, "TAMANIO_ARCHIVO", "0");
    config_set_value(archivo, "BLOQUE_INICIAL", bloqueInicialEnChar);
    log_warning(logger, "bloque inicial es %s", bloqueInicialEnChar);
    //log_warning(logger,"itoa:  %s",bloqueInicialEnChar);
    config_save(archivo);
    //log_warning(logger,"archivo:  %s",nueva_metadata->nombre);
    //log_warning(logger,"tamanio lista  %i",list_size(metadatas));
    config_destroy(archivo);

    free(path_archivo);

}

int proximoBitDisponible(){
    int fd = open(rutita_prueba,O_RDONLY);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ ,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
        exit(EXIT_FAILURE);
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    int i = 0;
    while(bitarray_test_bit(bitmap,i)==1){
        i++;
    }

    log_warning(logger,"el bit disponible es %i \n \n \n \n ",i);
    close(fd);
    bitarray_destroy(bitmap);

    return i;
}

void asignarProximoBitDisponible(){ //talvez se pueda achicar aun mas el codigo.
    int fd = open(rutita_prueba,O_RDWR);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
        exit(EXIT_FAILURE);
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

void ocupar_un_bloque_incio(int bloque){
    /*FILE* archivoBLoques = fopen("bloques.dat","rb+");
    //memcpy(0,"x",block_size);
    fseek(archivoBLoques,0,SEEK_SET);
    fwrite("xxxxxxxx",strlen("xxxxxxxx"),1,archivoBLoques);
    fclose(archivoBLoques);*/
    log_warning(logger,"inicializo el bloque %i",bloque);
    //char* buffer = (char*)malloc(block_size);
    memset(archivo_de_bloques+(bloque*block_size), 'a', block_size);
    //free(buffer);
}
void borrar_archivo(char* nombre_archivo){
    char* extension = "txt";
    char* path_archivo = string_new();
    t_metadata* meta = devolver_metadata(nombre_archivo);
    //elimino elemento de la lista
    saco_metadata_de_lista(nombre_archivo);
    log_error(logger,"la metadata es %s, el tamanio es %i ",meta->nombre, meta->tamanio_archivo);
    //libero sus bits del bitmap
    int cant_bloques_actuales =(int)ceil((double)meta->tamanio_archivo / block_size);
    int ultimo_bloque_Actual = (meta->bloq_inicial_archivo+cant_bloques_actuales)-1;
    liberarBits(cant_bloques_actuales, ultimo_bloque_Actual);
    //elimino archivo metadata(txt)
    string_append_with_format(&path_archivo, "%s/%s.%s", path_base_dialfs, nombre_archivo,extension);
    if (remove(path_archivo) == 0) {
        log_info(logger,"El archivo %s ha sido eliminado exitosamente.\n", nombre_archivo);

    } else {
        log_warning(logger,"Error al eliminar el archivo");
    }
}

void saco_metadata_de_lista(char* nombre_archivo){
    for (int i = 0; i < list_size(metadatas); i++) {
        t_metadata* meta_buscado = list_get(metadatas, i);
        if (strcmp(meta_buscado->nombre, nombre_archivo) == 0) {
            
            list_remove(metadatas, i);
            log_info(logger, "se elmina %s de la lista",meta_buscado->nombre);
        }

    }

    log_info(logger,"El archivo buscado no se encuentra en la lista ya fue eliminado antes..");

}

void truncar_archivo(char *nombre, int nuevo_tamanio_bytes) {

    t_metadata* meta = devolver_metadata(nombre);
    log_info(logger,"los valores que estoy comparando son %i y %i",nuevo_tamanio_bytes,meta->tamanio_archivo);
    if (meta->tamanio_archivo < nuevo_tamanio_bytes) {
        //aqui podria verificar si hay o no bloques disponibles.
        ampliar_tam_archivo(meta, nuevo_tamanio_bytes);

    } else if(meta->tamanio_archivo>nuevo_tamanio_bytes){
        //si un archivo ocupa un bloque se puede reducir? noo creo que prueben eso.
        reducir_tam_archivo(meta, nuevo_tamanio_bytes);
        
    }else{
        log_info(logger,"el archivo tiene el mismo tamaño para truncar");
    }
    //actualizar_meta(meta);
}

t_metadata* devolver_metadata(char *nombre) {
    
    char* aux = strtok(nombre, "\n");
    //log_info(logger, "llega el archivo %s",nombre);
    for (int i = 0; i < list_size(metadatas); i++) {
        t_metadata* meta_buscado = list_get(metadatas, i);

        if (strcmp(meta_buscado->nombre, nombre) == 0) {
            log_info(logger, "se encontro el archivo %s",meta_buscado->nombre);

            return meta_buscado;
        }

    }

    log_info(logger,"El archivo buscado no se encuentra en la lista.");
    return NULL;
}

// ejemplo ampliar_tam_archivo(meta,256) cada bloque tiene 64bytes segun el config.
// ejemplo ampliar_tam_archivo(meta,280) cada bloque tiene 64bytes segun el config.
// en el caso de 280 necesito 4,37 bloques lo redondeo a 5.

void ampliar_tam_archivo(t_metadata* meta, int tamanio_nuevo_bytes) {
    bool espacio_contiguo;
    int cant_bloques_actuales;
    int ultimo_bloque_Actual;
    if(meta->tamanio_archivo==0){
        meta->tamanio_archivo = block_size;
        cant_bloques_actuales=1;
        ultimo_bloque_Actual= meta->bloq_inicial_archivo;
        //caso de nuevo archivo
    }else{
        //obtengo ultimo bloque
        cant_bloques_actuales =(int)ceil((double)meta->tamanio_archivo / block_size);
        ultimo_bloque_Actual = (meta->bloq_inicial_archivo+cant_bloques_actuales)-1;
        //el ultimo_bloque esta mal creo, capaz falta sumarle el meta->bloq_inicio_Archivo
        
    }
    log_warning(logger, " los valores son %i y %i",tamanio_nuevo_bytes,meta->tamanio_archivo);
    int bytes_nuevos_necesarios = tamanio_nuevo_bytes - meta->tamanio_archivo;  
    log_warning(logger, "necesito %i bytes",bytes_nuevos_necesarios);
    log_warning(logger, "el archivo me ocupa %i bloques",cant_bloques_actuales);
    // cuantos bloques necesito agregar? multiplo o no? si es nuevo archivo?

    //----------parece funcionar hasta aca------------

    int cant_nuevos_bloques = calcular_bloq_necesarios(bytes_nuevos_necesarios);
    //int can_bloques_archivo=meta->bloq_inicial_archivo+cant_nueva_bloques;
    //chequeo que los proximo bits esten en 0 para asignarlos de lo contrario compactar();
    
    espacio_contiguo = hay_bloques_libres_contiguos(cant_nuevos_bloques,ultimo_bloque_Actual);
    if(espacio_contiguo == true){
        log_warning(logger, "hay bloques continuos libres suficientes");
        log_warning(logger, "tengo que agregar %i bloques",cant_nuevos_bloques);
        log_warning(logger, "ultimo bloque del archivo es %i ",ultimo_bloque_Actual);

        //meta->bloq_inicial_archivo=primerBloqueLibre;
        //el bloque inicial podria cambiar solo cuando compacto o no? 
        asignarBits(cant_nuevos_bloques,ultimo_bloque_Actual);
        modificar_config_tam(meta->nombre,tamanio_nuevo_bytes); //escribo en el config(txt) metadata
    }else{
        //log_info(logger, “PID: <PID> - Inicio Compactación.",pid_f_truncate);
        compactar(meta,cant_nuevos_bloques);
        //log_info(logger, “PID: <PID> - Fin Compactación.”,pid_f_truncate);
        usleep(retraso_compactacion * 1000);
        modificar_config_tam(meta->nombre,tamanio_nuevo_bytes);
    }
    meta->tamanio_archivo=tamanio_nuevo_bytes;//actualizo nuevo tamaño
    
    

}

void compactar(t_metadata* meta, int cant_bloq_necesarios){
    log_warning(logger, "compactar");

    t_metadata* aux;

    for(int i=0; i< list_size(metadatas);i++){
        aux = list_get(metadatas,i);
        if(strcmp(aux->nombre,meta->nombre)==0){
           aux->cantidad_bloque_agrandar=cant_bloq_necesarios;  
        }
        aux->datos = malloc(aux->tamanio_archivo);
        int bloque_inicial = aux->bloq_inicial_archivo;
        
        //void*datos_guardados = malloc(aux->tamanio_archivo);

        //memcpy(datos_guardados,archivo_de_bloques + (bloque_inicial*block_size),aux->tamanio_archivo);
        
        memcpy(aux->datos,archivo_de_bloques+(bloque_inicial*block_size),aux->tamanio_archivo);

    }

    vaciar_bit_map();

    // asigno de nuevos los bloques
    int bloque_libre_encontrado =0;
    int valor_calculado;
    for(int i=0;i<list_size(metadatas);i++){
        aux = list_get(metadatas,i);
        //log_error(logger,"el nombre del archivo es %s",aux->nombre);
        //log_error(logger,"el tamanio del archivo es %i",aux->tamanio_archivo);
    }

    for(int i=0;i<list_size(metadatas);i++){
        //bloque_libre_encontrado = proximoBitDisponible();

        //log_error(logger,"bloque libre encontrado es %i \n \n",bloque_libre_encontrado);

        aux = list_get(metadatas,i);
        if(strcmp(aux->nombre,meta->nombre)==0){
            //log_error(logger," cantidad de bloque que asigno es %i \n \n",(aux->tamanio_archivo / block_size) + aux->cantidad_bloque_agrandar);
            valor_calculado = (aux->tamanio_archivo / block_size) + aux->cantidad_bloque_agrandar;
            asignarBits((aux->tamanio_archivo / block_size) + aux->cantidad_bloque_agrandar,bloque_libre_encontrado);
        }else{
            //log_error(logger," cantidad de bloque que asigno es %i \n \n",(aux->tamanio_archivo / block_size));
            asignarBits((aux->tamanio_archivo / block_size),bloque_libre_encontrado);
            valor_calculado = (aux->tamanio_archivo / block_size);
            if((aux->tamanio_archivo / block_size) == 0){
                valor_calculado = 1;
            }
        }
        modificar_bloque_inicial(aux->nombre,bloque_libre_encontrado);
        memcpy(archivo_de_bloques + (bloque_libre_encontrado * block_size), aux->datos, aux->tamanio_archivo);

        bloque_libre_encontrado += valor_calculado; 

        




    }
}

void modificar_bloque_inicial(char* nombre_archivo,int bloque_inical){
    char* extension = "txt";
    char* path_archivo = string_new();
    t_config* archivo;

    string_append_with_format(&path_archivo, "%s/%s.%s", path_base_dialfs, nombre_archivo,extension);
    log_warning(logger,"el nombre de archivo a modificar es%s",path_archivo);

    archivo = config_create(path_archivo);

    char* nuevo_tam = string_itoa(bloque_inical);
    log_error(logger,"el nuevo BLOQUE INICIAL es %s",nuevo_tam);
    config_set_value(archivo, "BLOQUE_INICIAL", nuevo_tam);
    //config_set_value(archivo, "BLOQUE_INICIAL", bloqueInicialEnChar);
    config_save(archivo);
    config_destroy(archivo);
    free(path_archivo);
}



void vaciar_bit_map(){
    log_warning(logger, "vaciar_bit_map");
    int fd = open(rutita_prueba,O_RDWR);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
 
    for(int i=0;i<block_count;i++){
        bitarray_clean_bit(bitmap,i);
    }

    msync(bitarray,block_count / 8,MS_SYNC);
    close(fd);
    bitarray_destroy(bitmap);

}

int calcular_bloq_necesarios(int bytes_nuevos_necesarios) {
    int cant_bloq_necesarios;
    if (bytes_nuevos_necesarios % block_size == 0) {//es multipo
        // ejemplo 192 / 64 =3 con resto 0 porque es multiplo
        cant_bloq_necesarios = bytes_nuevos_necesarios / block_size; 
    } else {
        //aca entraria en el caso de ingresar 280-> 280-64=216 que no es multiplo de 64
        //los bytes restantes no llegan a ocupar 1 bloque
        if (bytes_nuevos_necesarios > block_size) {
            cant_bloq_necesarios = (bytes_nuevos_necesarios / block_size) + 1;
        } else {
            //cant bytes<block_size
            cant_bloq_necesarios = 1;
        }

    }
    return cant_bloq_necesarios;
}

bool hay_bloques_libres_contiguos(int cant_nuevos_bloques,int ultimo_bloque_Actual){
    //ver en si en el bitmap los proximos bits que necesito(simulan bloques que necesito) 
    //estan en 0. O si no estan en 0 devuelvo un false porque no tendria espacio contiguo.
    bool respuesta=true;
    int fd = open(rutita_prueba,O_RDONLY);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ ,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
        exit(EXIT_FAILURE);
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    //int i = 0;
    int proximo=ultimo_bloque_Actual+1;

    //devuelve bool bitarray_test_bit  true=1;
    for(int i=0;i<cant_nuevos_bloques;i++){
        if(bitarray_test_bit(bitmap,proximo)==1){//verifica si esta en 1 el proximo bit.
            respuesta=false;
        }
        proximo++;
    }
    
    close(fd);
    bitarray_destroy(bitmap);
    return respuesta;
}

void reducir_tam_archivo(t_metadata* meta, int tamanio_nuevo_bytes){
    int cant_bloques_actuales;
    int ultimo_bloque_Actual;
    int cant_bloques_final;
    if(meta->tamanio_archivo==0){
        log_warning(logger, "no podes reducir un archivo de un bloque, para eso eliminalo..");
    }
    cant_bloques_actuales =(int)ceil((double)meta->tamanio_archivo / block_size);
    cant_bloques_final =(int)ceil((double)tamanio_nuevo_bytes / block_size);
    //me paro en el ultimo bloque actual.
    ultimo_bloque_Actual = (meta->bloq_inicial_archivo+cant_bloques_actuales)-1;
    //libero bloques(bits del bitmap) desde el final..
    int diferencia_bytes = meta->tamanio_archivo - tamanio_nuevo_bytes;
    //int cant_bloq_a_liberar = calcular_bloq_necesarios(diferencia_bytes);
    int cant_bloq_a_liberar = cant_bloques_actuales - cant_bloques_final;
    //cant_liberar se ve mejor con cuentas/grafico.
    liberarBits(cant_bloq_a_liberar,ultimo_bloque_Actual);
    meta->tamanio_archivo = tamanio_nuevo_bytes;
    modificar_config_tam(meta->nombre,tamanio_nuevo_bytes);
}

void asignarBits(int cant_nuevos_bits,int ultimo_bit){

    log_error(logger, "el valor del ultimo bit es: %i",ultimo_bit);

    log_error("los valores que recibi son: %i y %i \n \n \n \n \n",cant_nuevos_bits,ultimo_bit);
    int fd = open(rutita_prueba,O_RDWR);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
        exit(EXIT_FAILURE);
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
    int proximo=ultimo_bit+1;
    //int proximo= ultimo_bit;
    for(int i=0;i<cant_nuevos_bits;i++){
        bitarray_set_bit(bitmap,proximo);
        if(bitarray_test_bit(bitmap,proximo)==1){
            log_error(logger,"no se pudo asignar el bit porque esta ocupado");
        }
        proximo++;
    }
    
    log_error(logger,"hasta %i tengo ocupado el bitmap",proximo);

    msync(bitarray,block_count / 8,MS_SYNC);//eso o fd? luego ver
    close(fd);
    bitarray_destroy(bitmap);
}

void liberarBits(int cant_bloq_a_liberar,int ultimo_bloque_Actual){
    int fd = open(rutita_prueba,O_RDWR);
    char* bitarray = malloc(block_count / 8);
    bitarray = mmap(NULL,block_count / 8,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(bitarray == MAP_FAILED)
    {
        log_error(logger,"no se pudo mapear el archivo de bitmap");
    }
    t_bitarray* bitmap = bitarray_create_with_mode(bitarray,block_count / 8,MSB_FIRST);
 
    for(int i=0;i<cant_bloq_a_liberar;i++){
        bitarray_clean_bit(bitmap,ultimo_bloque_Actual);
   
        ultimo_bloque_Actual--;
    }

    msync(bitarray,block_count / 8,MS_SYNC);
    close(fd);
    bitarray_destroy(bitmap);
}

void modificar_config_tam(char* nombre_archivo, int tamanio_nuevo_bytes){
    char* extension = "txt";
    char* path_archivo = string_new();
    t_config* archivo;

    string_append_with_format(&path_archivo, "%s/%s.%s", path_base_dialfs, nombre_archivo,extension);
    log_warning(logger,"el nombre de archivo a modificar es%s",path_archivo);

    archivo = config_create(path_archivo);

    char* nuevo_tam = string_itoa(tamanio_nuevo_bytes);
    config_set_value(archivo, "TAMANIO_ARCHIVO", nuevo_tam);
    //config_set_value(archivo, "BLOQUE_INICIAL", bloqueInicialEnChar);
    config_save(archivo);
    config_destroy(archivo);
    free(path_archivo);

}

void escribir_archivo_bloque(int puntero, char* nombre,int tamanio,void* a_escribir){
    log_warning(logger,"dato %s",(char*)a_escribir);
    t_metadata* meta = devolver_metadata(nombre);
    uint32_t numero_bloque = puntero / block_size;
    //me paro en el bloque que voy a escribir
    uint32_t bloque_escribir=(meta->bloq_inicial_archivo+numero_bloque);
    log_warning(logger,"el bloque a escribir es %i",bloque_escribir);
    memcpy(archivo_de_bloques  + (bloque_escribir * block_size), a_escribir, tamanio);
    //escribo el tamaño de un bloque o uso el tamanio que me pasa kernel?
    
    msync(archivo_de_bloques,block_count*block_size, MS_SYNC);
    //tamanio_Archivo_bloques =block_count*block_size
}
void *leer_archivo_bloque(int puntero, char* nombre,int tamanio)
{
    if(list_size(metadatas) == 0){
        log_error(logger,"no hay metadatas");
    }

    log_error(logger,"tamanio de metadata es  %i \n \n \n \n ",list_size(metadatas));
    for(int i=0;i<list_size(metadatas);i++){
        t_metadata* meta = list_get(metadatas,i);
        //log_error(logger,"el nombre del archivo es %s",meta->nombre);
        //log_error(logger,"el tamanio del archivo es %i",meta->tamanio_archivo);

    }


    t_metadata* meta = devolver_metadata(nombre);
    
    if(meta == NULL){
        log_error(logger,"no se encontro el archivo %s",nombre);
    }
    uint32_t numero_bloque = puntero / block_size;
    //log_error(logger,"el puntero es %i",puntero);
    //log_error(logger,"el numero de bloque es %i",numero_bloque);
    //log_error(logger,"el bloque inicial es %i",meta->bloq_inicial_archivo);
    //log_error(logger,"el tamanio que leo es %i",tamanio);
    uint32_t bloque_escribir=(meta->bloq_inicial_archivo+numero_bloque);
    // log_debug(logger, "SE VA A LEER EN UN BLOQUE");
    void *datoLeido = malloc(tamanio);//block size o tamanioleer?
    //void *datoLeido = malloc(tamanioALeer);//block size o tamanioleer?
    memcpy(datoLeido, archivo_de_bloques  + (bloque_escribir * block_size), tamanio);

    return datoLeido;
}
