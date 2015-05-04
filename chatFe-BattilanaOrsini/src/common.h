#ifndef COMMON_H
#define COMMON_H

#define SL 256  // string length

struct struct_hdata_t {
  char * uname;     // username
  char * fullname;  // full name
  char * email;     // email address
  int  sockid;      // socket identifier; -1 = not connected 
};

typedef struct struct_hdata_t hdata_t;

#endif


