#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include <errno.h> 
#include <unistd.h>  
 
#include <sys/types.h>  
#include <sys/stat.h>  
 
#include <fcntl.h>   
#include <termios.h> 
#include <pthread.h> 

#ifndef __INFRARED_H__
#define __INFRARED_H__

#ifdef __cplusplus 
extern "C" {
#endif
int test_infrared();
#ifdef __cplusplus 
} 
#endif 

#endif