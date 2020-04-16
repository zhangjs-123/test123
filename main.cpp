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
#include "485_test_serial.h"
#include "485_test.h"
#include "ppp.h"
#include "readini.h"
#include "infrared.h"
#include "inter_test.h"


using namespace std;
int fd1 = 0; 
int fd2 = 0;  
static int j;
static int buzzer;
static int IO_RYB2_Ct; 
static int IO_RYA2_Ct;
static int IO_RYB1_Ct; 
static int IO_RYA1_Ct;
static int IO_RYB0_Ct; 
static int IO_RYA0_Ct;

void receve_usrt1 (void);
void receve_usrt2 (void);

const char GPIO_FILE[] = "/sys/class/gpio/export";
const char GPIO_AA1[] ="/sys/class/gpio";
const char GPIO_AA2[] ="/sys/class/gpio";
char uart_port[]="/dev/ttymxc7";

struct KeyStatu
{
    const char *name;
    int inout;
    int num;
};
struct KeyStatu keyStatus[8] = {
    {"keyup",1},
    {"keydown",1},
    {"keyleft",1},
    {"keyright",1},
    {"keydo",1},
    {"keycn",1},
    {"keyopent",1},
    {"keyopenm",1},
};
struct KeyStatu keyStatus_power[2] = {
    {"key1_power",1},
    {"key2_power",1},
};

struct KeyStat
{
    const char *name;
    int num;
};

struct KeyStat LEDIO[7] = {
    {"led6"},
    {"led3"},
    {"led7"},
    {"led8"},
    {"led18"},
    {"led19"},
    {"led20"},
};

int opengpio(int sum)
{   char ss[10];
    sprintf(ss,"%d",sum);
    int fd1=open(GPIO_FILE, O_WRONLY);
    if(fd1<0)
    {
        perror("open fail");
        return 0;
    } 
    write(fd1,ss,sizeof(ss));
    close(fd1);
    return 0;
}

int gpionumber(int num)
{
    int math1=num/100;
    int math2=num%100;
    int sum=(math1-1)*32+math2;
    return sum;
}

int writeinout(int sum,char *inout)
{   char gpio1[50]={0};
    sprintf(gpio1,"/sys/class/gpio/gpio%d/direction",sum);
    int fd2=open(gpio1, O_RDWR);
    int writ2=write(fd2,inout,sizeof(inout));
    close(fd2);
    return 0;
}

char* readin(int sum)
{  char gpio2[50]={0};
   char *buf;
   buf=(char *)malloc(sizeof(char)*2);
   sprintf(gpio2,"/sys/class/gpio/gpio%d/value",sum);
   int fd3=open(gpio2, O_RDWR);
   int ret=read(fd3,buf,sizeof(buf));
   close(fd3);
   return buf;
}

int writeoutupdown(int sum,char *updown)
{
   char gpio2[50]={0};
   sprintf(gpio2,"/sys/class/gpio/gpio%d/value",sum);
   int fd3=open(gpio2, O_RDWR);
   int ret=write(fd3,updown,sizeof(updown));
   close(fd3);
   return fd3;
}

int led_test()
{   
    char gpio2[50]={0};
    char inout[]="out";
    for (int i = 0; i < 7; i++)
    {    
        int gpio=gpionumber(LEDIO[i].num);
        opengpio(gpio);
        writeinout(gpio,inout);
        sprintf(gpio2,"/sys/class/gpio/gpio%d/value",gpio);
        int fd3=open(gpio2, O_RDWR);
        int ret1=write(fd3,"0",sizeof("0"));
        sleep(1);
        int ret2=write(fd3,"1",sizeof("1"));
        close(fd3);
    }
     return 0;   
}

int read_key_status()
{

    char buf[10];
    char input[]="in";
    char gpio2[50]={0};
    char inout2[]="out";
    char a[]="1";

    for (int i = 0; i < 8; i++)
    {   
        int gpio=gpionumber(keyStatus[i].num);
        opengpio(gpio);
        writeinout(gpio,input);
        sprintf(gpio2,"/sys/class/gpio/gpio%d/value",gpio);
        int fd3=open(gpio2, O_RDWR);
        int ret=read(fd3,buf,strlen(buf));
        close(fd3);

        printf("%s:%s",keyStatus[i].name,buf);  
    }
    printf("\n");
    return 0;
}

