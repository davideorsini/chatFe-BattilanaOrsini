#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

void timestamp(char* ts){
	time_t t;
	t = time(NULL);
	ctime_r(&t,ts);
	ts[strlen(ts)-1] = '\0';		
}
