#include "ppp.h"
#include "485_test_serial.h"

using namespace std;
static string tty_dev;
static string isp_name;
static string ppp_start_path;
static string sim_apn;
static string sim_telephone;
static string sim_user;
static string sim_password;
static string sim_rootdirectory;

time_t g_lastRecv = 0;  //上行上次通信的时间

#define CMD_SIM_EXIST       "AT+CPIN?\r\n"        // 检测SIM卡是否存在
#define CMD_SIM_SIG_STREN   "AT+CSQ\r\n"          //检测信号强度
#define CMD_NET_STAT        "AT+CREG?\r\n"        // 查询注册到网络
#define CMD_SIM_TYPE        "AT+COPS?\r\n"        //查询运营商
#define IO_GPRS_RST     92
#define IO_GPRS_PWR     87
#define IO_GPRS_SWICTH  83

static int g_pppSocket=-1;
static const char* pppd_shell="ppp/ppp-on ";
static const char* pppd_process="pppd";
static const char* close_pppd_process="killall pppd 2>/dev/null ";
static const char* ifconfig_pppd="ppp0";

int getGpioValue(int n)
{
    char path[64];
    char value_str[3];
    int fd;
 
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", n);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open gpio value for reading!");
        return -1;
    }
 
    if (read(fd, value_str, 3) < 0) {
        perror("Failed to read value!");
        return -1;
    }
 
    close(fd);
    return (atoi(value_str));
}   

void setGpioValue(int n,int val)
{
    char path[100] = {0};
    sprintf(path,"/sys/class/gpio/gpio%d/value",n);
    FILE * fp =fopen(path,"w");
    if (fp == NULL)
        perror("direction open filed");
    else
        fprintf(fp,"%d",val);
    fclose(fp);
}   

int set_opt(int fd, int baud,
            unsigned char nBits, char nParity,
            unsigned char nStop)
{
    struct termios newtio;
    memset(&newtio,0, sizeof( newtio ));


    newtio.c_cflag=0;
    newtio.c_iflag=0;

    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nParity )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E': 
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':  

        newtio.c_cflag &= ~PARENB;
        break;
    case 'S':
        //newtio.c_cflag |= PARENB | CS8 | CMSPAR; // new concentrator has no CMSPAR
        newtio.c_cflag |= PARENB | CS8;
        break;
    }

    set_baud_helper(baud, &newtio);

    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        printf("set serial: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int createPPPSocket()
{
    if ((g_pppSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("ppp, create ppp socket ERROR: %s", strerror(errno));
    }
    return g_pppSocket;
}

string GetSimInfoByDb()//?
{  
  tty_dev="/dev/ttymxc5";
  sim_apn=" internet ";
  sim_telephone=" *99***1# ";
  sim_user="\" \"";
  sim_password="\" \"";
  sim_rootdirectory=GetSelfPath();
  ppp_start_path.clear();
  ppp_start_path=ppp_start_path+sim_rootdirectory+pppd_shell+" "+sim_apn+" "+sim_telephone+" & ";
  printf("=========%s",ppp_start_path.c_str());
  return ppp_start_path;
}

void powerGPRS()
{
    setGpioValue(IO_GPRS_RST,0);
    setGpioValue(IO_GPRS_PWR,1);
    setGpioValue(IO_GPRS_SWICTH,0);
    sleep(1);
    setGpioValue(IO_GPRS_RST,0);
    setGpioValue(IO_GPRS_PWR,1);
    setGpioValue(IO_GPRS_SWICTH,1);
}

int NetStatus(const char *if_name,string& msg )
{
	int fd = -1; 
	struct ifreq ifr; 
	struct ifconf ifc;  
	struct ifreq ifrs_buf[100]; 
	int if_number =0;
	int i;
 
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		//fprintf(stderr, "%s: socket error [%d] %s\r\n",if_name, errno, strerror(errno));
        msg="socket error";
		close(fd);
		return -1; 
	}
 
	ifc.ifc_len = sizeof(ifrs_buf);  
	ifc.ifc_buf = (caddr_t)ifrs_buf;  
	if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) <0)  
	{
		//fprintf(stderr, "%s: ioctl SIOCGIFCONF error [%d] %s\r\n",if_name, errno, strerror(errno));
        msg="ioctl SIOCGIFCONF error";
		close(fd);
		return -1; 
	}
 
	if_number = ifc.ifc_len / sizeof(struct ifreq);
	//printf("aaaa====%d",if_number);
	for(i=0; i< if_number; i++)
	{
		if(strcmp(if_name,ifrs_buf[i].ifr_name ) == 0)
		{
			break;
		}
	}
	if(i >= if_number)
	{
		close(fd);
        msg="DEVICE_NONE";
		return 0;
	}
	
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1); 
	ifr.ifr_name[IFNAMSIZ-1] = 0; 
	if (ioctl(fd, SIOCGIFFLAGS, (char *)&ifr) <0)  
	{
		//fprintf(stderr, "%s: ioctl SIOCGIFFLAGS error [%d] %s\r\n",if_name, errno, strerror(errno));
        msg="ioctl SIOCGIFFLAGS error";
		close(fd);
		return -1; 
	}	
	if(!(ifr.ifr_flags & IFF_UP))
	{
		close(fd);
		//fprintf(stderr, "DEVICE_DOWN\r\n");
        msg="DEVICE_DOWN";
		return 1;
	}
	if(!(ifr.ifr_flags & IFF_RUNNING))
	{
		close(fd);
		//fprintf(stderr, "DEVICE_UNPLUGGED\r\n");
        msg="DEVICE_UNPLUGGED";
		return 2 ;
	}
	//printf("%s Linked\r\n",if_name);
    msg="Linked";
	return 3;
}

