MAKEDIR=@mkdir -p  $$(dirname $@)
OP=-O3
COLIBS=../colibs/
SRC=./src/
LIBS=$(SRC)lib/
BIN=nukdesk
BINDIR=./bin/
COBINDIR:=$(COLIBS)bin/
CFLAGS=$(OP) -Wall  -g -I/usr/local/include 
OBFILES=$(BINDIR)main.o $(BINDIR)nusort.o $(BINDIR)ffload.o $(BINDIR)fileops.o $(BINDIR)config-parser.o $(BINDIR)icon-widget.o $(BINDIR)icon-loader.o $(BINDIR)sortfiles.o
COOBFILES=$(COBINDIR)spawn.o


$(BINDIR)$(BIN):$(OBFILES) $(COOBFILES) 
	$(MAKEDIR)
	cc  $(OBFILES) $(COOBFILES) $(CFLAGS) -o $@ -lstdthreads -lX11 -lm -lGL -lGLU -lmagic -L/usr/local/lib  
$(BINDIR)nusort.o: $(LIBS)nusort.c $(LIBS)nusort.h 
	$(MAKEDIR)
	cc -c   $(LIBS)nusort.c $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)main.o:$(SRC)main.c $(COLIBS)err.h $(COLIBS)bool.h $(COLIBS)spawn.h $(LIBS)icon-widget.h  $(LIBS)icon-loader.h $(LIBS)fileops.h $(LIBS)sortfiles.h  $(LIBS)nuklear_xlib_gl3.h
	$(MAKEDIR)
	cc -c  $(SRC)main.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)ffload.o:$(LIBS)ffload.c $(LIBS)ffload.h $(COLIBS)err.h 
	$(MAKEDIR)
		cc -c  $(LIBS)ffload.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)config-parser.o:$(LIBS)config-parser.c $(LIBS)config-parser.h $(COLIBS)err.h 
	$(MAKEDIR)
		cc -c  $(LIBS)config-parser.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)fileops.o:$(LIBS)fileops.c $(LIBS)fileops.h $(COLIBS)err.h $(COLIBS)bool.h  $(LIBS)ffload.h $(LIBS)config-parser.h $(LIBS)fileinfo_type.h 
	$(MAKEDIR)
		cc -c  $(LIBS)fileops.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)icon-widget.o:$(LIBS)icon-widget.c $(LIBS)icon-widget.h $(COLIBS)err.h $(COLIBS)bool.h $(COLIBS)nuklear.h $(LIBS)nuklear_xlib_gl3.h
	$(MAKEDIR)
		cc -c  $(LIBS)icon-widget.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)icon-loader.o:$(LIBS)icon-loader.c $(LIBS)icon-loader.h $(COLIBS)err.h $(COLIBS)nuklear.h $(LIBS)stb_image_resize.h $(LIBS)ffload.h $(LIBS)fileinfo_type.h 
	$(MAKEDIR)
		cc -c  $(LIBS)icon-loader.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(BINDIR)sortfiles.o:$(LIBS)sortfiles.c $(LIBS)sortfiles.h $(COLIBS)err.h $(LIBS)fileinfo_type.h $(LIBS)nusort.h
	$(MAKEDIR)
		cc -c  $(LIBS)sortfiles.c  $(LIBCFLAGS) $(CFLAGS) -o $@
$(COBINDIR)spawn.o:$(COLIBS)spawn.c $(COLIBS)spawn.h $(COLIBS)err.h
	$(MAKEDIR)
		cc -c  $(COLIBS)spawn.c  $(LIBCFLAGS) $(CFLAGS) -o $@
