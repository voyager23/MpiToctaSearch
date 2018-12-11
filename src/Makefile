# Makefile for MpiToctaSearch

.PHONY : clean all go

CFLAGS = -std=c11 -ggdb
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS =  `pkg-config --libs glib-2.0 gsl`

OBJECTS = mpi_tocta_search.o prepare_equalsums.o toolbox.o\
	get_options.o

HEADERS = mpi_tocta_search.h prepare_equalsums.h toolbox.h\
	get_options.h

OBJS = $(addprefix ../build/, $(OBJECTS) )
HDRS = $(addprefix ../include/, $(HEADERS))
VPATH = ../build ../include
OUT = ../build/$@

#-----------------------------------------------------------------------
	
mts : $(OBJECTS)
	gcc $(CFLAGS) $(OBJS) $(LIBS) -o ../bin/mts
	
mpi_tocta_search.o : mpi_tocta_search.c mpi_tocta_search.h
	gcc -c $(CFLAGS) $< -o $(OUT)
	
prepare_equalsums.o : prepare_equalsums.c prepare_equalsums.h
	gcc -c $(CFLAGS) $< -o $(OUT)
	
toolbox.o : toolbox.c toolbox.h
	gcc -c $(CFLAGS) $< -o $(OUT)
	
get_options.o: get_options.c get_options.h
	gcc -c $(CFLAGS) $< -o $(OUT)

#-----------------------------------------------------------------------

go :
	 ../bin/mts -t9,19
#-----------------------------------------------------------------------



