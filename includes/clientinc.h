#include <sock-lib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <soundlib.h>
#include <sys/ioctl.h>

int checkEndOfTransmission(char * txt, int txtsize);

typedef struct ClientCtrl {
	int inputfd; 				/* File Descriptor por donde el cliente escucha conexiones */
	int dspfd; 					/* File Descriptor de la placa de audio */
	int recibidos; 				/* Bytes recibidos del servidor */
	char * servermsg;			/* Puntero al mensaje que llega del servidor */
	int status; 				/* 0: recibiendo, 1: transmitiendo, 2: reproducir */
} ClientCtrl;