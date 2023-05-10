MAKEDIR=@mkdir -p  $$(dirname $@)
OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nukmenu
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP)  -g -L/usr/local/lib  -I/usr/local/include 

$(BINDIR)$(BIN): $(SRC)main.c
	$(MAKEDIR)
	cc $(CFLAGS)  $(SRC)main.c -o $@ -lX11 -lm -lcrypt 
