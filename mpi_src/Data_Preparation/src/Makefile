# Makefile for MpiToctaSearch

.PHONY : clean all go

CFLAGS = -std=c11 -O2
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS =  `pkg-config --libs glib-2.0 gsl `

OBJECTS = toolbox.o
HEADERS = toolbox.h

OBJS = $(addprefix ../build/, $(OBJECTS) )
HDRS = $(addprefix ../include/, $(HEADERS))
OUT = ../build/$@

#-----------------------------------------------------------------------

cdb : create_database.c
	gcc create_database.c -g $(CFLAGS) $(LIBS) -o ../bin/cdb
	
../build/toolbox.o : toolbox.c
	gcc -c toolbox.c $(CFLAGS) $(LIBS) -o ../build/toolbox.o

