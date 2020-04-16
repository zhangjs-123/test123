#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef __INTERTEST_H__
#define __INTERTEST_H__

#ifdef __cplusplus 
extern "C" {
#endif

int connect_check(char *eth);

#ifdef __cplusplus 
} 
#endif 

#endif

