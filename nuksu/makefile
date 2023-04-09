MAKEDIR=@mkdir -p  $$(dirname $@)
OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nuksu
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -L/usr/local/lib  -I/usr/local/include 
OBFILES=$(BINDIR)main.o $(BINDIR)strrmadd.o $(BINDIR)auth.o
COOBFILES=

$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	$(MAKEDIR)
	cc $(OBFILES) $(COOBFILES)  $(CFLAGS) -o $@ -lX11 -lm -lcrypt 
$(BINDIR)auth.o: $(LIBS)auth.c $(COLIBS)err.h 
	$(MAKEDIR)
	cc -c   $(LIBS)auth.c $(LIBCFLAGS) $(CFLAGS) -o $@ 
$(BINDIR)strrmadd.o: $(LIBS)strrmadd.c 
	$(MAKEDIR)
	cc -c  $(LIBS)strrmadd.c $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)main.o:$(SRC)main.c $(COLIBS)err.h $(COLIBS)bool.h
	$(MAKEDIR)
	cc -c $(SRC)main.c  $(LIBCFLAGS) $(CFLAGS) -o $@
