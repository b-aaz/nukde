OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nukdesk
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -L/usr/local/lib  -I/usr/local/include 
OBFILES=$(BINDIR)main.o $(BINDIR)nusort.o
COOBFILES=$(COBINDIR)err.o

$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	cc $^ $(CFLAGS) -o $@ -lstdthreads -lX11 -lm -lGL -lGLU -lmagic 
$(BINDIR)nusort.o: $(LIBS)nusort.c 
	cc -c $^ $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)main.o:$(SRC)main.c 
	cc -c $^  $(LIBCFLAGS) $(CFLAGS) -o $@
$(COBINDIR)err.o:$(COLIBS)err.c 
	cc -c $^  $(LIBCFLAGS) $(CFLAGS) -o $@
