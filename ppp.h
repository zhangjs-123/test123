#include <iostream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h> 
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <linux/if.h>
#include <string.h>
using std::string;

#ifndef __PPP_H__
#define __PPP_H__

#ifdef __cplusplus 
extern "C" {
#endif

void setGpioValue(int n,int val);
void initGpio(int n);
void setGpioDirection(int n,char *direction);
int getGpioValue(int n);
int set_opt(int fd, int baud,
            unsigned char nBits, char nParity,
            unsigned char nStop);
int readSerial(int fd, void *buf, size_t count, int timeout);
int writeSerial(int fd, const void *buf, size_t count, int timeout);
void powerGPRS();
int createPPPSocket();
string GetSelfPath();
string GetSimInfoByDb();
int NetStatus(const char *if_name,string& msg );
int PppAtCheck(int fd, const char *cmd,  char *rdBuf,int len, int wait);
int CheckGprsByAt();
int pppp_test();

#ifdef __cplusplus 
} 
#endif


#endif 