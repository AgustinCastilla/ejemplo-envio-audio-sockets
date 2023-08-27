#include <clientinc.h>

#ifndef DEBUGMSG
	#define DEBUGMSG 0
#else
	#undef DEBUGMSG
	#define DEBUGMSG 1
#endif

#define MAXDATASIZE 4096 /* máxima cantidad de bytes que puede recibir en una transacción */
#define MSEG 200 // Si lo subo mucho se escucha entre-cortado
#define RATE 48000
#define SIZE 16
#define CHANNELS 2
#define BUFFSIZE MSEG*RATE*SIZE*CHANNELS/8/1000
#define INPUTBUFFSIZE 64

/*
typedef struct ClientCtrl {
	int inputfd; 				/ File Descriptor por donde el cliente escucha conexiones /
	int dspfd; 					/ File Descriptor de la placa de audio /
	int recibidos; 				/ Bytes recibidos del servidor /
	char * servermsg;			/ Puntero al mensaje que llega del servidor /
	int status; 				/ 0: recibiendo, 1: transmitiendo, 2: reproducir /
} ClientCtrl;
*/

int main(int argc, char * argv[])
{
	/* Genero estructuras y variables */
	ClientCtrl CltCtrl;
	char * ClientInput;
	int Handshake;

	/* Pido memoria para el buffer del servidor */
	CltCtrl.servermsg = calloc(BUFFSIZE, sizeof(char));
	ClientInput = calloc(INPUTBUFFSIZE, sizeof(char));

	/* Tratamiento de la línea de comandos. */
	if(argc < 2)
	{
		fprintf(stderr, "Uso: %s hostname [port]\n", argv [0]);
		exit(1);
	}

	/* Mensaje de bienvenida */
	for(int k = 0; k < 2; k ++)
	{
		for(int i = 0; i < 4; i ++)
		{
			printf("\x1b[H\x1b[J");
			printf("¡Gracias por utilizar spotifai!\n\n");
			printf("Conectando con el servidor");
			for(int j = 1; j <= i; j ++) printf(".");
			printf("\n");
			//sleep(1);
		}
	}

	/* Conectamos */
	CltCtrl.inputfd = conectar(argc, argv);

	/* abrimos el dispositivo de audio*/
	if((CltCtrl.dspfd = open("/dev/dsp", O_WRONLY)) < 0)
	{ 
		perror("Error open dsp!"); 
		exit(1);
	}
	set_audio_params(CltCtrl.dspfd, RATE, CHANNELS, SIZE);

	/* Recibimos los datos del servidor */
	/* Visualizamos lo recibido */	
	CltCtrl.status = 0;
	while(CltCtrl.status != 2)
	{
		printf("\x1b[H\x1b[J");
		printf("=======================================================\n");
		while(CltCtrl.status == 0)
		{
			/* Leo... */
			if(DEBUGMSG) printf("[Cliente] El cliente ahora esta recibiendo...\n");
			CltCtrl.recibidos = read(CltCtrl.inputfd, CltCtrl.servermsg, BUFFSIZE);
			/* Verifico data */
			if(CltCtrl.recibidos == -1)
			{
				perror("[Cliente] Error de lectura en el socket.");
				exit(1);
			}

			/* Imprimo y checkeo EOT */
			/* Agregar segundo HS para saber cuando tiene que reproducir el audio */
			CltCtrl.servermsg[CltCtrl.recibidos] = '\0';
			printf("%s", CltCtrl.servermsg);
			Handshake = checkEndOfTransmission(CltCtrl.servermsg, CltCtrl.recibidos); 
			if(Handshake == 0) {
				if(DEBUGMSG) printf("[Cliente] Handshake detectado. Tomando control...\n");
				printf("\n=======================================================\n");
				printf("¿A que carpeta/archivo desea acceder?: ");
				CltCtrl.status = 1;
			}
			else if(Handshake == 1) {
				if(DEBUGMSG) printf("[Cliente] Handshake detectado. Reproduciendo...\n");
				CltCtrl.status = 2;
				printf("\x1b[H\x1b[J");
			}

			/* Limpio input buffer */
			memset(CltCtrl.servermsg, '\0', BUFFSIZE);
		}

		/* Checkeo si no es para play y mando texto y EOT */
		if(CltCtrl.status != 2)
		{
			/* El cliente escribe */
			if(DEBUGMSG) printf("[Cliente] El cliente ahora esta transmitiendo...\n");
			gets(ClientInput);

			/* Mando mensaje y handshake */
			write(CltCtrl.inputfd, ClientInput, INPUTBUFFSIZE);
			write(CltCtrl.inputfd, "-|EOT", 5);
			if(DEBUGMSG) printf("[Cliente] Handshake enviado.\n");

			/* Salida del usuario */
			if(strcmp(ClientInput, "salir") == 0 ||
			   strcmp(ClientInput, "Salir") == 0 ||
			   strcmp(ClientInput, "SALIR") == 0)
			{	

				printf("=======================================================\n");
				printf("¡Hasta luego!\n\n");
				/* Devolvemos recursos al sistema */
				close(CltCtrl.inputfd);
				close(CltCtrl.dspfd);
				free(CltCtrl.servermsg);
				exit(0);
			}

			/* Vuelvo a escuchar */
			CltCtrl.status = 0;
		}
	}

	/* Envio confirmacion */
	write(CltCtrl.inputfd, "-|CON", 5);

	/* Play audio */
	printf("¡Disfrute su cancion!\n");
	while(CltCtrl.recibidos > 0)
	{
		CltCtrl.recibidos = read(CltCtrl.inputfd, CltCtrl.servermsg, BUFFSIZE);
		
		write(CltCtrl.dspfd, CltCtrl.servermsg, BUFFSIZE);
  		ioctl(CltCtrl.dspfd, SOUND_PCM_SYNC, 0);
	}

	printf("=======================================================\n");
	printf("¡Hasta luego!\n\n");

	/* Devolvemos recursos al sistema */
	close(CltCtrl.inputfd);
	close(CltCtrl.dspfd);
	free(CltCtrl.servermsg);
	return 1;
}