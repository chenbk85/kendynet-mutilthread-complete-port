all:
	gcc -g -c *.c -I../include
	ar -rc network.a *.o
	mv network.a ../
timer:
	gcc -g -c timer.c -I../include
	ar -rc timer.a *.o
	mv timer.a ../
wheel:
	gcc -g -c timing_wheel.c SysTime.c -I../include
	ar -rc timer.a *.o
	mv timer.a ../		
