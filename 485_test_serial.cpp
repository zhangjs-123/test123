#include "485_test_serial.h"
#include "485_test.h"

#define FALSE -1
#define TRUE 0
using namespace std;

void UART_Close(int fd)
{
    close(fd);
}

int UART_Open(int fd,char* port)
{
    
      fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY|O_NONBLOCK);
      if (FALSE == fd){
        perror("Can't Open Serial Port");
          return(FALSE);
      }
    int portfd;
    #if (__GNUC__ == 4 && __GNUC_MINOR__ == 3)
	struct my_serial_rs485 rs485conf;
	struct my_serial_rs485 rs485conf_bak;
    #else
    	struct serial_rs485 rs485conf;
    	struct serial_rs485 rs485conf_bak;
    #endif
        portfd=fd;
    /* Driver-specific ioctls: ...\linux-3.10.x\include\uapi\asm-generic\ioctls.h */
    #define TIOCGRS485      0x542E
    #define TIOCSRS485      0x542F
    if (ioctl (portfd, TIOCGRS485, &rs485conf) < 0) 
	{
		/* Error handling.*/ 
		// Log::Inf("ioctl TIOCGRS485 error.\n");
        printf( "[ERR] ioctl TIOCGRS485 error.\n");
	}
	/* Enable RS485 mode: */
	rs485conf.flags |= SER_RS485_ENABLED;

	/* Set logical level for RTS pin equal to 1 when sending: */
	rs485conf.flags |= SER_RS485_RTS_ON_SEND;
	//rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;

	/* set logical level for RTS pin equal to 0 after sending: */ 
	rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);
	//rs485conf.flags &= ~(SER_RS485_RTS_ON_SEND);



	/* Set rts delay after send, if needed: */
	rs485conf.delay_rts_after_send = 0x80;

	if (ioctl (portfd, TIOCSRS485, &rs485conf) < 0)
	{
		/* Error handling.*/ 
		printf("ioctl TIOCSRS485 error.\n");
        
	}

	if (ioctl (portfd, TIOCGRS485, &rs485conf_bak) < 0)
	{
		/* Error handling.*/ 
		printf("ioctl TIOCGRS485 error.\n");
    }
    else
    {
        printf("Config Rs485 Ok.\n");
    }

    //判断串口的状态是否为阻塞状态
      if(fcntl(fd, F_SETFL, 0) < 0){
        printf("fcntl failed!\n");
            return(FALSE);
      } else {
           //    printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
      }
    
     //测试是否为终端设备
      if(0 == isatty(STDIN_FILENO)){
          printf("standard input is not a terminal device\n");
            return(FALSE);
      }
    printf("%s aleady open\n",port);
      return fd;
}



/*******************************************************************
* 名称： UART0_Set
* 功能： 设置串口数据位，停止位和效验位
* 入口参数： fd 串口文件描述符
* speed 串口速度
* flow_ctrl 数据流控制
* databits 数据位 取值为 7 或者8
* stopbits 停止位 取值为 1 或者2
* parity 效验类型 取值为N,E,O,,S
*出口参数： 正确返回为1，错误返回为0
*******************************************************************/
int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{
    
        int i;
  //    int status;
      int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B38400, B19200, B9600, B4800, B2400, B1200, B300
             };
        int name_arr[] = {
             38400, 19200, 9600, 4800, 2400, 1200, 300, 38400,
                   19200, 9600, 4800, 2400, 1200, 300
             };
    struct termios options;

   /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数,还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
    */
    if(tcgetattr( fd,&options) != 0){
     perror("SetupSerial 1");
     return(FALSE);
        }
        
        //设置串口输入波特率和输出波特率
    for(i= 0;i < sizeof(speed_arr) / sizeof(int);i++) {     
        if (speed == name_arr[i]) {
                  cfsetispeed(&options, speed_arr[i]);
                  cfsetospeed(&options, speed_arr[i]);
        }
        }    
