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



double soln0[4][8] = {	{4,11, 1,6, 2,13, 2,3},
						{2,13, 3,8, 1,2, 3,10},
						{1,2, 4,9, 2,7, 2,15},
						{2,7, 1,10, 4,11, 2,5} };
						
double soln1[4][8] = {	{2,5, 4,11, 1,10, 2,7},
						{1,10, 1,6, 3,8, 4,9},
						{3,8, 2,13, 3,10, 1,2},
						{3,10, 2,3, 2,5, 2,15} };
						
void prt_gsl_matrix_complex(gsl_matrix_complex *m);
void prt_gsl_matrix_complex(gsl_matrix_complex *m) {
	for(int row = 0; row < 4; ++row) {
		for(int col = 0; col < 4; ++col) {
			gsl_complex *p = gsl_matrix_complex_ptr(m, row, col);
			printf("%2.0f,%2.0f ", GSL_REAL(*p), GSL_IMAG(*p));
		}
		printf("\n");
	}
	printf("\n");
}

char* posn_independant_signature(gsl_matrix_complex *m);
char* posn_independant_signature(gsl_matrix_complex *m) {
	/* return a pointer to a 20 byte sha128 digest
	 * 	for each node:
	 *		calc and save a 20 byte digest
	 * 	sort array of digests
	 * 	calc final sha128 for 6 digests
	 */
	
	// allocate memory for a total of 7 digests

	
}

int main(int argc, char **argv)
{
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
	
	
	
	return 0;
}

