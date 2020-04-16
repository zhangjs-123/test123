#include "485_test_serial.h"
#include "infrared.h"
#define  BUF_SIZE    10240
static char  DEVICE[] = "/dev/ttymxc7";
//485通信的就是ttymxc1，ttymxc2两个文件

static int fd1= 0;   
const int TIME_OUT = 3;
 
int test_infrared()
{            
    int ret1; 
    int len;                         
    char write_math[30];
    char rcv_buf2[512];
    int i=1;
    int fd;
    char tmp[1024];

    fd = UART_Open(fd1, DEVICE);
    if(fd1<0){    
     printf("open error\n");    
     exit(1);     
    }

     ret1 = UART_Init(fd1,9600,0,8,1,'N');
    if (fd1<0){    
     printf("Set Port Error\n");    
     exit(1);
    }
    
    tmp[0] = i%0xFF;
    printf("0x%x\n", tmp[0]);
    write(fd1, tmp, 1);
    
    while (1) {
		len = read(fd, tmp, 0x01);	
		for(i = 0; i < len; i++)
			printf("%x", tmp[i]++);
         write(fd, tmp, len);
		printf("\n");
	}
    UART_Close(fd1);    
    return 0;     
}


