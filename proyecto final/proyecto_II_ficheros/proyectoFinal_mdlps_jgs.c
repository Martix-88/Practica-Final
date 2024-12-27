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
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre)
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main(){
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
...

     fent = fopen("particion.bin","r+b");
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
             if (strcmp(orden,"dir")==0) {
            Directorio(&directorio,&ext_blq_inodos);
            continue;
            }
         ...
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
     }

//Función para imprimir el bytemap por pantalla
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) {
         printf("Bytemap de bloques: \n");
         for (int i = 0; i < MAX_BLOQUES_PARTICION; i++) { //Mostrar los primeros 25 elementos del bytemap de bloques
                  printf("%d ", ext_bytemaps -> bmap_bloques[i]);
         }
         printf("\nBytemap de inodos:\n");
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

         if (indexOrigen == -1) {
                  printf("ERROR: Fichero %s no encontrado \n", nombreantiguo);
                  return -1;
         }
         if (indexDestino != -1) {
                  printf("ERROR: Fichero %s ya existe \n", nombrenuevo);
                  return -1;
         }

         strcpy(directorio[indexOrigen].dir_nfich, nombrenuevo);
         printf("Fichero %s renombrado a %s \n", nombreantiguo, nombrenuevo);
}

//Función para mostrar el contenido del fichero especidicado como un texto
int Imprimir (EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
        int i,j;
        unsigned int blnumber;
        EXT_DATOS datosFichero[MAX_NUMS_BLOQUE_INODO]; //Tamaño máximo posible del fichero

        i= BuscaFich(directorio, inodos, nombre);
        if (i > 0) {
           j = 0;
           do {
              blnumber = inodos -> blq_inodos[directorio[i].dir_inodo].i_nbloque[j];
              if (blnumber != NULL_BLOQUE) {
                datosfichero[j] = memdatos[blnumber - PRIM_BLOQUE_DATOS];
              }

             j++;
           } while ("%s\n", datosfichero);
        }

        return -2; //No encontrado
    }
}

//Función para borrar el fichero seleccionado
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich) {
         int index = BuscaFich (directorio, inodos, nombre);

         if (index == -1) {
                  printf("ERROR: Fichero %s no encontrado \n", nombre);
                  return -1;
         }

         //Libero los inodos y los bloques
         unsigned short int inodoIndex = directorio[index].dir_inodo;
         ext_bytemaps -> bmap_inodos[inodoIndex] = 0;

         for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
                  if (inodos -> blq_inodos[inodoIndex].i_nbloque[i] != NULL_BLOQUE) {
                           ext_bytemaps -> bmap_bloques[inodos -> blq_inodos[inodoIndex].i_nbloque[i]] = 0;
                           inodos -> blq_inodos[inodoIndex].i_nbloque[i] = NULL_BLOQUE;
                  }
         }
         //Actualizo el superbloque
         ext_superblock -> s_free_inodes_count++;
         ext_superblock -> s_free_blocks_count++;
         //Limpio el inodo
         inodos -> blq_inodos[inodoIndex].size_fichero = 0;
         //Elimino la entrada del directorio
         strcpy(directorio[index].dir_nfich, "");
         directorio[index].dir_inodo = NULL_INODO;

         printf("Fichero %s borrado\n", nombre);
         return 0;
}

//Función para copiar el contenido de un fichero en otro
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich) {
         int indexOrigen = BuscaFich(directorio, inodos, nombreorigen);
         int indexDestino = BuscaFich(directorio, inodos, nombredestino);

         if (indexOrigen == -1) {
                  printf("ERROR: Fichero origen %s no encontrado\n", nombreorigen);
                  return -1;
         }
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
         
         if (nuevoInodo == -1) {
                  printf("ERROR: No hay inodos libres\n");
                  return -1;
         }

         //Copio el tamaño del archivo origen al nuevo inodo
         inodos -> blq_inodos[nuevoInodo].size_fichero = inodos -> blq_inodos[directorio[indexOrigen].dir_inodo].size_fichero;
         //Copio bloques de datos y asigno nuevos bloques libres
         for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) { 
                  if (inodos->blq_inodos[directorio[indexOrigen].dir_inodo].i_nbloque[j] != NULL_BLOQUE) { 
                           int nuevoBloque = -1; 
                           for (int k = 0; k < MAX_BLOQUES_PARTICION; k++) { 
                                    if (ext_bytemaps->bmap_bloques[k] == 0) { 
                                             nuevoBloque = k; ext_bytemaps->bmap_bloques[k] = 1; 
                                             ext_superblock->s_free_blocks_count--; 
                                             break;
                                    } 
                           } 
                           if (nuevoBloque == -1) {
                                    printf("ERROR: No hay bloques libres\n"); 
                                    return -1; 
                           } 
                           inodos->blq_inodos[nuevoInodo].i_nbloque[j] = nuevoBloque; 
                           memcpy(&memdatos[nuevoBloque], &memdatos[inodos->blq_inodos[directorio[indexOrigen].dir_inodo].i_nbloque[j]], SIZE_BLOQUE); 
                  } else { 
                           inodos->blq_inodos[nuevoInodo].i_nbloque[j] = NULL_BLOQUE; 
                  } 
         }
         //Creo una entrada en la primera vacante disponible del directorio
         for (int i = 0; i < MAX_FICHEROS; i++) { 
                  if (directorio[i].dir_inodo == NULL_INODO) { 
                           strcpy(directorio[i].dir_nfich, nombredestino); 
                           directorio[i].dir_inodo = nuevoInodo; 
                           printf("Fichero %s copiado a %s\n", nombreorigen, nombredestino); 
                           return 0; 
                  } 
         }
         printf("ERROR: No hay espacio en el directorio\n");
         return -1;
}

//Función para grabar los i-nodos y el directorio del fichero seleccionado
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
         fseek(fich, 3 * SZE_BLOQUE, SEEK_SET);
         fwrite(directorio, SIZE_BLOQUE, 1, fich);
         fseek(fich, 2 * SIZE_BLOQUE, SEEK_SET);
         fwrite(inodos, SIZE_BLOQUE, 1, fich);
}

//Función para guardar el bytemap
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
         fseek(fich, SIZE_BLOQUE, SEEK_SET);
         fwrite(ext_bytemaps, SIZE_BLOQUE, 1, fich);
}

//Función para guardar el superbloque
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {
         fseek(fich, 0, SEEK_SET);
         fwrite(ext_superblock, SIZE_BLOQUE, 1, fich);
}

//Función para guardar los datos de un fichero
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {
         fseek(fich, 4 * SIZE_BLOQUE, SEEK_SET);
         fwrite(memdatos, SIZE_BLOQUE, MAX_BLOQUES_DATOS, fich);
}
