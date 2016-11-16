# TP3-SO-2016 

### Laboratorio 3 - Desarrollo de módulos para el Kernel de Linux - 2016 
![Icono](https://cdn0.iconfinder.com/data/icons/security-pro-set4/512/Encryption-01-128.png)
___
|Autores                               |
|------------------------------------|
|Gonzalez Somerstein Gustavo -7721064  |
|Maero Facundo - 38479441              |

## 1. Descripción del trabajo
---
Este práctico cuenta con dos módulos para el Kernel.
El primero, **charEncryptor** recibe una cadena de caracteres y la codifica byte a byte. 
Luego, esta **charDesencryptor**, que realiza el trabajo inverso: recibe una cadena encriptada y la traduce de nuevo a una sentencia legible.
Para poder trabajar con estos dos módulos, se cuenta también con un programa simple en C llamado **userInterface** que permite al usuario utilizar ambos módulos y ver los resultados que arroja cada uno en consola.

## 2. Instrucciones de instalación y de uso
---
 - Para instalar los módulos, descargue el repositorio en su computadora.
 - Luego, mediante la terminal, ejecute los siguientes comandos para descomprimir el archivo .zip y situarse en la carpeta donde se halla el Makefile:
```
% cd Downloads
% unzip TP3-SO-2016-master.zip
% cd TP3-SO-2016-master/work/code
```
 - Para comenzar el proceso de compilación, ejecute el comando:
```
% make
```
 
 - Una vez terminado, deberá instalar los módulos en el Kernel. Para ello ejecute: 
```
% sudo insmod charEncryptor.ko
% sudo insmod charDesencryptor.ko
```
- Finalmente, para ejecutar el programa de interfaz con los módulos, ingrese:
```
% sudo ./userInterface
```
Al compilar y linkear, se genera un archivo donde se guardan los posibles errores y advertencias que encuentre el programa CppCheck al realizar el análisis estático del código. Este archivo se encuentra en:
```
shell/work/code/err.txt
```
Si desea más información, remítase a la documentación proporcionada, que se encuentra en la ruta ```doc/html/index.html```
