#include <serverinc.h>

#ifndef DEBUGMSG
	#define DEBUGMSG 0
#else
	#undef DEBUGMSG
	#define DEBUGMSG 1
#endif

/* Busca el '-|EOT' al final del string */
int checkEndOfTransmission(char * txt, int txtsize)
{
	if(strcmp((txt + txtsize) - 5, "-|EOT") == 0) return 0;
	//if(strcmp((txt + txtsize) - 5, "-|IN-") == 0) return 1;
	//if(strcmp((txt + txtsize) - 5, "-|UP-") == 0) return 2;
	//if(strcmp((txt + txtsize) - 5, "-|PL-") == 0) return 3;
	return -1;
}

/* Verifica si es un archivo, directorio, u otro.
 * Valores de retorno:
 * -1: Error.
 * 0: Otro.
 * 1: Archivo.
 * 2: Directorio.
*/
int FileOrDirCheck(char * strdir)
{
	struct stat FileStat;
	if(stat(strdir, &FileStat) != -1)
	{
		if((FileStat.st_mode & S_IFMT) == S_IFREG) return 1;
		else if((FileStat.st_mode & S_IFMT) == S_IFDIR) return 2;
		else return 0;
	}
	else return -1;
}

/* Elimina la ultima direccion de la cadena de carpetas */
int DirGoUp(char * dir)
{
	if(strcmp(dir, "./music") == 0) return 0; //Para que no suban mas de lo permitido
	char * ch;
	ch = strrchr(dir, '/');
	if(ch == NULL) return 0;
	*(ch) = '\0';
	return 1;
}

/* Inserta una cadena en otra (basicamente) separadas por una barra */
int InsertDir(char * strdir, char * endstrdir)
{
	return sprintf(strdir, "%s/%s", strdir, endstrdir);
}

/* Envia por filedescriptor una lista de todos los directorios y archivos terminados en .raw */
int SendDirectoryFiles(DIR * Dir, int filedescriptor)
{
	struct dirent * FileStru;
	char aux[BUFFSIZE];

	/* Imprimo la opcion de salir */
	strcpy(aux, "Salir (x)\n");
	write(filedescriptor, aux, BUFFSIZE);
	if(DEBUGMSG) printf("Salir (x)\n");

	/* Imprimo directorios y archivos */
	while((FileStru = readdir(Dir)) != NULL) //Abro directorio
	{
		if(FileStru->d_type == DT_DIR && strcmp(FileStru->d_name, "..") == 0) //Si es directorio y se llama ".."
		{
			/* Imprimo opcion de carpeta arriba */
			strcpy(aux, "Carpeta Arriba (<┘)\n");
			if(write(filedescriptor, aux, BUFFSIZE)!=BUFFSIZE) {
				fprintf(stderr,"Error al enviar por socket\n");
				fprintf(stderr,"%s:%i %i,%s\n",__func__,__LINE__,errno,strerror(errno));
			}
			if(DEBUGMSG) printf("Carpeta Arriba (<┘)\n");
		}
		else if(FileStru->d_type == DT_DIR && strcmp(FileStru->d_name, ".") != 0) //Si es dir y no es "."
		{
			strcpy(aux, FileStru->d_name);
			strcat(aux, " (<┘)\n");
			if (write(filedescriptor, aux, BUFFSIZE)!=BUFFSIZE) {
				fprintf(stderr,"Error al enviar por socket\n");
				fprintf(stderr,"%s:%i %i,%s\n",__func__,__LINE__,errno,strerror(errno));
			}
			if(DEBUGMSG) printf("%s", aux);
		}
		else if(strstr(FileStru->d_name, ".raw") != NULL) //Si tiene ".raw" en el nombre
		{
			strcpy(aux, FileStru->d_name);
			strcat(aux, " (♫ )\n");
			if (write(filedescriptor, aux, BUFFSIZE)!=BUFFSIZE) {
				fprintf(stderr,"Error al enviar por socket\n");
				fprintf(stderr,"%s:%i %i,%s\n",__func__,__LINE__,errno,strerror(errno));
			}
			if(DEBUGMSG) printf("%s", aux);
		}
	}
	return 1;
}