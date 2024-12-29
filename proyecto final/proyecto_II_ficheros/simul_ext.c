/*
Nombre y apellidos: Marta de la Paz y Julio Godínez Simón
Curso: 2024/2024
Grupo: A
Asignatura: Sistemas Operativos
*/

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

//Funciones
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main () {
         char *comando[LONGITUD_COMANDO];
         char *orden[LONGITUD_COMANDO];
         char *argumento1[LONGITUD_COMANDO];
         char *argumento2[LONGITUD_COMANDO];

         int i,j;
         unsigned long int m;
     EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
         
     int entradadir;
     int grabardatos;
         
     FILE *fent;

     // Lectura del fichero completo de una sola vez
//...

     fent = fopen("particion.bin","r+b");
     if (fent == NULL) {
         printf("Error al abrir el archivo 'particion.bin'\n");
         return -1; //Indica que ha habido un error
     }
     
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    


     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);

     // Bucle de tratamiento de comandos
     for (;;){
                 do {
                 printf (">> ");
                 fflush(stdin);
                 fgets(comando, LONGITUD_COMANDO, stdin);
                 } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
              //Si el comando es info
              if (strcmp(orden, "info") == 0) {
                       LeeSuperBloque(&ext_superblock);
                       continue;
              }
              //Si el comando es dir
              if (strcmp(orden,"dir") == 0) {
                       Directorio(&directorio, &ext_blq_inodos); 
                       continue;
              }
              //Si el comando es bytemaps
              if (strcmp(orden, "bytemaps") == 0) {
                       Printbytemaps(&ext_bytemaps);
                       continue;
              }
              //Si el comando es rename
              if (strcmp(orden, "rename") == 0) {
                       Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
                       continue;
              }
              //Si el comando es remove
              if (strcmp(orden, "remove") == 0) {
                       Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
                       continue;
              }
              //Si el comando es copy
              if (strcmp(orden, "copy") == 0) {
                       Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent); 
                       continue;
              }
         //...
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos, fent);
         grabardatos = 0;
              
         //Si el comando es salir se habrán escrito todos los metadatos
         if (strcmp(orden,"salir")==0){
            GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
     }
}

//Función para imprimir el bytemap por pantalla
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) {
         printf("Bytemap de bloques: \n");
         //Bucle que itera sobre los elementos del bytemap hasta el límite definido y los imprime.
         //Cada elemento del bytemap indica si un bloque está libre (0) u ocupado (1).
         for (int i = 0; i < MAX_BLOQUES_PARTICION; i++) { 
                  printf("%d ", ext_bytemaps -> bmap_bloques[i]);
         }
         
         printf("\nBytemap de inodos:\n");
         //Bucle que itera sobre los elementos del bytemaps de inodos. Misma funcionalidad que el bucle anterior
         //pero esta vez con los inodos.
         for (int i = 0; i < MAX_INODOS; i++) {
                  printf("%d ", ext_bytemaps -> bmap_inodos[i]);
         }
         printf("\n");
}

//Función para comprobar que el comando sea correcto
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
         int num = sscanf(strcomando, "%s %s %s", orden, argumento1, argumento2);
         //Si solo es "salir" retorna 0 indicando que es correcto el comando
         if (num == 1 && strcmp(orden, "salir") == 0) {
                  return 0;
         }
         //Si se ha leído al menos un argumento, y no hay errores entonces retorna 0
         if (num >= 1) {
                  return 0;
         }
         //Si no se ha podido leer correctamente, retorna -1
         return -1;
}

//Función para leer el superbloque
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup) {
         //Verifico si el puntero es NULL
         if (psup == NULL) {
                  printf("ERROR. El superbloque es NULL\n");
                  return;
         }
         //Muestro la información del superbloque
         printf("Bloque %d bytes\n", psup -> s_block_size);
         printf("Inodos particion = %d\n",psup -> s_inodes_count);
         printf("Inodos libres = %d\n",psup -> s_free_inodes_count);
         printf("Bloques particion = %d\n",psup -> s_blocks_count);
         printf("Bloques libres = %d\n",psup -> s_free_blocks_count);
}

