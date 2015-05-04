#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "server_marshall.h"
#include "hash.h"
#include "timestamp.h"

int go = 1 , numThreadAttivi = 0;
hash_t table;
pthread_mutex_t mux1 , muxLog;

typedef struct _thread_arg {
	char * upath;
	char * lpath;
	int socket;
} thread_arg;

void * thread_main(void * targ);
void * thread_dispatcher(void * targ);
void * thread_worker(void * targ);

void writeLog(char * path, char * msg, int flagIntestazione) {
	FILE * fp = fopen(path,"a+");
	char ts[64]; timestamp(ts);
	pthread_mutex_lock(&muxLog);
	if ( flagIntestazione ) {
		if (fprintf( fp , "%s", msg) < 0 ) {
		perror("Errore durante scrittura log-file\n.");
		}	
	} else {
		if (fprintf( fp , "%s:%s\n", ts, msg) < 0 ) {
		perror("Errore durante scrittura log-file\n.");
		}	
	}
	fclose(fp);
	pthread_mutex_unlock(&muxLog);
}

void chat_server (int argc,char** argv) {
	if (argc != 3) {
		perror("Due argomenti richiesti.\n");
		exit(-1);
	}

	thread_arg targ;
	targ.upath = (char*)malloc((strlen(argv[1])+4)*sizeof(char));
	targ.lpath = (char*)malloc((strlen(argv[2])+4)*sizeof(char));
	strcat(strcpy(targ.upath,"../"),argv[1]);
	strcat(strcpy(targ.lpath,"../"),argv[2]);
	
	FILE * fp;
	if ( (fp = fopen(targ.upath,"r")) == NULL ) {
		fp = fopen(targ.upath,"w+");
		fclose(fp);
	}

	if ( (fp = fopen(targ.lpath,"r")) == NULL ) {
		fopen(targ.lpath,"w+");
		fclose(fp);
	} else {
		fp = fopen(targ.lpath,"w+");
		char  intestazione[160];
		time_t t = time(NULL);  	
		sprintf( intestazione , "********************************************************\n Chat Server started @ %s\n********************************************************\n" , ctime(&t) );
		writeLog( targ.lpath , intestazione , 1 );
		sprintf( intestazione , "\t\tDATA\t\t\t\tEVENTO\t\t\tNOME EVENTO\n\n");
		writeLog( targ.lpath , intestazione , 1 );
		fclose(fp);	
	}


	pthread_t main,dispatcher;	
	if ( pthread_create(&main , NULL , (void*) thread_main , (void*) &targ) ) {
		perror("ERRORE: Creazione thread main.\n");
		exit(-1);
	}

	if ( pthread_create(&dispatcher , NULL , (void*) thread_dispatcher , (void*) &targ) ) {
		perror("ERRORE: Creazione thread dispatcher.\n");
		exit(-1);
	} 
	pthread_join(main,NULL);
}

void * thread_main(void * targ) {
	thread_arg * arg = (thread_arg*)targ;
	table = CREAHASH();	
	hdata_t  * userdata = (hdata_t*)malloc(sizeof(hdata_t));
	int msd,csd;
	struct sockaddr_in my_addr;
	pthread_t worker;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr , PTHREAD_CREATE_DETACHED);
	pthread_mutex_init(&mux1 , NULL);
	pthread_mutex_init(&muxLog , NULL);
	FILE * ufile, * lfile;
	char * token;
	char * s = (char*)malloc(771*sizeof(char));

	ufile = fopen( (*arg).upath , "a+");
	while( fgets( s , 771 , ufile) != NULL ) {
		userdata = (hdata_t*)malloc(sizeof(hdata_t));
		userdata->uname = (char*)malloc(256*sizeof(char));
		userdata->fullname = (char*)malloc(256*sizeof(char));
		userdata->email = (char*)malloc(256*sizeof(char));

		token = strtok(s,":");
		strcpy( userdata->uname , token );
		token = strtok(NULL,":");
		strcpy( userdata->fullname , token );
		token = strtok(NULL,"\n");
		strcpy( userdata->email , token );
		userdata->sockid = -1;
		INSERISCIHASH(userdata->uname ,(void*) userdata , table);
	}
	//PRINTTABLE ( table );
	
	bzero(&my_addr,sizeof(struct sockaddr_in));
	msd = socket(AF_INET,SOCK_STREAM,0);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(3117); 
	my_addr.sin_addr.s_addr = INADDR_ANY;

	bind(msd,(struct sockaddr*)&my_addr,sizeof(my_addr));
	listen(msd,10);
		
	thread_arg * workerarg = (thread_arg*) malloc (sizeof(thread_arg));
	while ( go ) {
		workerarg->socket = accept(msd,NULL,0);
		workerarg->lpath = (char*) malloc(strlen((*arg).lpath)*sizeof(char));
		workerarg->lpath = (*arg).lpath;
		if ( pthread_create(&worker , &attr , (void*) thread_worker , (void*) workerarg) ) {
			perror("ERRORE: Creazione thread worker.");
			exit(-1);
		}
		pthread_mutex_lock(&mux1);
		numThreadAttivi++;	
		pthread_mutex_unlock(&mux1);
	}
	
	close(msd);
	pthread_exit(0);
}

