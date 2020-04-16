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
#include <bits/types.h>

#ifndef __485TEST_H__
#define __485TEST_H__

#ifdef __cplusplus 
extern "C" {
#endif

int test_485();
void receve_usrt1 (void);
void receve_usrt2 (void);

#ifdef __cplusplus 
} 
#endif 

#endif