//Función para buscar el fichero
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre) {
         //Verifico si el puntero al directorio es NULL
         if (directorio == NULL) {
                  return -1; //Si el puntero al directorio es NULL, devuelve -1
         }
         //Recorro todas las entradas del directorio
         for (int i = 0; i < MAX_FICHEROS; i++) {
                  //Compruebo si el nombre de la entrada actual es igual al nombre buscado
                  int j = 0;
                  while (directorio[i].dir_nfich[j] != '\0' && nombre[j] != '\0') {
                           if (directorio[i].dir_nfich[j] != nombre[j]) {
                                    break; //Si los caracteres no coinciden se sale del bucle
                           }
                           j++; //Se pasa al siguiente caracter
                  }
                  //Si ambos nombres son iguales (y la longitud es la misma), se devuelve el índice
                  if (directorio[i].dir_nfich[j] == '\0' && nombre[j] == '\0') {
                           return i;
                  }
         }
         //Si no se ha encontrado el archivo devuelve -1
         return -1;
}

//Función para listar todos los ficheros
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
         //Se comprueba que los punteros no sean nulos
         if (directorio == NULL || inodos == NULL) {
                  printf("ERROR: Los punteros proporcionados son nulos \n");
                  return;
         }
         printf("NOMBRE\t\tTAMANIO\t\tINODO\t\tBLOQUES\n");
         
         for (int i = 1; i < MAX_FICHEROS; i++) { //Recorro las entradas del diretorio
                  if (directorio[i].dir_inodo != NULL_INODO) { //En caso de que la entrada del directorio no tenga un inodo inválido
                           EXT_SIMPLE_INODE *inodo = &inodos -> blq_inodos[directorio[i].dir_inodo]; //Se obtiene el inodo correspondiente
                           printf("%s\t\t%d\t\t%d\t\t", directorio[i].dir_nfich, inodo -> size_fichero, directorio[i].dir_inodo); //Se imprime lo que corresponda
                           //Bucle para mostrar los bloques de disco asignados al archivo
                           for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
                                    if (inodo -> i_nbloque[j] != NULL_BLOQUE) {
                                             printf("%d ", inodo -> i_nbloque[j]);
                                    }
                           }
                           printf("\n");
                  }
         }
}

//Función para renombrar los ficheros
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
         int indexOrigen = BuscaFich (directorio, inodos, nombreantiguo);
         int indexDestino = BuscaFich (directorio, inodos, nombrenuevo);
         //Si el índice del fichero original es -1. Devuelve un mensaje de error
         if (indexOrigen == -1) {
                  printf("ERROR: Fichero %s no encontrado \n", nombreantiguo);
                  return -1; //Retorno -1 para indicar error
         }
         //Si el índice del fichero "nuevo" es distinto de -1. Devuelve un mensaje de error
         if (indexDestino != -1) {
                  printf("ERROR: Fichero %s ya existe \n", nombrenuevo);
                  return -1; //Retorno -1 para indicar error
         }

         strcpy(directorio[indexOrigen].dir_nfich, nombrenuevo); //Cambia el nombre del fichero en el directorio al nuevo nombre
         printf("Fichero %s renombrado a %s \n", nombreantiguo, nombrenuevo);
         
         return 0;
}

//Función para mostrar el contenido del fichero especidicado como un texto
int Imprimir (EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
        int i,j;
        unsigned int blnumber; //Numero de bloque lógico del fichero
        EXT_DATOS datosFichero[MAX_NUMS_BLOQUE_INODO]; //Array para almacenar los datos del fichero
         //Busco el índice del fichero en el directorio
        i = BuscaFich(directorio, inodos, nombre);
         
        if (i > 0) {
           j = 0;
           do {
                    //Obtengo el número del bloque lógico correspondiente
                    blnumber = inodos -> blq_inodos[directorio[i].dir_inodo].i_nbloque[j];
                    //Verifico si el bloque es válido
                    if (blnumber != NULL_BLOQUE) {
                             //Cargo el contenido del bloque en el array
                             datosFichero[j] = memdatos[blnumber - PRIM_BLOQUE_DATOS];
                             printf("%s\n", datosFichero[j].dato);
                    }
                    j++;
           } while (blnumber != NULL_BLOQUE);
        }
        return 0;
}

