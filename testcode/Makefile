#Makefile for Testcode Folder

gcrypt2 : gcrypt2.c
	gcc gcrypt2.c -lgcrypt `pkg-config --cflags --libs gsl glib-2.0` -o g2c
	
gcrypt : gcrypt.c
	gcc gcrypt.c -lgcrypt -o gc

ga : garray.c
	gcc $< -g `pkg-config --cflags --libs glib-2.0` -o ga

wrap : auto_search.c
	gcc -O2 auto_search.c -o ./auto_search
