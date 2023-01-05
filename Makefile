./bin/desk: ./src/main.c
	cc src/main.c     -O0  -o bin/desk -lX11 -lm -lGL -lGLU -lmagic -lstdthreads -I /usr/local/include/ -L  /usr/local/lib/ -Wall -Waddress -g
