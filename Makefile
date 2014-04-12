CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld

kernel.core.uarm : kernel
	elf2uarm -k kernel

kernel : pcb.o asl.o p1test.o
	$(LD) -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o kernel /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o pcb.o asl.o p1test.o

pcb.o : pcb.c
	$(CC) -mcpu=arm7tdmi -c -o pcb.o pcb.c

asl.o : asl.c
	$(CC) -mcpu=arm7tdmi -c -o asl.o asl.c

p1test.o : p1test.c
	$(CC) -mcpu=arm7tdmi -c -o p1test.o p1test.c

clean :
	-rm pcb.o asl.o p1test.o kernel kernel.core.uarm kernel.stab.uarm
