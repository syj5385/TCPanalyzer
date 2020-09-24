
# Makefile

CC = gcc
CFLAGS = -g

MAIN_SRC = main.c package.c network.c result.c
MAIN_OBJECTS = main.o package.o network.o result.o
MAIN_TARGET = Generator

all : clean build
	$(trgenerator)

build : $(MAIN_OBJECTS)
	$(CC) -o $(MAIN_TARGET) $(MAIN_OBJECTS) -lpthread
	
clean : 
	rm -f $(MAIN_OBJECTS) $(MAIN_TARGET) core
	rm -f ifstat.tmp core
	rm -f probe.tmp core
	rm -f ./output/throughput.xls ./output/cwnd.xls ./output/rtt.xls core

main.o : main.c
package.o : package.c
network.o : network.c
result.o : result.c