int  key_test()
{  
   // system("ls -al ./readio.sh");
   char buf[5];
   char gpio2[50];
   char inzt[]="00000000";
   char inout[]="in";
   char inout2[]="out";
   char a[]="1";
   char abc[]="11111111";
 
    for (int i = 0; i < 2; i++)
    {   
        opengpio(keyStatus_power[i].num);
        writeinout(keyStatus_power[i].num,inout2);
        writeoutupdown(keyStatus_power[i].num,a);
    }

    while(1)
   {
    for (int i = 0; i < 8; i++)
     {   
        int gpio=gpionumber(keyStatus[i].num);
        opengpio(gpio);
        writeinout(gpio,inout);
        sprintf(gpio2,"/sys/class/gpio/gpio%d/value",gpio);
        int fd3=open(gpio2, O_RDWR);
        int ret=read(fd3,buf,sizeof(buf));
       // printf("buf=%s",buf);
        close(fd3);
        if(buf[0]!=keyStatus[i].inout)
        {   
            
            printf("%s aleady use!\n",keyStatus[i].name);
             j++;
            if(j>11)
            {
            inzt[i] = '1';
            printf("%c\n",inzt[i]);
            printf("%s\n",inzt);
            if(strcmp(inzt,abc)==0)
          {
            return 0;
          }
            }
            if(j>111111)
            {
                j=11;
            }
          keyStatus[i].inout=(int)buf[0];
        }    
    }
   }
   return 0;
}

int  buzzer_off()
{
    char a[]="0";
    char inout[]="out";  
    opengpio(buzzer);
    writeinout(buzzer,inout);
    writeoutupdown(buzzer,a);
    printf("buzze off");
    return 0;
}

int  buzzer_on()
{

    char inout[]="out";
    char a[]="1";
    opengpio(buzzer);
    writeinout(buzzer,inout);
    writeoutupdown(buzzer,a);
    printf("buzze on");  
    return 0; 
}

int Relay_u8_off()
{ 
    char inout[]="out";
    char a[]="1";
    char b[]="0";
   
    int gpio1=gpionumber(IO_RYB2_Ct);
    int gpio2=gpionumber(IO_RYA2_Ct);
    opengpio(gpio1);
    opengpio(gpio2);
    writeinout(gpio1,inout);
    writeinout(gpio2,inout);
    writeoutupdown(gpio1,a);
    writeoutupdown(gpio2,b);
    return 0;
}

int Relay_u8_on()
{
    char inout[]="out";
    char a[]="1";
    char b[]="0";
    int gpio1=gpionumber(IO_RYB2_Ct);
    int gpio2=gpionumber(IO_RYA2_Ct);
    opengpio(gpio1);
    opengpio(gpio2);
    writeinout(gpio1,inout);
    writeinout(gpio2,inout);
    writeoutupdown(gpio1,b);
    writeoutupdown(gpio2,a);
    return 0;
}
int Relay_u9_off()
{ 
    char inout[]="out";
    char a[]="1";
    char b[]="0";
   
    int gpio1=gpionumber(IO_RYB1_Ct);
    int gpio2=gpionumber(IO_RYA1_Ct);
    opengpio(gpio1);
    opengpio(gpio2);
    writeinout(gpio1,inout);
    writeinout(gpio2,inout);
    writeoutupdown(gpio1,a);
    writeoutupdown(gpio2,b);
    return 0;
}

int Relay_u9_on()
{
    char inout[]="out";
    char a[]="1";
    char b[]="0";
    int gpio1=gpionumber(IO_RYB1_Ct);
    int gpio2=gpionumber(IO_RYA1_Ct);
    opengpio(gpio1);
    opengpio(gpio2);
    writeinout(gpio1,inout);
    writeinout(gpio2,inout);
    writeoutupdown(gpio1,b);
    writeoutupdown(gpio2,a);
    return 0;
}

int Relay_k1_off()
{ 
    char inout[]="out";
    char a[]="1";
    char b[]="0";
   
    int gpio1=gpionumber(IO_RYB0_Ct);
    int gpio2=gpionumber(IO_RYA0_Ct);
    opengpio(gpio1);
    opengpio(gpio2);
    writeinout(gpio1,inout);
    writeinout(gpio2,inout);
    writeoutupdown(gpio1,a);
    writeoutupdown(gpio2,b);
    return 0;
}

