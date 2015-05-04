#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

int main () {
  
  hash_t H;
  
  hdata_t * as, * bs;
  
  char * key;
      
  H = CREAHASH();
 
  //////////////////////////////////////////////////////////////
  as = (hdata_t *) malloc(sizeof(hdata_t));
  
  as->uname    = "fabio";
  as->fullname = "fabio schifano";
  as->email    = "schifano@fe.infn.it";
  as->sockid   = -1;
  
  INSERISCIHASH(as->uname, (void*) as, H);
  
  //////////////////////////////////////////////////////////////
  // inserzione di un nuovo elemento
  
  // questa malloc e' necessaria per NON sovrascrivere i dati precedenti
  as = (hdata_t *) malloc(sizeof(hdata_t));
  
  as->uname    = "paperino";
  as->fullname = "paperino disney";
  as->email    = "paperino@fe.infn.it";
  as->sockid   = -1;
  
  // inserisce un elemento hdata_t * nella tabella hash H
  INSERISCIHASH(as->uname, (void*) as, H);
  
  //////////////////////////////////////////////////////////////
  
  key = "fabio";
  
  // restituisce se esiste un puntatore ad un elemento hdata_t
  // con chiave key; NULL altrimenti  
  bs = CERCAHASH(key, H);
  
  if ( bs != NULL ) {
    printf("uname: %s, full name: %s, email: %s  sockid: %d\n", 
      bs->uname, bs->fullname, bs->email, bs->sockid );
  } else {
    fprintf(stderr, "ERROR: key: %s not found\n", key);
  }
  
  //////////////////////////////////////////////////////////////
  
  key = "pluto";
  
  bs = CERCAHASH(key, H);
  
  if ( bs != NULL ) {
    printf("uname: %s, full name: %s, email: %s  sockid: %d\n", 
      bs->uname, bs->fullname, bs->email, bs->sockid );
  } else {
    fprintf(stderr, "ERROR: key: %s not found\n", key);
  }
  
  //////////////////////////////////////////////////////////////
  
  key = "paperino";
  
  bs = CERCAHASH(key, H);
  
  if ( bs != NULL ) {
    printf("uname: %s, full name: %s, email: %s  sockid: %d\n", 
      bs->uname, bs->fullname, bs->email, bs->sockid );
  } else {
    fprintf(stderr, "ERROR: key: %s not found\n", key);
  }
  
  exit(0);
  
}
