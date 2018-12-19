#Makefile


CFLAGS = -std=c11 -ggdb
CFLAGS += `pkg-config --cflags glib-2.0 gsl`
LIBS = ../libcrc/lib/libcrc.a
LIBS +=  `pkg-config --libs glib-2.0 gsl`

gg : glib_gsl.c
	mpicc $(CFLAGS) glib_gsl.c $(LIBS) -o gg
	
hi : hello_c.c
	mpicc hello_c.c -o hi

go :
	mpirun  --hostfile jupiter.hosts ./gg
	
cp_hosts: 
	cp ../../ServerControl/ServerCtl/machinefile.server_ctl jupiter.hosts
