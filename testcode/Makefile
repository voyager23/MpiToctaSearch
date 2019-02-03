#Makefile for Testcode Folder

gcrypt3 : gcrypt3.c
	gcc -g gcrypt3.c ../mpi_src/toolbox.c `pkg-config --cflags --libs gsl glib-2.0 openssl` -lgcrypt -o g3c
	
gcrypt2 : gcrypt2.c
	gcc -g gcrypt2.c ../mpi_src/toolbox.c `pkg-config --cflags --libs gsl glib-2.0 openssl` -o g2c
	
gcrypt : gcrypt.c
	gcc gcrypt.c -lgcrypt -o gc

ga : garray.c
	gcc $< -g `pkg-config --cflags --libs glib-2.0` -o ga

wrap : auto_search.c
	gcc -O2 auto_search.c -o ./auto_search
