OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nuksu
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -L/usr/local/lib  -I/usr/local/include 
OBFILES=$(BINDIR)main.o $(BINDIR)strrmadd.o $(BINDIR)auth.o
COOBFILES=$(COBINDIR)err.o

$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	cc $^ $(CFLAGS) -o $@ -lX11 -lm -lcrypt 
$(BINDIR)auth.o: $(LIBS)auth.c 
	cc -c $^ $(LIBCFLAGS) $(CFLAGS) -o $@ 
$(BINDIR)strrmadd.o: $(LIBS)strrmadd.c 
	cc -c $^ $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)main.o:$(SRC)main.c 
	cc -c $^  $(LIBCFLAGS) $(CFLAGS) -o $@
$(COBINDIR)err.o:$(COLIBS)err.c 
	cc -c $^  $(LIBCFLAGS) $(CFLAGS) -o $@
