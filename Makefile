PLAT ?=linux
CC=g++

ifneq ($(PLAT),linux)
CC=arm-linux-gnueabihf-g++ 
endif

EXTRA_CFLAGS+= -lpthread 
object=485_test_serial.o  485_test.o  readini.o ppp.o infrared.o inter_test.o main.o
#gcc -c -o 485_test.o 485_test.c -lpthread

all: $(object)
	$(CC) -ggdb3  485_test_serial.cpp readini.cpp 485_test.cpp  ppp.cpp infrared.cpp inter_test.cpp main.cpp $(EXTRA_CFLAGS) -o 485test

clean:
	rm  $(object) 485test