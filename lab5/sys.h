#ifndef H
#define H

#include <stdio.h>      // Standard I/O functions 
#include <stdlib.h>     // Standard library 
#include <string.h>     // String handling functions 
#include <unistd.h>     // Unix system calls 
#include <errno.h>      // Error difinations 
#include <fcntl.h>      // Micros for I/O 
#include <sys/types.h>  // System data type 
#include <sys/stat.h>   // File system status 

#include "err.h" // Error handling functions 

// true & false
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef TRUE
  #define TRUE 1
#endif

// max & min
#ifndef min
  #define min(m, n) ((m) < (n) ? (m) : (n))
#endif
#ifndef max
  #define max(m, n) ((m) > (n) ? (m) : (n))
#endif 

#endif