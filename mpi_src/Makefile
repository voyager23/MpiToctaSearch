# Makefile for MpiToctaSearch

.PHONY : clean all go

CFLAGS = -std=c11 -O2
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS = ../libcrc/lib/libcrc.a
LIBS +=  `pkg-config --libs glib-2.0 gsl `
LIBS += -lgcrypt

OBJECTS = mpi_tocta_search.o get_options.o toolbox.o compact_equalsums.o classify.o

HEADERS = mpi_tocta_search.h get_options.h toolbox.h compact_equalsums.h classify.h

OBJS = $(addprefix ../build/, $(OBJECTS) )
HDRS = $(addprefix ../mpi_include/, $(HEADERS))
#VPATH = ../build ../mpi_include
OUT = ../build/$@

#-----------------------------------------------------------------------

mts : $(OBJS)
	mpicc $(CFLAGS) $(OBJS) $(LIBS) -o ../bin/mts
	
../build/mpi_tocta_search.o : mpi_tocta_search.c $(HDRS)
	mpicc -c $(CFLAGS)  $< -o $(OUT)
	
../build/get_options.o: get_options.c $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
../build/toolbox.o : toolbox.c  $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
../build/compact_equalsums.o : compact_equalsums.c  $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)
	
../build/classify.o : classify.c $(HDRS)
	mpicc -c $(CFLAGS) $< -o $(OUT)	
	
#-----------------------------------------------------------------------	
	
wrap2 : auto_search_2.c ../mpi_include/auto_search_2.h
	gcc  -O2 auto_search_2.c -o ../bin/wrap2

#-----------------------------------------------------------------------

go48 :
#@mpirun -host jupiter:1,C6100-4:1 ../bin/mts -q -t 09,19
#@mpirun -host jupiter:6,mint19:6 ../bin/mts -t 09,19 -q

#@Target 09,23 expect 1536 solutions
#@Target 09,19 expect 48 solutions
#@Target 06,50 expect 96 solutions
#@mpirun -host jupiter:8,mint19:8 ../bin/mts -q -t 09,23 

#@Localhost only
	mpirun -np 4 ../bin/mts -t 09,19

#@mpirun -host jupiter:8,mint19:8 ../bin/mts -q -t 09,23

go96 :
#@Localhost only
	mpirun -np 4 ../bin/mts -t 06,50

go1536 :
#@Localhost only
	mpirun -np 4 ../bin/mts -t 09,23
	
#-----------------------------------------------------------------------


