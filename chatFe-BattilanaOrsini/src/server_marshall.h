#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LOGIN 'L'
#define MSG_REGLOG 'R'
#define MSG_OK 'O'
#define MSG_ERROR 'E'
#define MSG_SINGLE 'S'
#define MSG_BRDCAST 'B'
#define MSG_LIST 'I'
#define MSG_LOGOUT 'X'

typedef struct {
char			type;
char*			sender;
char*			receiver;
char*			msg;	
unsigned int	msglen;
} msg_t;

char* marshall(msg_t mess){
	char* packet;
	switch (mess.type) {
	case MSG_ERROR:
		packet = (char*)malloc((30 + mess.msglen) * sizeof(char));
		sprintf(packet,"%c~%d~%s~",mess.type,mess.msglen,mess.msg);
	break;
	case MSG_LIST:
		packet = (char*)malloc(30 + mess.msglen * sizeof(char));
		sprintf(packet,"%c~%d~%s~",mess.type,mess.msglen,mess.msg);
	break;
	case MSG_OK:
		packet = (char*)malloc(30 * sizeof(char));
		sprintf(packet,"%c~",mess.type);
	break;
	case MSG_SINGLE:
		mess.msglen += strlen(mess.receiver)+2;
		packet = (char*)malloc((30 + strlen(mess.sender) + mess.msglen)*sizeof(char));
		sprintf(packet,"%c~%d~%s~%s~",mess.type,mess.msglen,mess.sender,mess.msg);
	break;
	case MSG_BRDCAST:
		mess.msglen += 1;
		packet = (char*)malloc((30 + mess.msglen)*sizeof(char));
		sprintf(packet,"%c~%d~%s~",mess.type,mess.msglen,mess.msg);
	break;
	default:
		printf("Server: case default.\n");
	break; 
	}

return packet;
}

void unmarshall(int socket,msg_t* box){
	char *buff = (char*)malloc(30*sizeof(char));
	char *len,*token;
	char tmp;
	int i;
	char type;
	read(socket,&type,sizeof(char));
	switch (type) {
	case MSG_LOGIN:
	case MSG_REGLOG:
	case MSG_BRDCAST:
		i = 0;
		read(socket,&tmp,sizeof(char));
		do{
			read(socket,&tmp,sizeof(char));
			*(buff+i) = tmp;
			i++;	
		}
		while(tmp != '~');
		*(buff+i-1) = '\0';
		box->msglen = atoi(buff);
		box->msg = (char*)malloc(((box->msglen))*sizeof(char));
		read(socket,box->msg,((box->msglen)-1)*sizeof(char));
		read(socket,&tmp,sizeof(char));
	break;
	case MSG_LIST:
	case MSG_LOGOUT:
		read(socket,&tmp,sizeof(char));
	break;
	case MSG_SINGLE:
		i = 0;
		read(socket,&tmp,sizeof(char));
		do{
			read(socket,&tmp,sizeof(char));
			*(buff+i) = tmp;
			i++;	
		}
		while(tmp != '~');
		*(buff+i-1) = '\0';
		box->msglen = atoi(buff);
		len = (char*)malloc((box->msglen)*sizeof(char));
		read(socket,len,(box->msglen)*sizeof(char));
		token = strtok(len,"~");
		box->receiver = (char*)malloc(strlen(token)*sizeof(char));
		strcpy(box->receiver,token);
		token = strtok(NULL,"~");
		box->msg = (char*)malloc(strlen(token)*sizeof(char));
		strcpy(box->msg,token);
	break;
	default:
		printf("Case default non previsto.\n");
	break;
	}
	box->type = type;
}
