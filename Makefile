DST:= App

SRC_PATH:=./device/src/
SRC_PATH1:=./
SRC_PATH2:=./net/src/
INCLUDE_PATH:=./device/include/
INCLUDE_PATH1:=./net/include/
INCLUDE_PATH2:=/opt/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/include/


SRC:=${SRC_PATH1}main.c
SRC+=${SRC_PATH}ds18b20.c
SRC+=${SRC_PATH}gps.c
SRC+=${SRC_PATH}tds.c
SRC+=${SRC_PATH}ts300b.c
SRC+=${SRC_PATH}mailbox.c
SRC+=${SRC_PATH2}server.c
SRC+=$(SRC_PATH)uart.c

CC:=arm-linux-gnueabihf-gcc
#CC:=gcc
$(DST):$(SRC)
	$(CC) $^ -o  $@ -I$(INCLUDE_PATH) -I$(INCLUDE_PATH1) -I$(INCLUDE_PATH2) -lpthread -lm -L/home/xbc/nfs/rootfs/lib/ -lsqlite3
 
.PHONY:
clean:
	rm ${DST} -rf