MAKEDIR=@mkdir -p  $$(dirname $@)
OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nukmenu
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -I/usr/local/include 
OBFILES=$(BINDIR)main.o
COOBFILES=$(COBINDIR)spawn.o

$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	$(MAKEDIR)
	cc  $(OBFILES) $(COOBFILES) $(CFLAGS) -o $@ -lX11 -L/usr/local/lib  
$(BINDIR)main.o:$(SRC)main.c $(COLIBS)err.h $(COLIBS)bool.h $(COLIBS)bit.h
	$(MAKEDIR)
	cc -c $(SRC)main.c $(CFLAGS)  -o $@ 
$(COBINDIR)spawn.o:$(COLIBS)spawn.c $(COLIBS)spawn.h $(COLIBS)err.h
	$(MAKEDIR)
	cc -c  $(COLIBS)spawn.c  $(LIBCFLAGS) $(CFLAGS) -o $@