//Función para borrar el fichero seleccionado
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich) {
         //Busoc el índice del fichero en el directorio
         int index = BuscaFich (directorio, inodos, nombre);

         //Si no encuentra el fichero imprime un mensaje de error
         if (index == -1) {
                  printf("ERROR: Fichero %s no encontrado \n", nombre);
                  return -1;
         }

         //Libero los inodos y los bloques
         unsigned short int inodoIndex = directorio[index].dir_inodo; //índice del inodo asociado al fichero
         //Libero el inodo marcándolo como 0 en el bytemap
         ext_bytemaps -> bmap_inodos[inodoIndex] = 0;

         //Recorro los bloques asociados al inodo y los libero
         for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
                  //Si el bloque no es nulo, los marca como libre en el mapa
                  if (inodos -> blq_inodos[inodoIndex].i_nbloque[i] != NULL_BLOQUE) {
                           ext_bytemaps -> bmap_bloques[inodos -> blq_inodos[inodoIndex].i_nbloque[i]] = 0;
                           //Marcael bloque como nulo en el inodo
                           inodos -> blq_inodos[inodoIndex].i_nbloque[i] = NULL_BLOQUE;
                  }
         }
         //Actualizo el superbloque
         //Actualizo el contador de inodos y de bloques libres
         ext_superblock -> s_free_inodes_count++;
         ext_superblock -> s_free_blocks_count++;
         //Limpio el inodo asociado al ficheor
         inodos -> blq_inodos[inodoIndex].size_fichero = 0;
         //Elimino la entrada del directorio
         strcpy(directorio[index].dir_nfich, ""); //Borra el nombre del fichero
         directorio[index].dir_inodo = NULL_INODO; //Marco el inodo de la entrada como nulo

         printf("Fichero %s borrado\n", nombre);
         return 0;
}

//Función para copiar el contenido de un fichero en otro
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich) {
         int indexOrigen = BuscaFich(directorio, inodos, nombreorigen); //Índice del fichero de origen
         int indexDestino = BuscaFich(directorio, inodos, nombredestino);//Índice del fichero de destino

         //Si el fichero de origen no existe, manda un mensaje de error
         if (indexOrigen == -1) {
                  printf("ERROR: Fichero origen %s no encontrado\n", nombreorigen);
                  return -1;
         }

         //Si el dichero de destino ya existe, manda un mensaje de error
         if (indexDestino != -1) {
                  printf("ERROR: Fichero destino %s ya existe \n", nombredestino);
                  return -1;
         }

         //Encuentro el primer inodo libre y lo marco como ocupado
         int nuevoInodo = -1;
         for (int i = 0; i < MAX_INODOS; i++) {
                  if (ext_bytemaps -> bmap_inodos[i] == 0) {
                           nuevoInodo = i;
                           ext_bytemaps -> bmap_inodos[i] = 1;
                           ext_superblock -> s_free_inodes_count--;
                           break;
                  }
         }

         //Si no hay inodos libres, muestra un error y termina
         if (nuevoInodo == -1) {
                  printf("ERROR: No hay inodos libres\n");
                  return -1;
         }

         //Copio el tamaño del archivo origen al nuevo inodo
         inodos -> blq_inodos[nuevoInodo].size_fichero = inodos -> blq_inodos[directorio[indexOrigen].dir_inodo].size_fichero;
         //Copio bloques de datos y asigno nuevos bloques libres
         for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) { 
                  //Si el bloque del fichero origen no es nulo
                  if (inodos->blq_inodos[directorio[indexOrigen].dir_inodo].i_nbloque[j] != NULL_BLOQUE) { 
                           int nuevoBloque = -1; 
                           //Busca el primer bloque libre y lo marca como ocupado
                           for (int k = 0; k < MAX_BLOQUES_PARTICION; k++) { 
                                    if (ext_bytemaps->bmap_bloques[k] == 0) { 
                                             nuevoBloque = k; //Encuentra un bloque libre
                                             ext_bytemaps->bmap_bloques[k] = 1; //Marca el bloque como ocupado
                                             ext_superblock->s_free_blocks_count--; //Actualiza el contador de bloques libres
                                             break;
                                    } 
                           } 
                           //Si no hay bloques libres, muestra un error y termina
                           if (nuevoBloque == -1) {
                                    printf("ERROR: No hay bloques libres\n"); 
                                    return -1; 
                           } 
                           //Asocia el nuevo bloque al inodo del fichero copiado
                           inodos->blq_inodos[nuevoInodo].i_nbloque[j] = nuevoBloque; 
                           //Copia el contenido del bloque en el fichero de origen al nuevo bloque
                           memcpy(&memdatos[nuevoBloque], &memdatos[inodos->blq_inodos[directorio[indexOrigen].dir_inodo].i_nbloque[j]], SIZE_BLOQUE); 
                  } else { 
                           //Si no hay más bloques en el fichero de origen, marca como nulo
                           inodos->blq_inodos[nuevoInodo].i_nbloque[j] = NULL_BLOQUE; 
                  } 
         }
         //Creo una entrada en la primera vacante disponible del directorio
         for (int i = 0; i < MAX_FICHEROS; i++) { 
                  if (directorio[i].dir_inodo == NULL_INODO) { 
                           //Asigno el nombre y el inodo al nuevo fichero
                           strcpy(directorio[i].dir_nfich, nombredestino); 
                           directorio[i].dir_inodo = nuevoInodo; 
                           printf("Fichero %s copiado a %s\n", nombreorigen, nombredestino); 
                           return 0; 
                  } 
         }
         //En caso de que no hay espacio en el directorio, muestra un error
         printf("ERROR: No hay espacio en el directorio\n");
         return -1;
}

