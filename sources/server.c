#include <serverinc.h>

#ifndef DEBUGMSG
	#define DEBUGMSG 0
#else
	#undef DEBUGMSG
	#define DEBUGMSG 1
#endif

#define DIRSIZE 64

#define TEMPPATH "./music"

/*
typedef struct ServerCtrl {
	int inputfd; 				/ File Descriptor por donde el servidor escucha conexiones /
	int audiofd; 				/ File Descriptor del archivo de audio a enviar /
	struct sockaddr_in my_addr;	/ contendrá la dirección IP y el número de puerto local /
	int audioread; 				/ Bytes leidos del archivo de musica /
	int recibidos; 				/ Bytes recibidos del cliente /
	int forkid; 				/ Process ID de los childs /
	int status; 				/ 0: recibiendo, 1: transmitiendo, 2: reproducir /
} ServerCtrl;

typedef struct ClientInfo {
	char * clientmsg; 			/ Puntero al mensaje que llega del cliente /
	char * clientdirtxt; 		/ Puntero al string donde se 
						 		almacena la ubicacion del cliente en el directorio /
	int sockdup; 				/ Duplicado del socket / 
	DIR * Clientdir; 			/ DIR del directorio manejado por el cliente /
} ClientInfo;
*/

int main ()
{
	/* Genero estructuras y variables */
	ServerCtrl SrvCtrl;
	ClientInfo CltInfo;
	int CheckFile, enviados;

	/* Pido memoria para el buffer del cliente y para el string que almacena la direccion */
	CltInfo.clientmsg = calloc(BUFFSIZE, sizeof(char));
	CltInfo.clientdirtxt = calloc(DIRSIZE, sizeof(char));

	/* Verifico si puedo usar el puerto */
	if((SrvCtrl.inputfd = Open_conection(&SrvCtrl.my_addr)) == -1)
	{
		perror ("[Padre] Falló la creación de la conexión"); 
		exit(1);
	}
	
	/* Copio el path definido en la estructura de la info del cliente */
	strcpy(CltInfo.clientdirtxt, TEMPPATH);
	
	/* Verifico si no se borro la carpeta defecto */
	if((CltInfo.Clientdir = opendir(CltInfo.clientdirtxt)) == NULL) {
		perror("[Padre] Error open DIR");
		exit(0);
	}
	closedir(CltInfo.Clientdir);
	
	/* Programa general */
	printf("[Padre] Servidor iniciado!.\n");
	SrvCtrl.status = 1;
	while(1)
	{
		CltInfo.sockdup = Aceptar_pedidos(SrvCtrl.inputfd);
		if((SrvCtrl.forkid = fork()) != 0)
		{
			printf("[Padre] Creado subproceso ID #%d para el socket %d.\n", SrvCtrl.forkid, CltInfo.sockdup);
			close(CltInfo.sockdup);
		}
		else
		{	
			if((CltInfo.Clientdir = opendir(CltInfo.clientdirtxt)) == NULL)
			{
				perror("[Child] Error open DIR");
				exit(0);
			}
			while(SrvCtrl.status != 2) //Hasta que entre en modo 'reproducir'
			{
				if(DEBUGMSG) printf("\n_______________________________________________________\n\n");
				if(DEBUGMSG) printf("[Child] El servidor ahora esta transmitiendo...\n");

				/* Envio el contenido del directorio en el que se encuentra el cliente */
				if(DEBUGMSG) printf("Enviando directio: \"%s\"\n", CltInfo.clientdirtxt);
				SendDirectoryFiles(CltInfo.Clientdir, CltInfo.sockdup);
				write(CltInfo.sockdup, "-|EOT", 5); //Arreglar, meter en aux
				if(DEBUGMSG) printf("[Child] Handshake enviado.\n");

				/* Ahora el servidor entra en estado de 'escuchar' */
				SrvCtrl.status = 0;
				if(DEBUGMSG) printf("[Child] El servidor ahora esta recibiendo...\n");
				while(SrvCtrl.status == 0)
				{
					SrvCtrl.recibidos = read(CltInfo.sockdup, CltInfo.clientmsg, BUFFSIZE);
					//printf("%s\n", climsg);

					/* Checkeo si esta el Handshake */
					if(checkEndOfTransmission(CltInfo.clientmsg, SrvCtrl.recibidos) == 0) {
						if(DEBUGMSG) printf("[Child] Handshake detectado. Tomando control...\n");
						CltInfo.clientmsg[SrvCtrl.recibidos - 5] = '\0';
						SrvCtrl.status = 1;
					}
					
					if(DEBUGMSG) printf("[Child] client: '%s'\n", CltInfo.clientmsg);

					/* Salida de usuario */
					if(strcmp(CltInfo.clientmsg, "salir") == 0 ||
					   strcmp(CltInfo.clientmsg, "Salir") == 0 ||
					   strcmp(CltInfo.clientmsg, "SALIR") == 0) {
						if(DEBUGMSG) printf("[Child] Comando recibido, terminando conexión.\n");
						
						/* Devuelvo todo */			
						close(SrvCtrl.audiofd);
						close(CltInfo.sockdup);
						free(CltInfo.clientmsg);
						free(CltInfo.clientdirtxt);

						/* Cierro */
						printf("[Child] Finalizada conexion...\n");
						if(DEBUGMSG) printf("[Child] conexión cerrada, terminando proceso hijo.\n");
						exit(0);
					}

					/* Recorto el handshake del mensaje y actualizo directorio */
					if((strcmp(CltInfo.clientmsg, "..") == 0) || strcmp(CltInfo.clientmsg, "carpeta arriba") == 0)
					{
						closedir(CltInfo.Clientdir);
						DirGoUp(CltInfo.clientdirtxt);
						CltInfo.Clientdir = opendir(CltInfo.clientdirtxt);
						if(DEBUGMSG) printf("[Child] El nuevo directorio es: %s\n", CltInfo.clientdirtxt);
					}
					else if(strcmp(CltInfo.clientmsg, ".") == 0)
					{
						closedir(CltInfo.Clientdir);
						CltInfo.Clientdir = opendir(CltInfo.clientdirtxt);
						if(DEBUGMSG) printf("[Child] El directorio queda igual.\n");
					}
					else if(strlen(CltInfo.clientmsg) > 0) //Si el msg es ".." subo de carpeta y ya esta.
					{
						/* Cierro y actualizo directorio */
						closedir(CltInfo.Clientdir);
						InsertDir(CltInfo.clientdirtxt, CltInfo.clientmsg);

						/* Checkeo si es un directorio/archivo */
						CheckFile = FileOrDirCheck(CltInfo.clientdirtxt);
						if(CheckFile == -1) DirGoUp(CltInfo.clientdirtxt); //Si da error vuelvo al anterior.
						else if(CheckFile == 0) DirGoUp(CltInfo.clientdirtxt); //Lo mismo si no es un dir/file.
						else if(CheckFile == 1) {
							if(strstr(CltInfo.clientmsg, ".raw") != NULL) {
								SrvCtrl.status = 2; //Entro en estado de reproduccion, salgo del while.
							}
							else DirGoUp(CltInfo.clientdirtxt);
						}
						//Si es un directorio no hago nada porque ya se actualizo.
						if(DEBUGMSG) printf("[Child] CheckFile result: %d.\n", CheckFile);

						/* Abro el nuevo directorio */
						if(DEBUGMSG) printf("[Child] El nuevo directorio es: %s\n", CltInfo.clientdirtxt);
						CltInfo.Clientdir = opendir(CltInfo.clientdirtxt);
					}

				/* Limpio input buffer */
				memset(CltInfo.clientmsg, '\0', BUFFSIZE);
				if(DEBUGMSG) printf("[Child] Buffer limpiado");
				} //While escuchando al cliente.
			}

			/* Empiezo preparativos para reproducir, verifico el archivo */
			if(DEBUGMSG) printf("[Child] Abriendo: %s\n", CltInfo.clientdirtxt);
			SrvCtrl.audiofd = open(CltInfo.clientdirtxt, O_RDONLY);
			if(SrvCtrl.audiofd == -1)
			{
				perror("[Child] Error opening file");
				close(CltInfo.sockdup);
				continue;
			}

			/* Mando Handshake para avisar que reproduzco audio */
			write(CltInfo.sockdup, "-|REP", 5);
			if(DEBUGMSG) printf("[Child] Handshake de reproduccion enviado.\n");
			
			/* Espero a que mande algo, no importa que */
			if(DEBUGMSG) printf("[Child] Esperando confirmacion.\n");
			SrvCtrl.recibidos = read(CltInfo.sockdup, CltInfo.clientmsg, BUFFSIZE);
			if(DEBUGMSG) printf("[Child] Enviando musica\n");

			/* Empiezo a reproducir */
			SrvCtrl.audioread = 1;
			while(SrvCtrl.audioread > 0)
			{
				if((SrvCtrl.audioread = read(SrvCtrl.audiofd, CltInfo.clientmsg, BUFFSIZE)) == -1)
				{
					perror("[Child] Error reading file");
					close(CltInfo.sockdup);
					continue;
				}
				if((enviados = write(CltInfo.sockdup, CltInfo.clientmsg, SrvCtrl.audioread)) == -1)
				{
					perror("[Child] Error escribiendo mensaje en socket");
					close(CltInfo.sockdup);
					continue;
				}
			}

			/* Devuelvo recursos */
			close(SrvCtrl.audiofd);
			close(CltInfo.sockdup);
			free(CltInfo.clientmsg);
			free(CltInfo.clientdirtxt);
			break;
		}
	}
	printf("[Child] Finalizada conexion...\n");
	exit(0);
}