#for windows compile
CC = i586-mingw32msvc-gcc -std=c99 -I./inc/
CFLAGS = -c -std=c99 -I./inc/
#USE = -L./lib/ -mwindows -lglfw3 -lglu32 -lopengl32 -Wl,--subsystem,windows
USE = -L./lib/ -mwindows -lglfw3 -lglu32 -lopengl32 -Wl,--subsystem,console

#for linux compile
#CC = gcc -std=c99 -g
#USE = -lglfw3 -lrt -lXrandr -lXinerama -lXi -lXcursor -lGL -lGLU -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

OBJECTS = main.o tetromino.o game.o NEAT.o graphics.o text.o console.o ui.o NEATContainers.o events.o

%.o : %.c
	$(CC) -c -o $@ $<

game : $(OBJECTS)
	$(CC) -o tetris.exe $(OBJECTS) $(USE)

.PHONY : clean
clean :
	rm -rf tetris.exe $(OBJECTS)