int PppAtCheck(int fd, const char *cmd,  char *rdBuf,int len, int wait=100)
{
    memset(rdBuf, 0, len);
    int n = writeSerial(fd, cmd, strlen(cmd), wait);
    n = readSerial(fd, rdBuf, len, wait);
    if (n > 0)
    {
        rdBuf[len-1] = 0;
        //printf("%s\n",rdBuf);
        return n;
    }
    return 0;
}

int CheckGprsByAt()    //检查
{
    int fd = open(tty_dev.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);
    int baud    = 115200;
    set_opt(fd, baud, 8, 'N', 1);

    if(fd<=0)
        return -1;

	char ackbuf[128]={0};
	const char *delim = "\r\n";
	char *saveptr = NULL;
	char *str = NULL;
	int csq = -1;
	
	int faild_try=60;
	while(faild_try--)
	{
		printf("Check 2G/3G/4G moulde\n");
		isp_name.clear();
		// 检测SIM卡是否存在
		if(PppAtCheck(fd, CMD_SIM_EXIST, ackbuf, sizeof(ackbuf)))
		{
			if (NULL != strstr(ackbuf, "READY"))
			{
				printf("SIM Card Ok\n");
                sleep(1);
				// 检测信号强度
				if(PppAtCheck(fd, CMD_SIM_SIG_STREN, ackbuf, sizeof(ackbuf)))
				{
					char csqbuf[32];
					str = strtok_r(ackbuf, delim, &saveptr);
					while(str)
					{
						if (strstr(str, "CSQ:"))
						{
							memset(csqbuf, 0, sizeof(csqbuf));
							strncpy(csqbuf, &str[6], 2);
							csq = atoi(csqbuf);
							break;
						}
						str = strtok_r(NULL, delim, &saveptr);
					}
					if(csq>=10 && csq<99)
					{
						printf("SIG=%2d OK\n",csq);
						// 查询注册到网络
						if(PppAtCheck(fd, CMD_NET_STAT, ackbuf, sizeof(ackbuf)))
						{
							int  creg = 0;
							str = strtok_r(ackbuf, delim, &saveptr);
							while(str)
							{
								if (strstr(str, "CREG:"))
								{
									char cregbuf[16];
									memset(cregbuf, 0, sizeof(cregbuf));
									strncpy(cregbuf, &str[9], 2);
									creg = atoi(cregbuf);
									break;
								}

								str = strtok_r(NULL, delim, &saveptr);
							}
							//1 Registered, home network
							//5 Registered, roaming 漫游 
							if ((1 == creg) || (5 == creg))
							{
								// 查询运营商,后续可以自动寻找APN从文件读取
								if(PppAtCheck(fd, CMD_SIM_TYPE, ackbuf, sizeof(ackbuf)))
								{
									str = strtok_r(ackbuf, delim, &saveptr);
									while(str)
									{
										if (strstr(str, "+COPS:"))
										{
											int fst=0,i=0;
											for(;i<strlen(str),isp_name.length()<128;i++)
											{
												if(str[i]=='\"')
												{
													if(fst==0) {fst=1;continue;}
													else break;
												}
												if(fst==1)
												{
                                                    isp_name=isp_name+str[i];
												}
											}
										}
										str = strtok_r(NULL, delim, &saveptr);
									}
									if(isp_name.length()>0)
                                    {
                                        close(fd);
                                        printf("ISP=%s \n",isp_name.c_str());
                                        return csq;
                                    }
								}
							}
						}	
					}
				}
			}
		}
        sleep(2);
	}
    close(fd);
	return csq;
}

string GetSelfPath()
{
    char buf[128]={0};
    //readlink()会将参数path的符号链接内容存储到参数buf所指的内存空间
    //返回值 ：执行成功则返回字符串的字符数
    // /proc/self/exe 它代表当前程序，所以可以用readlink读取它的源路径就可以获取当前程序的绝对路径
    int cnt = readlink("/proc/self/exe", buf, sizeof(buf));
    if (cnt < 0 || cnt >= sizeof(buf))
    {
        return "";
    }
    //获取当前目录绝对路径，即去掉程序名
    int i;
    for (i = cnt; i >=0; --i)
    {
        if (buf[i] == '/')
        {
            buf[i+1] = '\0';
            break;
        }
    }
    return string(buf);
}

int pppp_test()
{
    string msg;
    string ppp_start_path;
    int sockfd;
    char buf[1024]={0};
    sockfd=createPPPSocket();   //ppp创建套接字
    ppp_start_path=GetSimInfoByDb();    //获取各列元素的内容
    system(close_pppd_process); //关闭ppp
    powerGPRS();     //复位
    if(NetStatus(ifconfig_pppd,msg)==3)
    {
        system(close_pppd_process);
        powerGPRS();  
    }
    int hardware_err_cnt=0;
    int first=0;
    g_lastRecv = time(NULL);
    if(CheckGprsByAt()>0) //检查卡，信号强度，哪家电信公司
    { 
        system(ppp_start_path.c_str());//ppp拨号
    }    
            // 等待pppd拨号成功,预留1min

    for(int i=0;i<60;i++)
    {
      if(NetStatus(ifconfig_pppd,msg)!=3)//检测是否存在ppp0
       {
         sleep(1);
       }
      else
       {
         printf("PPP is Ok\n");
         break;
       }
                
    }
    sleep(1);   
    if(hardware_err_cnt>60)
    {
        printf("Error: Need To reboot\n");
    }
    sleep(10);
return 0; 
}















