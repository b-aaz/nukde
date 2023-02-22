OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nukdesk
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -L/usr/local/lib  -I/usr/local/include 
OBFILES=$(BINDIR)main.o $(BINDIR)nusort.o
COOBFILES=

$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	cc  $(OBFILES) $(COOBFILES) $(CFLAGS) -o $@ -lstdthreads -lX11 -lm -lGL -lGLU -lmagic 
$(BINDIR)nusort.o: $(LIBS)nusort.c 
	cc -c   $(LIBS)nusort.c $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)main.o:$(SRC)main.c $(COLIBS)err.h 
	cc -c  $(SRC)main.c  $(LIBCFLAGS) $(CFLAGS) -o $@