int Relay_k1_on()
{
    char inout[]="out";
    char a[]="1";
    char b[]="0";
    int gpio1=gpionumber(IO_RYB0_Ct);
    int gpio2=gpionumber(IO_RYA0_Ct);
    opengpio(gpio1);
    opengpio(gpio2);
    writeinout(gpio1,inout);
    writeinout(gpio2,inout);
    writeoutupdown(gpio1,b);
    writeoutupdown(gpio2,a);
    return 0;
}

int test_rtc()
{  
    FILE * fp0;
    FILE * fp1;
    char buffer[80];
    fp0=popen(" hwclock -f /dev/rtc0","r");
    if(fp0<0){
        perror("rtc0 fail!");
    }
    if(fp0>0){
        printf("rtc0 time:");
        fgets(buffer,sizeof(buffer),fp0);
        printf("%s",buffer);
        pclose(fp0);
    }

    fp1=popen(" hwclock -f /dev/rtc1","r");
    if(fp1<0){
        perror("rtc0 fail!");
    }
    if(fp1>0){
         printf("rtc1 time:");
        fgets(buffer,sizeof(buffer),fp1);
        printf("%s",buffer);
        pclose(fp1);
    }
    return 0;
}

int ppp_test() {
     pppp_test(); 
     return 0;   
}

int internet_test() {
    char eth0[]="eth0";
    char eth1[]="eth1";
    connect_check(eth0);
    system("ifconfig eth1 192.168.1.123 up");
    connect_check(eth1);
  return 0;
}

int near_infrared(){
     test_infrared();
     return 0;
}

int  Bluetooth()
{
   system("microcom -s 9600 /dev/ttymcs4");
}
int Power_failuretest()
{   
    char buf[10];
    char input[]="in";
    char gpio2[50]={0};
    char a[]="1";
    char level1;
    char level2;
    int fd3;

    int gpio=gpionumber(221);
    opengpio(gpio);
    writeinout(gpio,input);
        
    printf("-----电源io口的电平");
    sprintf(gpio2,"/sys/class/gpio/gpio%d/value",gpio);
    for(int i=1;i<5;i++)
    {
    fd3=open(gpio2, O_RDWR);
    int ret=read(fd3,buf,strlen(buf));
    printf("%s\n",buf);
    sleep(1);
    }
    level1=buf[0];
    close(fd3);
    printf("----- 请关闭电源");
    printf("------电源io口的电平");
    for(int i=1;i<5;i++)
    {
    fd3=open(gpio2, O_RDWR);
    int ret=read(fd3,buf,strlen(buf));
    printf("%s\n",buf);
    sleep(1);
    }
    level2=buf[0];
    close(fd3);
    if(level1 == '1' && level2 == '0' )
    {
      printf("掉电检测正常\n");
    }  
    else
    {
      printf("掉电检测不正常\n");
    }
    return 0;
}

int  allproject()
{  
    printf("===================开始检测灯===================\n");
    led_test(); 
    printf("===================当前按键状态=================\n");
    read_key_status(); 
    printf("===================检测按键====================\n");
    printf("请按按键");
    key_test();

    printf("===================检测蜂鸣器==================\n");
    printf("======打开蜂鸣器======\n");
    sleep(3);
    buzzer_off();
    printf("======关闭蜂鸣器======\n");
    sleep(3);
 

    printf("===================检测继电器==================\n");
    Relay_u8_off();
    printf("======打开继电器u8======\n");
    sleep(2);
    Relay_u8_on();
    printf("======关闭继电器u8======\n");
    sleep(2);

    Relay_u9_off();
    printf("======打开继电器u9======\n");
    sleep(2);
    Relay_u9_on();
    printf("======关闭继电器u9======\n");
    sleep(2);

    Relay_k1_off();
    printf("======打开继电器k1======\n");
    sleep(2);
    Relay_k1_on();
    printf("======关闭继电器k1======\n");
    sleep(2);
    
    printf("===================检测RTC===================\n");
    test_rtc();
    printf("===================检测INTERNET===================\n");
    internet_test();
    printf("===================掉电检测===================\n");
    Power_failuretest();
    printf("===================检测拨号===================\n");  //////////////////////////
    ppp_test();
    printf("===================检测485===================\n");
    test_485();
    return 0;
  
}


