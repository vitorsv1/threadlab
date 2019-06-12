uicp.o: uicp.c 
	gcc -g -c -I /opt/raylib/src uicp.c -o uicp.o

uicp: uicp.o
	gcc -o uicp  uicp.o -s -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -L	GL -lm -lpthread -ldl -lrt -lX11

