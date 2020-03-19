# Subnetting

Este programa sirve para identificar la estructura de una red, dada una IP y su Màscara.

## Prerequisitos

Realice los pasos a continuación para instalar el compilador GCC en Ubuntu 18.04:

Comience actualizando la lista de paquetes:

 ```
 $ sudo apt update
 ```
 Instale el paquete build-essential escribiendo:
 
 ```
 sudo apt install build-essential
 ```
 
El comando instala un montón de paquetes nuevos que incluyen gcc, g ++ y make.
También puede instalar las páginas del manual sobre el uso de GNU / Linux para el desarrollo:

```
sudo apt-get install manpages-dev
```

Para validar que el compilador GCC se instaló correctamente, use el comando gcc --version que imprime la versión GCC:

 ```
gcc --version
 ```

## Compilaciòn
 * $ cd /ruta/hacia/elarchivo/
 * $ gcc calredes.c -o nombreoutput

## Ejecuciòn del programa
Una vez compilado el programa podemos ejecutarlo con el siguiente comando:

```bash
./nombreoutput <ipred> <mascarared>
```
Donde el formato es:
ipred: x.x.x.x
mascarared: x.x.x.x

## Ejemplo de salida

 ```
 ./calredes 192.168.10.0 255.255.255.0
Network:        192.168.10.0/24
Wildcard:       0.0.0.255
Netmask:        255.255.255.0
Hostmin:        192.168.10.1
Hostmax:        192.168.10.254
Broadcast:      192.168.10.255
Hosts:          254
 ```

