/*
 * gcrypt2.c
 * 
 * Copyright 2019 mike <mike@mike-XPS-Mint19>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_matrix.h>
#include "../mpi_include/toolbox.h"
#include <gcrypt.h>

//----------------------------------------------------------------------
double soln0[4][8] = {	{4,11, 1,6, 2,13, 2,3},
						{2,13, 3,8, 1,2, 3,10},
						{1,2, 4,9, 2,7, 2,15},
						{2,7, 1,10, 4,11, 2,5} };
						
double soln1[4][8] = {	{2,5, 4,11, 1,10, 2,7},
						{1,10, 1,6, 3,8, 4,9},
						{3,8, 2,13, 3,10, 1,2},
						{3,10, 2,3, 2,5, 2,15} };
//----------------------------------------------------------------------			
void prt_gsl_matrix_complex(gsl_matrix_complex *m);
void prt_gsl_matrix_complex(gsl_matrix_complex *m) {
	for(int row = 0; row < m->size1; ++row) {
		for(int col = 0; col < m->size2; ++col) {
			gsl_complex *p = gsl_matrix_complex_ptr(m, row, col);
			printf("%2.0f,%2.0f ", GSL_REAL(*p), GSL_IMAG(*p));
		}
		printf("\n");
	}
	printf("\n");
}
//----------------------------------------------------------------------
int cmp4complex(const void *left, const void *right);
int cmp4complex(const void *left, const void *right) {
	// cast the void pointers as gsl_complex *p
	gsl_complex *pl = (gsl_complex*)left;
	gsl_complex *pr = (gsl_complex*)right;
	for(int i = 0; i < 4; ++i) {
		int result = compare_gsl_complex(pl++, pr++);
		if(result != 0) return result;
	}
	return 0;
}
//-----------------------------------------------------------------------
char* posn_independant_signature(gsl_matrix_complex *m, int algo);
char* posn_independant_signature(gsl_matrix_complex *m, int algo) {
	/* Expects pointers to a matrix_complex and the algorithm
	 * to use (GCRY_MD_SHA1). Returns a pointer to a digest of the
	 * correct length.
	 */
	gcry_error_t e;
	gcry_md_hd_t hd;
	e = gcry_md_open(&hd, algo, 0);	
	
	
	// allocate workspace as a contiguous vector of gsl_complex_double
	gsl_vector_complex *wspace = gsl_vector_complex_alloc(6*4);
	
	// copy rows and cols to wspace
	gsl_vector_complex *buffer = gsl_vector_complex_alloc(4);
	for(int i = 0; i < 4; ++i) 
		memcpy(gsl_vector_complex_ptr(wspace, i*4), gsl_matrix_complex_ptr(m, i, 0), sizeof(gsl_complex)*4);
	// get/set col3 - row 4
	gsl_matrix_complex_get_col(buffer, m, 3);
	// vector copy
	for(int i = 0; i < 4; ++i) 
		gsl_vector_complex_set(wspace, (16 + i), gsl_vector_complex_get(buffer, i));	
	// get/set col 1 - row 5
	gsl_matrix_complex_get_col(buffer, m, 1);
	// vector copy
	for(int i = 0; i < 4; ++i) 
		gsl_vector_complex_set(wspace, (20 + i), gsl_vector_complex_get(buffer, i)); 

	// sort each row into ascending order
	for(int r = 0; r < 6; ++r) 		
		qsort(gsl_vector_complex_ptr(wspace, (r*4)), 4, sizeof(gsl_complex), compare_gsl_complex);
	
	// resort 6 rows into ascending order
	qsort(gsl_vector_complex_ptr(wspace,0), 6, (sizeof(gsl_complex) * 4), cmp4complex);
	
	// calc sha128 hash for matrix wspace
	gcry_md_reset(hd);
	gcry_md_write(hd, wspace, sizeof(gsl_complex)*24);
	
	// return pointer to 20 char digest

	printf("Workspace\n");
	for(int row = 0; row < 6; ++row) {
		for(int col = 0; col < 4; ++col) {
			gsl_complex *p = gsl_vector_complex_ptr(wspace, row*4 + col);
			printf("%2.0f,%2.0f ", GSL_REAL(*p), GSL_IMAG(*p));
		}
		printf("\n");
	}
	printf("\n");
	
	// Cleanup code 
	// gcry_md_close (hd);	
	return(gcry_md_read(hd,GCRY_MD_SHA1));
}

//======================================================================
int main(int argc, char **argv)
{
	
	/* Version check should be the very first call because it
	 makes sure that important subsystems are initialized. */
	if (!gcry_check_version (GCRYPT_VERSION))
	{
	  fputs ("libgcrypt version mismatch\n", stderr);
	  exit (2);
	}

	/* Disable secure memory.  */
	gcry_control (GCRYCTL_DISABLE_SECMEM, 0);

	/* ... If required, other initialization goes here.  */

	/* Tell Libgcrypt that initialization has completed. */
	gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
	
	gsl_matrix_complex *mat0 = gsl_matrix_complex_alloc(4,4);
	gsl_matrix_complex *mat1 = gsl_matrix_complex_alloc(4,4);
	
	for(int row = 0; row < 4; ++row) {
		for(int col = 0; col < 8; col+=2) {
			gsl_complex t = gsl_complex_rect(soln0[row][col], soln0[row][col+1]);
			gsl_matrix_complex_set(mat0, row, col/2, t);
		}
	}
	
	for(int row = 0; row < 4; ++row) {
		for(int col = 0; col < 8; col+=2) {
			gsl_complex t = gsl_complex_rect(soln1[row][col], soln1[row][col+1]);
			gsl_matrix_complex_set(mat1, row, col/2, t);
		}
	}
	
	prt_gsl_matrix_complex(mat0);
	prt_gsl_matrix_complex(mat1);
		
	char *digest = posn_independant_signature(mat0, GCRY_MD_SHA1);
	
	for(int i = 0; i < 20; ++i) printf("%02x ", digest[i]&0x00ff);
	printf("\n");
	
	//free(digest);
	
		
	digest = posn_independant_signature(mat1, GCRY_MD_SHA1);
	
	for(int i = 0; i < 20; ++i) printf("%02x ", digest[i]&0x00ff);
	printf("\n");
	
	//free(digest);
	
	return 0;
}

