# ejemplo-envio-audio-sockets

Cliente y Servidor que permite al cliente navegar por los directorios de una carpeta específica del servidor y reproducir archivos de audio '.raw' ubicados en el servidor.

# Preparado

## Instalacion

Se necesitan los siguientes paquetes para la reproduccion de audio:
```
sudo apt-get install alsa-oss
sudo apt-get install alsa-base
sudo apt-get install alsa-utils
```

Para otros equipos también puede ser utiliando alguno de los siguientes paquetes:
```
sudo apt-get install osspd-alsa
sudo apt-get install osspd
```

Puede utilizar el siguiente comando si sigue tirando errores:
```
sudo alsa force-reload
```

## Compilado:

Utilice el siguiente comando para compilar todos los archivos:
```
make all
```

# Uso (Cliente)

Luego de conectarse simplemente escriba el nombre de la carpeta a la que desea acceder.
Si quiere ir una carpeta arriba escriba "carpeta arriba" (todo en minúsculas) o "..".
Para reproducir un archivo tipo '.raw' debe escribir el nombre completo incluyendo la extensión.
Si desea desconectarse escriba "salir".

# Pipes

En caso de tener problemas con los pipes puede crear nuevos con el comando:
```
mkfifo [nombre]
```

# Creación de archivo RAW

Si desea crear un archivo RAW teniendo el audio en otro formato (mp3 por ejemplo) puede utiliar la siguiente herramienta:
```
sudo apt install ffmpeg
```
y luego:
```
ffmpeg -y -i [input] -acodec pcm_s16le -f s16le -ac 2 -ar 48000 [output]
```

Asegurese de que el output file es extensión '.raw'.

# Libreria

El proyecto utiliza la libreria proporcionada por la catedra de Informática 1 de la UTN FRBA (Alejandro Furfaro).