//修改控制模式，保证程序不会占用串口        
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;
    //设置数据流控制
    switch(flow_ctrl){
        case 0 : //不使用流控制
            options.c_cflag &= ~CRTSCTS;
            break;    
            case 1 : //使用硬件流控制
                options.c_cflag |= CRTSCTS;
                break;
            case 2 : //使用软件流控制
                options.c_cflag |= IXON | IXOFF | IXANY;
                break;
    }
     //设置数据位
    options.c_cflag &= ~CSIZE; //屏蔽其他标志位
    switch (databits){
        case 5 :
                options.c_cflag |= CS5;
                break;
            case 6    :
                options.c_cflag |= CS6;
                break;
            case 7    :
                options.c_cflag |= CS7;
                break;
            case 8:
                options.c_cflag |= CS8;
                break;
               default:
                fprintf(stderr,"Unsupported data size\n");
                return (FALSE);
    }
     //设置校验位
    switch (parity) {
        case 'n':
            case 'N': //无奇偶校验位。
                options.c_cflag &= ~PARENB;
                options.c_iflag &= ~INPCK;
                break;
            case 'o':
            case 'O': //设置为奇校验
                options.c_cflag |= (PARODD | PARENB);
                options.c_iflag |= INPCK;
                break;
            case 'e':
            case 'E': //设置为偶校验
                options.c_cflag |= PARENB;
                options.c_cflag &= ~PARODD;
                options.c_iflag |= INPCK;
                break;
            case 's':
            case 'S': //设置为空格
                options.c_cflag &= ~PARENB;
                options.c_cflag &= ~CSTOPB;
                break;
            default:
                fprintf(stderr,"Unsupported parity\n");
                return (FALSE);
    }
     // 设置停止位
    switch (stopbits){
        case 1:
            options.c_cflag &= ~CSTOPB;
                break;
            case 2:
                options.c_cflag |= CSTOPB;
                   break;
            default:
                 fprintf(stderr,"Unsupported stop bits\n");
                 return (FALSE);
    }
    //修改输出模式，原始数据输出
        options.c_oflag &= ~OPOST;
    //设置等待时间和最小接收字符
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */
   
    //如果发生数据溢出，接收数据，但是不再读取
    tcflush(fd,TCIFLUSH);
   
    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
               perror("com set error!/n");
       return (FALSE);
    }
    return (TRUE);
}


int UART_Init(int fd, int speed,int flow_ctrlint ,int databits,int stopbits,char parity)
{
     //设置串口数据帧格式
    if (FALSE == UART_Set(fd,speed,flow_ctrlint,databits,stopbits,parity)) {         
        return FALSE;
        } else {
           return TRUE;
       }
}



/*******************************************************************
* 名称： UART0_Recv
* 功能： 接收串口数据
* 入口参数： fd :文件描述符
* rcv_buf :接收串口中数据存入rcv_buf缓冲区中
* data_len :一帧数据的长度
* 出口参数： 正确返回为1，错误返回为0
*******************************************************************/
int UART_Recv(int fd, char *rcv_buf,int data_len)
{
    int len,fs_sel;
    fd_set fs_read;
    
    struct timeval time;
    
    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);
    
    time.tv_sec = 10;
    time.tv_usec = 0;
    
    //使用select实现串口的多路通信
   // fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
   // if(fs_sel){
       printf("1\n");
     len = read(fd,rcv_buf,data_len);
     printf("len=%d\n",len);
     printf("rcv_buf=%s\n",rcv_buf);
     return len;

    //    } else {
     //   return FALSE;
    //}    
}

/*******************************************************************
* 名称： UART0_Send
* 功能： 发送数据
* 入口参数： fd :文件描述符
* send_buf :存放串口发送数据
* data_len :一帧数据的个数
* 出口参数： 正确返回为1，错误返回为0
*******************************************************************/
int UART_Send(int fd, char *send_buf,int data_len)
{
    int ret;
    
    ret = write(fd,send_buf,data_len);
   if(ret<0)
   {
       perror("send fail");
   }

   printf("writeret=%d\n",ret);
    
}


/*****************************************************************************
 * 
 * 串口函数提取
 */

