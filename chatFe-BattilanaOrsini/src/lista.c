#include <stdlib.h>

#include "lista.h"
//#include "lista-aux.h"

/********************************************/
    
int main () {
  lista L, P, D;
  int i, a;
  posizione pL, pD, pP;
  
  /* inizializza il generatore di numeri random */
  srand(71);
  
  L = CREALISTA();  
  P = CREALISTA();
  D = CREALISTA();
  
  pL = PRIMOLISTA(L);  
  
  /* init lista L */
  for (i = 0; i <= 10; i++) {    
    /* in questo modo i numeri random sono piu' belli */
    a = 1+(int) (100.0*rand()/(RAND_MAX+1.0));     
    INSLISTA(a, &pL);
  }

  printf("L:\n");
  PRINTLISTA(L);
  printf("\n");
  
  REVERSELISTA(L);
  printf("L-reversed: \n");
  PRINTLISTA(L); 
  printf("\n");
  
  APPARTIENELISTA(97, L);
  
  pL = PRIMOLISTA(L);  
  pD = PRIMOLISTA(D);  
  pP = PRIMOLISTA(P);  
  
  /* scandisce lista L e copia i valori in lista P e D */
  while ( ! FINELISTA(pL, L) ) { 
    
    a = LEGGILISTA(pL);
    
    if ( a % 2 ) {
      INSLISTA(a, &pD);
    } else {
      INSLISTA(a, &pP);
    }
             
    CANCLISTA(&pL);
  }
  
  printf("L:\n");
  PRINTLISTA(L);
  printf("\n");
  
  printf("P:\n");
  PRINTLISTA(P);
  printf("\n");
  
  printf("D:\n");
  PRINTLISTA(D);  
  printf("\n");
  
  printf("Len of P is: %d\n", LENLISTA(P));
  printf("Len of D is: %d\n", LENLISTA(D));
  
  return 0;
}
  
