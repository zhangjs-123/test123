#include "inter_test.h"

int connect_check(char *eth)
{
	
	int net_fd;
    char buf[512]={0};
	char statue[20];
	sprintf(buf,"/sys/class/net/%s/operstate",eth);
	net_fd=open(buf,O_RDONLY);//以只读的方式打开/sys/class/net/eth0/operstate
	if(net_fd<0)
	{
	
		printf("open err\n");
		return 0;
	}
	
	memset(statue,0,sizeof(statue));
    int ret=read(net_fd,statue,10);
	if(NULL!=strstr(statue,"up"))
	{
		printf("%s already connect\n",eth);
		return 0;
	}
	else if(NULL!=strstr(statue,"down"))
	{
	   printf("%s not connected\n",eth);
	   return 0;
	}
	else
	{
		printf("unknown err\n");
		return 0;
	}
}