#include "485_test_serial.h"
#include "485_test.h"
// test new commit
#define  BUF_SIZE    10240
static char  DEVICE1[] = "/dev/ttymxc1";
static char  DEVICE2[] = "/dev/ttymxc2";
//485通信的就是ttymxc1，ttymxc2两个文件
const int TIME_OUT=2;

static int fd1 = 0; 
static int fd2 = 0;  
static  time_t tStart=0;
void* receve_usrt1 (void*);
void* receve_usrt2 (void*);



int test_485()
{            
    int ret1,ret2;                          
    char write_math[30];
    char rcv_buf2[512];
    pthread_t sockread1,sockread2;
    int i=1;
    char tmp[1024];

    fd1 = UART_Open(fd1, DEVICE1);
    if(fd1<0){    
     printf("open error\n");    
     exit(1);     
    }

     fd2 = UART_Open(fd2, DEVICE2);
    if(fd2<0){    
     printf("open error\n");    
     exit(1);     
    }

     ret1 = UART_Init(fd1,9600,0,8,1,'N');
    if (fd1<0){    
     printf("Set Port Error\n");    
     exit(1);
    }

     ret2 = UART_Init(fd2,9600,0,8,1,'N');
    if (fd2<0){    
     printf("Set Port Error\n");    
     exit(1);
    }
    tStart = time(NULL);
   

    ret2 = pthread_create(&sockread2,NULL,receve_usrt2,NULL); // 创建接收2的线程
    if(ret2 < 0)
    {
        perror("phread_create_read2");
         return 0;
    }


     ret1 = pthread_create(&sockread1,NULL,receve_usrt1,NULL); // 创建接收1的线程
    if(ret1 < 0)
    {
        perror("phread_create_read1");
        return 0;
    }
    tmp[0] = i%0xFF;
    printf("0x%x\n", tmp[0]);
    write(fd1, tmp, 1);
    pthread_join(sockread1,NULL); // 等待线程退出
    pthread_join(sockread2,NULL); // 等待线程退出 
    UART_Close(fd1);    
    UART_Close(fd2);
    return 0;     
}

void* receve_usrt1 (void *c)
{  
    printf("receve_usrt1.....\n");
    char rcv_buf1[512];
    int ret1;
    int rcv;
    int i;
    int j=0;
    	for(j=0;j<30;j++) {
		int len = read(fd1, rcv_buf1, 1);
        if(len<0)
        {
            break;
        }	
        if(len>0)
        {
		for( i = 0; i < len; i++)
        {
			printf("%x", rcv_buf1[i]);
            rcv_buf1[i]++;
        }
        sleep(1);
         int wri=write(fd2, rcv_buf1, len);
         if(wri<0)
        {
            break;
        }
		printf("\n");
        }
        time_t tNow = time(NULL);
        if(abs(tNow - tStart) > TIME_OUT)
        {
            break;
        } 
   }
    printf("456\n");
   pthread_exit(NULL);
}

void * receve_usrt2 (void * c)
{
    printf("receve_usrt2.....\n");
    char rcv_buf2[512];
    int ret2;
    int rcv;
    int i;
    int j=0;

for(j=0;j<30;j++) {
		int len = read(fd2, rcv_buf2, 0x01);
        if(len<0)
        {
            break;
        }
        if(len>0)
        {	
		for( i = 0; i < len; i++)
        {
			printf("%x", rcv_buf2[i]);
            rcv_buf2[i]++;
            rcv_buf2[i]++;
        }
        sleep(1);   
         int wri=write(fd1, rcv_buf2, len);
         if(wri<0)
        {
            break;
        }   
		printf("\n");
        }
        time_t tNow = time(NULL);
        if(abs(tNow - tStart) > TIME_OUT)
        {
            break;
        }
       }
     printf("123\n");
    pthread_exit(NULL);
}
