#include <clientinc.h>

/* Busca el '-/EOT' al final del string */
int checkEndOfTransmission(char * txt, int txtsize)
{
	if(strcmp((txt + txtsize) - 5, "-|EOT") == 0) return 0;
	else if(strcmp((txt + txtsize) - 5, "-|REP") == 0) return 1;
	//if(strcmp((txt + txtsize) - 5, "-/UP-") == 0) return 2;
	//if(strcmp((txt + txtsize) - 5, "-/PL-") == 0) return 3;
	return -1;
}