int main() 
{
    int choose = -1;
    int pin;
    CIni ini;
    int fd=ini.OpenFile("./various-data","r");
    buzzer=ini.GetInt("section1","RYS_C1_Zr");
    IO_RYB2_Ct=ini.GetInt("section1","RYS_B2_Ct");
    IO_RYA2_Ct=ini.GetInt("section1","RYS_A2_Ct");
    IO_RYB1_Ct=ini.GetInt("section1","RYS_B1_Ct");
    IO_RYA1_Ct=ini.GetInt("section1","RYS_A1_Ct");
    IO_RYB0_Ct=ini.GetInt("section1","RYS_B0_Ct");
    IO_RYA0_Ct=ini.GetInt("section1","RYS_A0_Ct");
    keyStatus[0].num=ini.GetInt("section1","KEY_UP"); 
    keyStatus[1].num=ini.GetInt("section1","KEY_DOWN");   
    keyStatus[2].num=ini.GetInt("section1","KEY_LEFT");
    keyStatus[3].num=ini.GetInt("section1","KEY_RIGHT");
    keyStatus[4].num=ini.GetInt("section1","KEYDn_Dt");
    keyStatus[5].num=ini.GetInt("section1","KeyCn_Dt");
    keyStatus[6].num=ini.GetInt("section1","KeyOpenT_Dt");
    keyStatus[7].num=ini.GetInt("section1","KeyOpenM_Dt");
    keyStatus_power[0].num=ini.GetInt("section1","KeyEn_Ct");
    keyStatus_power[1].num=ini.GetInt("section1","KeyTm_Ct");
    LEDIO[0].num=ini.GetInt("section1","IO_Alm_LED_Ct");
    LEDIO[1].num=ini.GetInt("section1","IO_RUN_LED_Ct");
    LEDIO[2].num=ini.GetInt("section1","IO_Read485_LED_Ct");
    LEDIO[3].num=ini.GetInt("section1","IO_Cascade485_LED_Ct");
    LEDIO[4].num=ini.GetInt("section1","IO_Wan_LED_Ct");
    LEDIO[5].num=ini.GetInt("section1","IO_Lan_LED_Ct");
    LEDIO[6].num=ini.GetInt("section1","IO_Bt_LED_Ct");
    for (;;)
    {   
        printf("1. led_test\n");         
        printf("2. read_key_status\n");  
        printf("3. key_test\n");         
        printf("4. buzzer_off\n");
        printf("5. buzzer_on\n");
        printf("6. Relay_off\n");
        printf("7. Relay_on\n");
        printf("8. test_rtc\n");         
        printf("9. internet_test\n"); 
        printf("10.Power_failuretest\n");  
        printf("11. 485_test\n");       
        printf("12. ppp_test\n");      
        printf("13.Bluetooth\n");
        printf("14.near infrared\n");  
        printf("15. allproject(1-12)\n");
        printf("0. quit\n");
        printf("input: ");
        scanf("%d", &choose);
        printf("input number is: %d\n", choose);
        switch (choose)
        {
        case 0:
            return 0;
        case 1:
            led_test();
            break;
        case 2:
            read_key_status();
            break;
        case 3:
            key_test();
            break;
        case 4:
            buzzer_off();
            break;
        case 5:
            buzzer_on();
            break;
        case 6:
            Relay_u8_off();
            sleep(1);
            Relay_u9_off();
            sleep(1);
            Relay_k1_off();
            break;
        case 7:
            Relay_u8_on();
            sleep(1);
            Relay_u9_on();
            sleep(1);
            Relay_k1_on();
            break;
        case 8:
            test_rtc();
            break;
        case 9:
          internet_test(); 
            break;
         case 10:
           Power_failuretest();
            break;
        case 11:
         test_485();
            break;   
        case 12:
         ppp_test();
            break;
        case 13:
          Bluetooth();
            break; 
        case 14:
           near_infrared();
            break;   
        case 15:
            allproject();
        break;
        default:
           return 0;
        }
        printf("------------------------------\n");
    }
    return 0;
}