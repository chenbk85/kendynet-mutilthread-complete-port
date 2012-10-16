all:
	gcc -g -o test test.c network.a -I./include -lpthread -lrt