//Función para grabar los i-nodos y el directorio del fichero seleccionado
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
         //Desplaza el puntero del archivo al bloque 3 para escribir el contenido del directorio.
         //El desplazamiento es calculado como 3 * SIZE_BLOQUE.
         fseek(fich, 3 * SIZE_BLOQUE, SEEK_SET);

         //Escribe el contenido del directorio en el archivo.
         //Se escribe 1 bloque de tamaño SIZE_BLOQUE desde la dirección de memoria apuntada por 'directorio'.
         fwrite(directorio, SIZE_BLOQUE, 1, fich);

         //Desplaza el puntero del archivo al bloque 2 para escribir el contenido de los inodos.
         //El desplazamiento es calculado como 2 * SIZE_BLOQUE.
         fseek(fich, 2 * SIZE_BLOQUE, SEEK_SET);

         //Escribe el contenido de los inodos en el archivo.
         //Se escribe 1 bloque de tamaño SIZE_BLOQUE desde la dirección de memoria apuntada por 'inodos'.
         fwrite(inodos, SIZE_BLOQUE, 1, fich);
}

//Función para guardar el bytemap
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
         //Desplaza el puntero del archivo al bloque 1.
         //Los mapas de bytes se almacenan en esta ubicación específica dentro del archivo.
         fseek(fich, SIZE_BLOQUE, SEEK_SET);

         //Escribe el contenido de los mapas de bytes en el archivo.
         //Se escribe 1 bloque de tamaño SIZE_BLOQUE desde la dirección de memoria apuntada por 'ext_bytemaps'.
         fwrite(ext_bytemaps, SIZE_BLOQUE, 1, fich);
}

//Función para guardar el superbloque
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {
         //Desplaza el puntero del archivo al inicio (posición 0) del archivo.
         //El superbloque siempre se encuentra al comienzo del archivo del sistema de archivos.
         fseek(fich, 0, SEEK_SET);

         //Escribe el contenido del superbloque en el archivo.
         //Se escribe 1 bloque de tamaño SIZE_BLOQUE desde la dirección de memoria apuntada por 'ext_superblock'.
         fwrite(ext_superblock, SIZE_BLOQUE, 1, fich);
}

//Función para guardar los datos de un fichero
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {
         //Desplaza el puntero del archivo al bloque 4.
         //Los datos de los bloques comienzan a partir del bloque 4 en el sistema de archivos.
         fseek(fich, 4 * SIZE_BLOQUE, SEEK_SET);

         //Escribe todos los bloques de datos en el archivo.
         //Se escriben MAX_BLOQUES_DATOS bloques de tamaño SIZE_BLOQUE desde la dirección de memoria apuntada por 'memdatos'.
         fwrite(memdatos, SIZE_BLOQUE, MAX_BLOQUES_DATOS, fich);
}
