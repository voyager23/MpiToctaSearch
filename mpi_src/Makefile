# Makefile for MpiToctaSearch

.PHONY : clean all go

CFLAGS = -std=c11 -ggdb
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS =  `pkg-config --libs glib-2.0 gsl`

OBJECTS = mpi_tocta_search.o get_options.o toolbox.o

HEADERS = mpi_tocta_search.h get_options.h toolbox.h

OBJS = $(addprefix ../build/, $(OBJECTS) )
HDRS = $(addprefix ../mpi_include/, $(HEADERS))
VPATH = ../build ../mpi_include
OUT = ../build/$@

#-----------------------------------------------------------------------

mts : $(OBJECTS)
	mpicc $(CFLAGS) $(OBJS) $(LIBS) -o ../bin/mts
	
mpi_tocta_search.o : mpi_tocta_search.c mpi_tocta_search.h
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
get_options.o: get_options.c get_options.h
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
toolbox.o : toolbox.c toolbox.h
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
prepare_equalsums.o : prepare_equalsums.c prepare_equalsums.h
	gcc -c $(CFLAGS) $< -o $(OUT)
	
#-----------------------------------------------------------------------

go :
	 mpirun -host jupiter:8 ../bin/mts
	 
#-----------------------------------------------------------------------



