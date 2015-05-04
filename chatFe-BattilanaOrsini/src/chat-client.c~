#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "parser.h"
#include "common.h"
#include "client_marshall.h"

int main(int argc,char** argv){

	int flagRegistrazione = 0;
	int skt;
	int maxConnessioni = 10;
	hdata_t *user = (hdata_t*)malloc(sizeof(hdata_t));
	char* s = (char*)malloc((strlen(*argv)+1)*sizeof(char));;
	msg_t login;
	bzero(&login,sizeof(msg_t));
	
	if (argc == 1 || argc > 6) {
		printf("Parametri di accesso invalidi.\n");
		return(-1);
	}

	short comando = parser(argc,argv);
	switch (comando) {
		case -1:
			exit(-1);
		break;

		case 1:
			printf("Guida all'uso chat-client (!)\n");
			exit(1);
		break;

		case 2:
			sprintf(s,"%s %s",argv[2],argv[3]);
			user->fullname = (char*)malloc((strlen(s)+1)*sizeof(char));
			strcpy(user->fullname,s);
			user->email = (char*)malloc((strlen(argv[4])+1)*sizeof(char));
			strcpy(user->email,argv[4]);
			user->uname = (char*)malloc((strlen(argv[5])+1)*sizeof(char));
			strcpy(user->uname,argv[5]);
			flagRegistrazione = 1;	
			printf("Reg_log: %s\t%s\t%s\n",user->uname,user->fullname,user->email);
		break;

		case 3:
			user->uname = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
			strcpy(user->uname,argv[1]);
		break;
	}

	struct sockaddr_in server_info;
	bzero(&server_info,sizeof(struct sockaddr_in));

	skt = socket(AF_INET,SOCK_STREAM,0);

	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(3117);
	server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	//server_info.sin_addr.s_addr = inet_addr("192.168.0.112");

	while (maxConnessioni != 0) {
		if (connect(skt,(struct sockaddr*)&server_info,sizeof(server_info)) == 0) {
				printf("CONNESSIONE EFFETTUATA!\n");		
				break;
		} else {
				maxConnessioni--;
				sleep(1);
		}
	}
	if (maxConnessioni == 0) {
		perror("Numero massimo di connessioni fallite raggiunto.Riprova più tardi.\n");
		exit(1);
	}
	if (flagRegistrazione) {
		bzero(&login,sizeof(msg_t));
		login.type = MSG_REGLOG;
		login.msg = (char*)malloc((strlen(user->fullname)+strlen(user->email)+strlen(user->uname)+4)*sizeof(char));
		sprintf(login.msg,"%s:%s:%s",user->uname,user->fullname,user->email);
		login.msglen = strlen(login.msg)+1;
	} else {
		bzero(&login,sizeof(msg_t));
		login.type = MSG_LOGIN;
		login.msg = (char*)malloc((strlen(user->uname)+1)*sizeof(char));
		strcpy(login.msg,user->uname);
		login.msglen = strlen(login.msg)+1;
	}
	char* mail = (char*)malloc((strlen(login.msg)+3)*sizeof(char));
	mail = marshall(login);
	write(skt,mail,(strlen(mail)+1)*sizeof(char));
	
	msg_t* unpacked = (msg_t*)malloc(sizeof(msg_t));
	unmarshall(skt,unpacked);
	printf("%c\t%s\t%s\t%s\t%d\n",unpacked->type,unpacked->sender,unpacked->receiver,unpacked->msg,unpacked->msglen);

	close(skt);

	// Avvio i 2 thread e pa-pa-pa...

	return 0;
}
