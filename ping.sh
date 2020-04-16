#########################################################################
# File Name: gpio.sh

#!/bin/bash
#检测网络连接  
echo "please scanf ping ip"
read ping
echo "please scanf eth0 ip"
read eth0
ifconfig eth0 $eth0 up
ping -c 3 $ping
if [ $? -eq 0 ];then  
    echo 网络0连接正常
else  
    echo 网络0连接异常  
fi
sleep 1s

ifconfig eth0 down
echo "please scanf eth1 ip"
read eth1
ifconfig eth1 $eth1 up
#ping -c 1 ip > /dev/null 2>&1
ping -c 3 $ping
if [ $? -eq 0 ];then  
    echo 网络1连接正常
else  
    echo 网络1连接异常  
fi





