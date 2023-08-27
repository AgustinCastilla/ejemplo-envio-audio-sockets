#include <soundlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void set_audio_params (int fd, int rate, int channels, int size)
{
	int 	arg;		/* argumento para ioctl */
	int	status;		/* salida de ioctl */
/* seteamos los parametros de muestreo  */
	arg = size;	   /* arg = Tamaño de muestra */
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg); 
/* SOUND_PCM_WRITE_BITS es la macro que escribe en la placa de sonido (a través del driver por
supuesto), el valor del tamaño de muestra en bits que seteamos en la variable arg, cuyo puntero
se provee como tercer argumento a ioctl().
La interfaz genérica de audio manejada por streams traduce comandos generales a los comandos
particulares para el hardware de modo transparente al programador */
	if (status == -1) 
		perror("Error con comando SOUND_PCM_WRITE_BITS");
/* El hecho que ioctl devuelva error no quiere decir que no se haya configurado el parámetro 
que se quiso programar. 
Por lo general la variable pasada por referencia vuelve con el valor programado si el original
no estaba entre los posibles valores del parámetro que se quiso configurar. Por ejemplo, Tamaño
de muestra: si arg hubiese ido en un valor por ejemplo 13, o 15, seguramente no  coincidiría
con los valores posibles para configurar el conversor A/D de la placa de sonido. 
En tal caso el driver programa al valor mas cercano al argumento recibido,modifica la variable 
recibda como referencia al valor, para que el programa que lo envió pueda saber el valor con el
que se configuró, y hace que la función devuelva -1, para que se pueda analizar el error y tomar
nota del valor configurado finalmente. */
	if (arg != size)
		fprintf (stderr,"Tamaño de muestras no soportado. Se programó %d\n",arg);

	arg = channels;  /* mono o stereo */
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status == -1)
		perror("Error en comando SOUND_PCM_WRITE_CHANNELS");
	if (arg != channels)
		fprintf (stderr,"Cantidad de canales no soportado. Se programó %d\n",arg);

	arg = rate;	   /* Velocidad de Muestreo */
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
	if (status == -1)
		perror("Error en comando SOUND_PCM_WRITE_RATE");
	if (arg != rate)
		fprintf (stderr,"Velocidad de muestreo no soportada. Se programó %d\n",arg);

	return;
}