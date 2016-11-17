/**
 * @file   userInterface.c
 * @author Facundo Maero, Gustavo Gonzalez
 * @date   Noviembre 2016
 * @version 0.1
 * @brief Un programa en C que funciona como interface entre los modulos del Kernel
 *        y el usuario, mostrando en pantalla diferentes mensajes informativos
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "colores.h"
 
#define BUFFER_LENGTH 80               ///< Longitud del buffer
static char recieve[BUFFER_LENGTH];     ///< Buffer modificado por el modulo a llamar
 
int main(){
   int fdEnc, fdDes;
   char stringToSend[BUFFER_LENGTH];

   printf(BOLDBLUE "Encriptador de mensajes del Kernel de Linux\n" RESET);
   printf("Ingrese la frase a encriptar.\n");
   printf("Para salir, ingrese " BOLDRED "'exit'\n" RESET);

   fdEnc = open("/dev/charEncryptor", O_RDWR);             // Abro el archivo con permisos de lectoescritura
   if (fdEnc < 0){
      perror(RED "Error al abrir el Encryptor..." RED);
      return errno;
   }
   fdDes = open("/dev/charDesencryptor", O_RDWR);             // Abro el archivo con permisos de lectoescritura
   if (fdDes < 0){
      perror(RED"Error al abrir el Desencryptor..."RED);
      return errno;
   }

   while(1){
      int ret, flag=1, result;

      result = scanf("%[^\n]%*c", stringToSend);                // Leo un string con espacios
      if(result<=0){
         perror(RED "Error al leer el mensaje\n" RESET);
      }

      flag = strcmp(stringToSend,"exit");
      if(flag == 0){
         break;
      }

      printf("Enviando mensaje al encriptador: " BOLDBLUE "%s\n" RESET, stringToSend);

      ret = write(fdEnc, stringToSend, strlen(stringToSend)); // Envio el string al encriptador escribiendo en el archivo dev
      if (ret < 0){
         perror(RED "Error al escribir el archivo del modulo." RESET);
         return errno;
      }
    
      ret = read(fdEnc, recieve, BUFFER_LENGTH);        // Leo la respuesta del modulo
      if (ret < 0){
         perror(RED "Error al leer la respuesta del modulo." RESET);
         return errno;
      }
      printf("El mensaje encriptado es: " BOLDMAGENTA "%s\n" RESET, recieve);

      printf("Desencriptando el mensaje...\n");
      strcpy(stringToSend, recieve);
      ret = write(fdDes, stringToSend, strlen(stringToSend)); // Envio el string al encriptador escribiendo en el archivo dev
      if (ret < 0){
         perror(RED "Error al escribir el archivo del modulo." RESET);
         return errno;
      }
    
      printf("Presione ENTER para ver el resultado...");
      getchar();
    
      ret = read(fdDes, recieve, BUFFER_LENGTH);        // Leo la respuesta del modulo
      if (ret < 0){
         perror(RED "Error al leer la respuesta del modulo." RESET);
         return errno;
      }
      printf("El mensaje recuperado es: " BOLDBLUE "%s\n" RESET, recieve);
      printf("\n=========================================================");
      printf("\nPor favor ingrese otro mensaje:\n");
   }
   
   close(fdEnc);
   close(fdDes); 
   printf("Saliendo del programa...\n");
   return 0;
}