void set_baud_helper(int baud, struct termios *oldtio)
{
    switch(baud)
    {
    case 300:
        cfsetispeed(oldtio, B300);
        cfsetospeed(oldtio, B300);
        break;	  
    case 600:
        cfsetispeed(oldtio, B600);
        cfsetospeed(oldtio, B600);
        break;	
    case 1200:
        cfsetispeed(oldtio, B1200);
        cfsetospeed(oldtio, B1200);
        break;
    case 2400:
        cfsetispeed(oldtio, B2400);
        cfsetospeed(oldtio, B2400);
        break;
    case 4800:
        cfsetispeed(oldtio, B4800);
        cfsetospeed(oldtio, B4800);
        break;
    case 19200:
        cfsetispeed(oldtio, B19200);
        cfsetospeed(oldtio, B19200);
        break;
    case 38400:
        cfsetispeed(oldtio, B38400);
        cfsetospeed(oldtio, B38400);
        break;
    case 57600:
        cfsetispeed(oldtio, B57600);
        cfsetospeed(oldtio, B57600);
        break;
    case 115200:
        cfsetispeed(oldtio, B115200);
        cfsetospeed(oldtio, B115200);
        break;
    default:
        cfsetispeed(oldtio, B9600);
        cfsetospeed(oldtio, B9600);
        break;
    }
}

// 只改变波特率
int set_baud(int fd, int baud)
{
    struct termios oldtio;
    if  ( tcgetattr(fd, &oldtio)  !=  0) 
    { 
        printf("Serial,get serial attr: %s\n", strerror(errno));
        return -1;
    }
    set_baud_helper(baud, &oldtio);

    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&oldtio))!=0)
    {
        printf("set serial: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int readSerial(int fd, void *buf, size_t count, int timeout)
{
    fd_set rdSet;
    struct timeval tv;

    FD_ZERO(&rdSet);
    FD_SET(fd, &rdSet);

    memset(&tv, 0, sizeof(tv));
    tv.tv_usec = timeout * 50000;

    // 已经读到的字节数
    int hasRead = 0;
    int ret = 0;

    int iCounter = 0;
    //printf("----------- in readSerial ---------------\n");
    do
    {
        ret = select(fd + 1, &rdSet, NULL, NULL, &tv);

        if (ret < 0)
        {
            printf("Serial,read select ret: %d, %s\n", ret, strerror(errno));
        }
        else if (FD_ISSET(fd, &rdSet))
        {
            ret = read(fd, (char*)buf + hasRead, count - hasRead);
            if (ret < 0)
            {
                if ((errno == EAGAIN) ||
                    (errno == EWOULDBLOCK) ||
                    (errno == EINTR))
                {
                    //printf("Serial,read timeout error:%s. ignore ti!!!", strerror(errno));
                    ret = 0;
                }
            }
            else
            {
                hasRead += ret;
            }
        }
        //printf("[%d]-------- ret: %d, hasRead: %d\n", iCounter++, ret, hasRead);
    } while ((ret > 0) && (hasRead < count));


    return hasRead;
}

int writeSerial(int fd, const void *buf, size_t count, int timeout)
{
    if (count <= 0)
    {
        return 0;
    }

    fd_set	wtSet;
    struct timeval	tv;

    FD_ZERO(&wtSet);
    FD_SET(fd, &wtSet);

    memset(&tv, 0, sizeof(tv));
    tv.tv_usec = timeout * 1000;

    int ret = select(fd + 1, NULL, &wtSet, NULL, &tv );

    if (ret < 0)
    {
        printf("Serial,write select ret: %d, %s\n", ret, strerror(errno));
    }
    else if (FD_ISSET(fd, &wtSet))
    {
        ret = write(fd, buf, count);
        if (ret < 0)
        {
            if ((errno == EAGAIN) ||
                (errno == EWOULDBLOCK) ||
                (errno == EINTR))
            {
                //printf("Serial,write timeout error:%s. ignore ti!!!", strerror(errno));
                ret = 0;// no data available
            }
        }
        else if (ret > 0)
        {
            fsync(fd);
        }
        
    }

    return ret;
}