#!/bin/sh 

io_rst="92"
io_switch="87"
io_power="83"

io_rst_val="/sys/class/gpio/gpio"$io_rst"/value"
io_switch_val="/sys/class/gpio/gpio"$io_switch"/value"
io_power_val="/sys/class/gpio/gpio"$io_power"/value"

for n in $io_rst $io_switch $io_power
do
    echo $n > /sys/class/gpio/unexport 
    echo $n > /sys/class/gpio/export
    echo out > "/sys/class/gpio/gpio"$n"/direction"
done



power_down(){
    echo 0   > $io_rst_val
    echo 1   > $io_switch_val
    echo 0   > $io_power_val
}

power_switch(){
    echo 0   > $io_rst_val
    echo 0   > $io_power

    echo 0   > $io_switch_val
    sleep 1
    echo 1   > $io_switch_val
}


power_on(){
    echo 0   > $io_rst_val
    echo 1   > $io_switch_val
    echo 1   > $io_power_val
}


# 没有参数 =on
echo usage:$0 [on/off]
if [ $# == 0 ] ||  [ $1 == "on" ]
then
   echo power_on
   power_on
else
   echo power_down
   power_down
fi
