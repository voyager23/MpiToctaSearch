# Makefile for MpiToctaSearch

.PHONY : clean all go

CFLAGS = -std=c11 -ggdb
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS = ../libcrc/lib/libcrc.a
LIBS +=  `pkg-config --libs glib-2.0 gsl`

OBJECTS = mpi_tocta_search.o get_options.o toolbox.o compact_equalsums.o

HEADERS = mpi_tocta_search.h get_options.h toolbox.h compact_equalsums.h

OBJS = $(addprefix ../build/, $(OBJECTS) )
HDRS = $(addprefix ../mpi_include/, $(HEADERS))
#VPATH = ../build ../mpi_include
OUT = ../build/$@

#-----------------------------------------------------------------------

mts : $(OBJECTS)
	mpicc $(CFLAGS) $(OBJS) $(LIBS) -o ../bin/mts
	
mpi_tocta_search.o : mpi_tocta_search.c $(HDRS)
	mpicc -c $(CFLAGS)  $< -o $(OUT)
	
get_options.o: get_options.c $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
toolbox.o : toolbox.c  $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
compact_equalsums.o : compact_equalsums.c  $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
mpi_subset_search.o : mpi_subset_search.c  $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
#-----------------------------------------------------------------------

go :
	@#mpirun -host jupiter:8 ../bin/mts -t 09,19
	@#mpirun -host jupiter:6,mint19:6 ../bin/mts -t 09,19
	mpirun -host mike-XPS-Mint19:8,C6100-4:8 ../bin/mts -q -t 09,19
	 
#-----------------------------------------------------------------------



