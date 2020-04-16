#include "485_test_serial.h"
#include "485_test.h"

#define  BUF_SIZE    10240
static char  DEVICE1[] = "/dev/ttymxc1";
static char  DEVICE2[] = "/dev/ttymxc2";
//485通信的就是ttymxc1，ttymxc2两个文件
const int TIME_OUT=16;

static int fd1 = 0; 
static int fd2 = 0;  
static  time_t tStart=0;
void* receve_usrt1 (void*);
void* receve_usrt2 (void*);
 char rcv_buf2[512];
  int len;


int test_485()
{            
    int ret1,ret2;                          
    char write_math[30];
    pid_t p1 = fork();
    int i=1;
    char tmp[1024];
    char rcv_buf1[512];
    
    int rcv;
    int j=0;
    int len;
    tStart = time(NULL);
    if(p1 > 0)
    {   
        printf("receve_usrt1.....\n");
        fd1 = UART_Open(fd1, DEVICE1);
        if(fd1<0){    
        printf("open error\n");    
        exit(1);
        }

        ret1 = UART_Init(fd1,9600,0,8,1,'N');
        if (fd1<0){    
        printf("Set Port Error\n");    
        exit(1);
        }
         
        for(j=0;j<30;j++) {
		int len = read(fd1, rcv_buf1, 1);
        if(len<0)
        {
            break;
        }

         if(len>0)
        {
		 for( int i = 0; i < len; i++)
         {
			printf("%x", rcv_buf1[i]);
            rcv_buf1[i]++;
         }
        }
    	sleep(1);
         int wri=write(fd2, rcv_buf1, len);
         if(wri<0)
        {
            break;
        }
        printf("\n");
        time_t tNow = time(NULL);
         if(abs(tNow - tStart) > TIME_OUT)
        {
            break;
        } 	
      }
       printf("456\n");
    }
    else
    {
      fd2 = UART_Open(fd2, DEVICE2);
      if(fd2<0){    
      printf("open error\n");    
      exit(1);     
     }

     ret2 = UART_Init(fd2,9600,0,8,1,'N');
     if (fd2<0){    
     printf("Set Port Error\n");    
     exit(1);
     }

     printf("receve_usrt2.....\n");
     
      tmp[0] = i%0xFF;
         printf("0x%x\n", tmp[0]);
       write(fd2, tmp, 1);

      for(j=0;j<30;j++) {
		int len = read(fd2, rcv_buf2, 0x01);
        if(len<0)
        {
            break;
        }
        if(len>0)
        {	
		for( int i = 0; i < len; i++)
         {
			printf("%x", rcv_buf2[i]);
            rcv_buf2[i]++;
            rcv_buf2[i]++;
         }
        }
        sleep(1);   
        int wri=write(fd1, rcv_buf2, len);
        if(wri<0)
        {
            break;
        }   
		printf("\n");
        time_t tNow = time(NULL);
        if(abs(tNow - tStart) > TIME_OUT)
        {
            break;
        }
       }
       printf("123\n");
   }   
    return 0;     
}
