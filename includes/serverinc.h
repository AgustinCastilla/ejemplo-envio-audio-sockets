#include <sock-lib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define MSEG 200
#define RATE 48000
#define SIZE 16
#define CHANNELS 2
#define BUFFSIZE MSEG*RATE*SIZE*CHANNELS/8/1000

int checkEndOfTransmission(char * txt, int txtsize);
int FileOrDirCheck(char * strdir);
int DirGoUp(char * dir);
int InsertDir(char * strdir, char * endstrdir);
int SendDirectoryFiles(DIR * Dir, int filedescriptor);

typedef struct ServerCtrl {
	int inputfd; 				/* File Descriptor por donde el servidor escucha conexiones */
	int audiofd; 				/* File Descriptor del archivo de audio a enviar */
	struct sockaddr_in my_addr;	/* contendrá la dirección IP y el número de puerto local */
	int audioread; 				/* Bytes leidos del archivo de musica */
	int recibidos; 				/* Bytes recibidos del cliente */
	int forkid; 				/* Process ID de los childs */
	int status; 				/* 0: recibiendo, 1: transmitiendo, 2: reproducir */
} ServerCtrl;

typedef struct ClientInfo {
	char * clientmsg; 			/* Puntero al mensaje que llega del cliente */
	char * clientdirtxt; 		/* Puntero al string donde se 
						 		almacena la ubicacion del cliente en el directorio */
	int sockdup; 				/* Duplicado del socket */ 
	DIR * Clientdir; 			/* DIR del directorio manejado por el cliente */
} ClientInfo;