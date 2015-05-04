// parser.h [-h] [-r chiave] chiave  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parser(int argc, char** argv) {

int commandLine = 0;

//printf("Hai inserito comando con opzione\n");
if (strcmp(argv[1],"-h") == 0) {
	//printf("Hai inserito opzione di help\n");
	commandLine = 1;
} else if (strcmp(argv[1],"-r") == 0) {
	//printf("Hai inserito opzione di registrazione\n");
	if (strstr(argv[2],":") != NULL || strstr(argv[3],":") != NULL 
		|| strstr(argv[4],":") != NULL || strstr(argv[5],":") != NULL 
		|| strncmp(argv[5],"-",1) == 0) {
		printf("Parametri di registrazione invalidi.[:]\n");
		commandLine = -1;
	} else {
		//printf("Username fornito: %s",username);
		commandLine = 2;		
	} 
}else {
	//printf("Comando con chiave\n");
	if (strstr(argv[1],":") != NULL || strncmp(argv[1],"-",1) == 0) {
		printf("Username invalido.[:]\n");
		commandLine = -1;
	} else {
		//printf("Username fornito: %s",username);
		commandLine = 3;
	}
}

return commandLine;
}



