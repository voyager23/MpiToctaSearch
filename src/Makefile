# Makefile for MpiToctaSearch

.PHONY : clean all go

CFLAGS = -std=c11 -ggdb
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS =  `pkg-config --libs glib-2.0 gsl`

OBJECTS = mpi_tocta_search.o prepare_equalsums.o toolbox.o

HEADERS = mpi_tocta_search.h prepare_equalsums.h toolbox.h

OBJS = $(addprefix ../build/, $(OBJECTS) )
HDRS = $(addprefix ../include/, $(HEADERS))
VPATH = ../build ../include
OUT = ../build/$@

#-----------------------------------------------------------------------
	
#tss : $(OBJECTS) $(HDRS)
#	gcc $(CFLAGS) $(OBJS) $(LIBS) -o ../bin/tss

#tocta_serial_search.o : tocta_serial_search.c tocta_serial_search.h
#	gcc -c $(CFLAGS) $< -o $(OUT)
	
mts : $(OBJECTS)
	gcc $(CFLAGS) $(OBJS) $(LIBS) -o ../bin/mts
	
mpi_tocta_search.o : mpi_tocta_search.c mpi_tocta_search.h
	gcc -c $(CFLAGS) $< -o $(OUT)
	
prepare_equalsums.o : prepare_equalsums.c
	gcc -c $(CFLAGS) $< -o $(OUT)
	
toolbox.o : toolbox.c
	gcc -c $(CFLAGS) $< -o $(OUT)
	
#-----------------------------------------------------------------------

go :
	 ../bin/mts -t9,19
#-----------------------------------------------------------------------



