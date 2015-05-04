/*************************************/
/* Library hash: schifano fabio 2013 */
/*************************************/

#ifndef HASH_H
#define HASH_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "lista.h"


/* numero primo */
#define HL 10 

typedef lista * hash_t;

//////////////////////////////////////////////////////////////////////

int hashfunc(char * k) {
  int i = 0;
  int hashval = 0;
  for ( i=0; i < strlen(k); i++ ) {
    hashval = ((hashval*SL) + k[i]) % HL;
  }
  return hashval;
}

//////////////////////////////////////////////////////////////////////
// restituisce la posizione della cella che contiene la chiave key == uname

hdata_t * CERCALISTA ( char * key, lista L ) {
  int found;
  posizione p;
  hdata_t * r;
  char * k;
  r = NULL;
  p = PRIMOLISTA(L);
  found = 0;
  //fprintf(stderr, "looking for key: %s\n", key);
  while ( (!found) && (!FINELISTA(p,L)) ) {
    k = ((hdata_t *)(p->elemento))->uname;
    if ( strcmp (k, key ) == 0 ) {
      r = p->elemento; found = 1;
    }
    p = SUCCLISTA(p);
  }
  return r;
}

//////////////////////////////////////////////////////////////////////

hash_t CREAHASH () {
  hash_t H;
  int i;
  H = (hash_t) malloc(HL*sizeof(lista));
  for ( i=0; i < HL; i++ ) {
    H[i] = CREALISTA(); 
  }
  return H;
}

//////////////////////////////////////////////////////////////////////
// cerca un elemento nella tabella Hash
// restituisce un "hdata_t *" oppure NULL se non esiste
 
void * CERCAHASH(char * key, hash_t H) {
  int i;
  i = hashfunc(key);
  return CERCALISTA(key, H[i]);
}

//////////////////////////////////////////////////////////////////////
// inserisce l'elemento hdata_t * nella tabella hash H

void INSERISCIHASH (char * key, hdata_t * elem, hash_t H) {
  int i;
  posizione p;
  i = hashfunc(key);
  if (CERCAHASH(key, H) == NULL) {
    p = PRIMOLISTA(H[i]);
    INSLISTA((void *)elem, &p); 
  }
}

//////////////////////////////////////////////////////////////////////

void PRINTTABLE(hash_t table){
	int i;
	posizione p;
	printf("\n");
	for(i=0;i<HL;i++){
		p = PRIMOLISTA(table[i]);
		while(p != table[i]){
			printf("**%s**%s**%s**%d\n",((hdata_t*)(p->elemento))->uname,((hdata_t*)(p->elemento))->fullname,((hdata_t*)(p->elemento))->email,((hdata_t*)(p->elemento))->sockid);
			p = p->successivo;
		}
		printf("*********\n");
	}
}

void PRINTROW(hash_t table, char* key){
	int i = hashfunc(key);
	posizione p;
	p = table[i];
	p = p->successivo;
	while(p ){
		printf("**%s**%s**%s**%d\n",((hdata_t*)(p->elemento))->uname,((hdata_t*)(p->elemento))->fullname,((hdata_t*)(p->elemento))->email,((hdata_t*)(p->elemento))->sockid);
		p = p->successivo;
	}
}


#endif

