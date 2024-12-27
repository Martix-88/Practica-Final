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
         
}

//Función para listar todos los ficheros
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
  
}

//Función para renombrar los ficheros
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
  
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
  
}

//Función para copiar el contenido de un fichero en otro
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich) {
  
}

//Función para grabar los i-nodos y el directorio del fichero seleccionado
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
  
}

//Función para guardar el bytemap
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
  
}

//Función para guardar el superbloque
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {
  
}

//Función para guardar los datos de un fichero
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {
  
}