void* thread_worker(void* targ) {
	thread_arg* arg = (thread_arg*) targ;
	int csd = (int)(arg->socket);
	msg_t* box = (msg_t*)malloc(sizeof(msg_t));
	hdata_t* match = NULL;
	char* packet;
	int i;	
	unmarshall(csd , box);
	fprintf(stderr,"Box->Type: **%c** \t Box->msg: **%s**\n",box->type,box->msg);
	switch(box->type){
	case MSG_LOGIN:
		match = CERCAHASH( box->msg , table);
		if (match && match->sockid == -1) {
			match->sockid = csd;	
			char * logstring = (char*) malloc ( (strlen(box->msg)+strlen("login") + 1) * sizeof(char));
			sprintf(logstring,"\tlogin:\t      %s",box->msg);
			writeLog( arg->lpath , logstring , 0 );					
			bzero(box , sizeof(msg_t));
			box->type = MSG_OK;
			packet = (char*)malloc(2*sizeof(char));
			packet = marshall(*box);
			write(csd , packet , (strlen(packet))*sizeof(char));		
		}
		else if(match && match->sockid != -1 ){
				bzero(box , sizeof(msg_t));
				box->type = MSG_ERROR;
				box->msg = (char*)malloc((strlen("Errore: username gia' loggato")+1)*sizeof(char));
				strcpy(box->msg,"Errore: username gia' loggato");
				box->msglen = strlen(box->msg)+1;
				char * logstring = (char*) malloc ( (strlen(box->msg)+strlen("registrazione") + 1) * sizeof(char));
				sprintf(logstring,"\tregistrazione:\t%s",box->msg);
				writeLog( arg->lpath , logstring , 0 );
				packet = (char*)malloc((box->msglen+2)*sizeof(char));
				packet = marshall(*box);
				write(csd , packet , (strlen(packet)+1)*sizeof(char));	
		}
		else{	
			bzero(box , sizeof(msg_t));
			box->type = MSG_ERROR;
			box->msg = (char*)malloc((strlen("Errore: username non registrato")+1)*sizeof(char));
			strcpy(box->msg,"Errore: username non registrato");
			box->msglen = strlen(box->msg)+1;
			char * logstring = (char*) malloc ( (strlen(box->msg)+strlen("registrazione") + 1) * sizeof(char));
			sprintf(logstring,"\tregistrazione:\t%s",box->msg);
			writeLog( arg->lpath , logstring , 0 );
			box->msglen = strlen(box->msg)+1;
			packet = (char*)malloc((box->msglen+2)*sizeof(char));
			packet = marshall(*box);
			write(csd , packet , (strlen(packet)+1)*sizeof(char));	
		}
		close(csd);
		pthread_mutex_lock(&mux1);
		numThreadAttivi--;	
		pthread_mutex_unlock(&mux1);
		pthread_exit(0);
	break;
	case MSG_REGLOG:
		match = (hdata_t*)malloc(sizeof(hdata_t));
		bzero(match,sizeof(hdata_t));
		char * token;
		token = strtok(box->msg , ":");
		match->uname =(char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy( match->uname, token);
		
		token = strtok(NULL , ":");
		match->fullname =(char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy( match->fullname, token);
		
		token = strtok(NULL , "\0");
		match->email =(char*)malloc((strlen(token)+1)*sizeof(char));
		strcpy( match->email, token);
		
		match->sockid = csd;
		if(CERCAHASH(match->uname,table) != NULL){
			bzero(box , sizeof(msg_t));
			box->msg = (char*)malloc((strlen("Errore: utente gia' registrato")+1)*sizeof(char));
			strcpy(box->msg, "Errore: utente gia' registrato");
			box->type = MSG_ERROR;
			char * logstring = (char*) malloc ( (strlen(box->msg)+strlen("registrazione") + 1) * sizeof(char));
		
			sprintf(logstring,"\tregistrazione:\t%s",box->msg);
			writeLog( arg->lpath , logstring , 0 );
			box->msglen = strlen(box->msg)+1;
			packet = (char*)malloc((box->msglen+2)*sizeof(char));
			packet = marshall(*box);
			write(csd , packet , (strlen(packet)+1)*sizeof(char));
			}
		else{
		
			INSERISCIHASH(match->uname , match , table);
			
			char * logstring = (char*) malloc ( (strlen(match->uname)+strlen("registrazione") + 1) * sizeof(char));
			
			sprintf(logstring,"\tregistrazione:\t%s",box->msg);
			writeLog( arg->lpath, logstring, 0);
		
		
			bzero(box , sizeof(msg_t));
			box->type = MSG_OK;
			packet = (char*)malloc(2*sizeof(char));
			packet = marshall(*box);
			write(csd ,packet ,(strlen(packet)+1)*sizeof(char));
		}
	break;
	case MSG_LIST:
	break;
	default:
		perror("Type non riconosciuto.\n");
	break;	
	}
	
	pthread_exit(0);
}

void * thread_dispatcher(void * targ) {

	pthread_exit(0);
}

int main(int argc,char** argv) {

pid_t pid = fork();
if ( pid == 0 ) {
	chat_server(argc,argv);
} else if ( pid < 0 ) {
	perror("ERRORE: fork fallita.\n");
	exit(-1);
}

exit(0);
}
