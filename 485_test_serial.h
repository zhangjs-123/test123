#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>
#include <linux/serial.h>
#include <sys/ioctl.h>

#ifndef __485TESTSERIAL_H__
#define __485TESTSERIAL_H__

#ifdef __cplusplus 
extern "C" {
#endif
int writeSerial(int fd, const void *buf, size_t count, int timeout);
int readSerial(int fd, void *buf, size_t count, int timeout);
int set_baud(int fd, int baud);
void set_baud_helper(int baud, struct termios *oldtio);
int UART_Send(int fd, char *send_buf,int data_len);
int UART_Recv(int fd, char *rcv_buf,int data_len);
int UART_Init(int fd, int speed,int flow_ctrlint ,int databits,int stopbits,char parity);
int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
void UART_Close(int fd);
int UART_Open(int fd,char* port);
#ifdef __cplusplus 
} 
#endif 

#endif