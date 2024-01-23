MAKEDIR=@mkdir -p  $$(dirname $@)
OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nuksu
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -I/usr/local/include  -Wall -Wextra -std=c89 -Wno-unused-function -pedantic
OBFILES=$(BINDIR)main.o $(BINDIR)strrmadd.o $(BINDIR)auth.o $(BINDIR)widgets.o
COOBFILES=

.PHONY: clean

$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	$(MAKEDIR)
	cc $(OBFILES) $(COOBFILES)  $(CFLAGS) -o $@ -lX11 -lm -lcrypt -L/usr/local/lib  
$(BINDIR)auth.o: $(LIBS)auth.c $(COLIBS)err.h $(COLIBS)bool.h 
	$(MAKEDIR)
	cc -c   $(LIBS)auth.c $(LIBCFLAGS) $(CFLAGS) -o $@ 
$(BINDIR)widgets.o: $(LIBS)widgets.c $(COLIBS)bool.h  $(LIBS)strrmadd.h
	$(MAKEDIR)
	cc -c   $(LIBS)widgets.c $(LIBCFLAGS) $(CFLAGS) -o $@ 
$(BINDIR)strrmadd.o: $(LIBS)strrmadd.c 
	$(MAKEDIR)
	cc -c  $(LIBS)strrmadd.c $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)main.o:$(SRC)main.c $(COLIBS)err.h $(COLIBS)bool.h $(LIBS)auth.h $(LIBS)widgets.h
	$(MAKEDIR)
	cc -c $(SRC)main.c  $(LIBCFLAGS) $(CFLAGS) -o $@


clean: 
	rm $(OBFILES) $(BINDIR)$(BIN)
