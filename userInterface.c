/**
 * @file   userInterface.c
 * @author Facundo Maero, Gustavo Gonzalez
 * @date   Noviembre 2016
 * @version 0.1
 * @brief Un programa en C que funciona como interface entre los modulos del Kernel
 *        y el usuario, mostrando en pantalla diferentes mensajes informativos
 *
*/
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256               ///< Longitud del buffer
static char recieve[BUFFER_LENGTH];     ///< Buffer modificado por el modulo a llamar
 
int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Encriptador de mensajes del Kernel de Linux\n");
   fd = open("/dev/charEncryptor", O_RDWR);             // Abro el archivo con permisos de lectoescritura
   if (fd < 0){
      perror("Error al abrir el dispositivo...");
      return errno;
   }
   printf("Ingrese la frase a encriptar:\n");
   scanf("%[^\n]%*c", stringToSend);                // Leo un string con espacios
   printf("Enviando mensaje al encriptador: [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); // Envio el string al encriptador escribiendo en el archivo dev
   if (ret < 0){
      perror("Error al escribir el archivo del modulo.");
      return errno;
   }
 
   printf("Presione ENTER para ver el mensaje encriptado...\n");
   getchar();
 
   printf("Leyendo el mensaje...\n");
   ret = read(fd, recieve, BUFFER_LENGTH);        // Leo la respuesta del modulo
   if (ret < 0){
      perror("Error al leer la respuesta del modulo.");
      return errno;
   }
   printf("El mensaje encriptado es: [%s]\n", recieve);
   close(fd);






   fd = open("/dev/charDesencryptor", O_RDWR);             // Abro el archivo con permisos de lectoescritura
   if (fd < 0){
      perror("Error al abrir el dispositivo...");
      return errno;
   }
   printf("Ingrese la frase a desencriptar:\n");
   scanf("%[^\n]%*c", stringToSend);                // Leo un string con espacios
   printf("Enviando mensaje al encriptador: [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); // Envio el string al encriptador escribiendo en el archivo dev
   if (ret < 0){
      perror("Error al escribir el archivo del modulo.");
      return errno;
   }
 
   printf("Presione ENTER para ver el mensaje desencriptado...\n");
   getchar();
 
   printf("Leyendo el mensaje...\n");
   ret = read(fd, recieve, BUFFER_LENGTH);        // Leo la respuesta del modulo
   if (ret < 0){
      perror("Error al leer la respuesta del modulo.");
      return errno;
   }
   printf("El mensaje desencriptado es: [%s]\n", recieve);
   close(fd);


   return 0;
}