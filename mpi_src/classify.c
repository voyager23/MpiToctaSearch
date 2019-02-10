/*
 * classify.c
 * 
 * Copyright 2018 mike <mike@mike-XPS-15-9560>
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

#include "../mpi_include/classify.h"

// Lists indexes into gsl_vector_complex elem_value for all rotations
// This allows each transformation to be built
const unsigned rotrefs[48][16] =
{
// -----identity configuration-------------------
{  0, 5, 1, 4, 1, 7, 2, 6, 2, 9, 3, 8, 3,11, 0,10},
// a, f, b, e, b, h, c, g, c, j, d, i, d, y, a, x
// ----------------------------------------------
{0,11,3,10,3,9,2,8,2,7,1,6,1,5,0,4},
{3,11,0,10,0,5,1,4,1,7,2,6,2,9,3,8},
{3,9,2,8,2,7,1,6,1,5,0,4,0,11,3,10},
{2,9,3,8,3,11,0,10,0,5,1,4,1,7,2,6},
{2,7,1,6,1,5,0,4,0,11,3,10,3,9,2,8},
{1,7,2,6,2,9,3,8,3,11,0,10,0,5,1,4},
{1,5,0,4,0,11,3,10,3,9,2,8,2,7,1,6},
{10,4,6,8,6,1,7,2,7,5,11,9,11,0,10,3},
{10,0,11,3,11,5,7,9,7,1,6,2,6,4,10,8},
{11,0,10,3,10,4,6,8,6,1,7,2,7,5,11,9},
{11,5,7,9,7,1,6,2,6,4,10,8,10,0,11,3},
{7,5,11,9,11,0,10,3,10,4,6,8,6,1,7,2},
{7,1,6,2,6,4,10,8,10,0,11,3,11,5,7,9},
{6,1,7,2,7,5,11,9,11,0,10,3,10,4,6,8},
{6,4,10,8,10,0,11,3,11,5,7,9,7,1,6,2},
{8,10,4,6,4,0,5,1,5,11,9,7,9,3,8,2},
{8,3,9,2,9,11,5,7,5,0,4,1,4,10,8,6},
{9,3,8,2,8,10,4,6,4,0,5,1,5,11,9,7},
{9,11,5,7,5,0,4,1,4,10,8,6,8,3,9,2},
{5,11,9,7,9,3,8,2,8,10,4,6,4,0,5,1},
{5,0,4,1,4,10,8,6,8,3,9,2,9,11,5,7},
{4,0,5,1,5,11,9,7,9,3,8,2,8,10,4,6},
{4,10,8,6,8,3,9,2,9,11,5,7,5,0,4,1},
{2,6,1,7,1,4,0,5,0,10,3,11,3,8,2,9},
{2,8,3,9,3,10,0,11,0,4,1,5,1,6,2,7},
{3,8,2,9,2,6,1,7,1,4,0,5,0,10,3,11},
{3,10,0,11,0,4,1,5,1,6,2,7,2,8,3,9},
{0,10,3,11,3,8,2,9,2,6,1,7,1,4,0,5},
{0,4,1,5,1,6,2,7,2,8,3,9,3,10,0,11},
{1,4,0,5,0,10,3,11,3,8,2,9,2,6,1,7},
{1,6,2,7,2,8,3,9,3,10,0,11,0,4,1,5},
{7,2,6,1,6,8,10,4,10,3,11,0,11,9,7,5},
{7,9,11,5,11,3,10,0,10,8,6,4,6,2,7,1},
{11,9,7,5,7,2,6,1,6,8,10,4,10,3,11,0},
{11,3,10,0,10,8,6,4,6,2,7,1,7,9,11,5},
{10,3,11,0,11,9,7,5,7,2,6,1,6,8,10,4},
{10,8,6,4,6,2,7,1,7,9,11,5,11,3,10,0},
{6,8,10,4,10,3,11,0,11,9,7,5,7,2,6,1},
{6,2,7,1,7,9,11,5,11,3,10,0,10,8,6,4},
{5,1,4,0,4,6,8,10,8,2,9,3,9,7,5,11},
{5,7,9,11,9,2,8,3,8,6,4,10,4,1,5,0},
{9,7,5,11,5,1,4,0,4,6,8,10,8,2,9,3},
{9,2,8,3,8,6,4,10,4,1,5,0,5,7,9,11},
{8,2,9,3,9,7,5,11,5,1,4,0,4,6,8,10},
{8,6,4,10,4,1,5,0,5,7,9,11,9,2,8,3},
{4,6,8,10,8,2,9,3,9,7,5,11,5,1,4,0},
{4,1,5,0,5,7,9,11,9,2,8,3,8,6,4,10}
};

const int elem_posn[12][2] =
/*
 * Specifies the co-ords of complex elements in the example matrix
 * a f b e
 * b h c g
 * c j d i
 * d y a x
 * Used to setup vector elem_value which is 12 complex numbers
 * from the example matrix.
 */
{
	{0,0},	// a - 0
	{1,0},	// b - 1
	{2,0},	// c
	{3,0},	// d - 3
	{0,3},	// e
	{0,1},	// f - 5
	{1,3},	// g
	{1,1},	// h - 7
	{2,3},	// i
	{2,1},	// j - 9
	{3,3},	// x
	{3,1}	// y - 11
};

void classify_all_solutions(GList** AllSolutions, GList** SolutionLists) {
	
	/* Note: AllSolutions is already sorted by g_list_insert_sorted using
	 * compare_gsl_matrix_complex(const void* m1, const void* m2) as the
	 * comparison function. See toolbox.c
	 * Solution Lists is a GList of GLists.
	 */
	 
	GList *source = NULL;
	GList *dest   = NULL;
	GList *match  = NULL;
	gsl_matrix_complex *wspace    = gsl_matrix_complex_alloc(4,4);
	gsl_matrix_complex *eg_matrix = gsl_matrix_complex_alloc(4,4);
	
	// Sanity checks
	if(*AllSolutions == NULL) {
		printf("Classify_all_solutions got NULL pointer.\n");
		//exit(1);
		return;
	}
	if(*SolutionLists != NULL) {
		printf("Classify_all_solutions got non-empty SolutionLists.\n");
		//exit(1);
		return;
	}
	
	source = g_list_first(*AllSolutions);
				
	while(source != NULL) {
		*SolutionLists = g_list_prepend(*SolutionLists, NULL);
		dest = g_list_first(*SolutionLists);
		gsl_matrix_complex_memcpy(eg_matrix, source->data);
		int check_count = 0;
		for(int i = 0; i < 48; ++i) {
			// copy 16 values from eg_matrix to wspace using rotrefs and elem_posn
			for(int r = 0; r < 4; ++r) {
				for(int c = 0; c < 4; ++c) {	// wspace co-ords
					// determine source row & col
					int* elposn = (int*)&(elem_posn[rotrefs[i][r*4 + c]]);
					int sr = *elposn;
					int sc = *(elposn+1);
					// from ex_matrix to wspace
					gsl_complex gp = gsl_matrix_complex_get(eg_matrix, sr, sc);
					gsl_matrix_complex_set(wspace, r, c, gp);
				}
			}
			// search AllSolutions for a match to wspace
			match = g_list_find_custom(*AllSolutions, wspace, compare_gsl_matrix_complex);
			if(match == NULL) {
				printf("No match found in AllSolutions for wspace.\n\n");
				exit(1);
			} else {
				//printf("\rFound wspace in *AllSolutions. %d      ", check_count++);
			}
			
			/* unlink the matrix pointed to by match and append to dest
			 * list under SolutionLists
			 */
			 if(match != NULL) {
				*AllSolutions = g_list_remove_link(*AllSolutions, match);
				dest->data = g_list_concat(dest->data, match);
			 }	
		} // for i...
		source = *AllSolutions;
	} // while(source != NULL) 
	
	gsl_matrix_complex_free(wspace);
	gsl_matrix_complex_free(eg_matrix);		
}
//==============================================================================
void posn_independant_signature(gsl_matrix_complex *m, char *digest, int algo) {

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

#if(1)			
	// -----Debug print of assembled workspace
	printf("\nP_I_S Initial Workspace\n");
	for(int row = 0; row < 6; ++row) {
		for(int col = 0; col < 4; ++col) {
			gsl_complex *p = gsl_vector_complex_ptr(wspace, row*4 + col);
			printf("%2.0f,%2.0f ", GSL_REAL(*p), GSL_IMAG(*p));
		}
		printf("\n");
	}
	// -----------------------------------------------------------------
#endif
	
	// sort each row into ascending order
	for(int r = 0; r < 6; ++r) 		
		qsort(gsl_vector_complex_ptr(wspace, (r*4)), 4, sizeof(gsl_complex), compare_gsl_complex);
	
	// resort 6 rows into ascending order
	qsort(gsl_vector_complex_ptr(wspace,0), 6, (sizeof(gsl_complex) * 4), cmp4complex);
	
#if(1)
	printf("Fully Sorted Workspace\n");
	for(int row = 0; row < 6; ++row) {
		for(int col = 0; col < 4; ++col) {
			gsl_complex *p = gsl_vector_complex_ptr(wspace, row*4 + col);
			printf("%2.0f,%2.0f ", GSL_REAL(*p), GSL_IMAG(*p));
		}
		printf("\n");
	}
#endif

	// calc digest for matrix wspace
	const int dlen = gcry_md_get_algo_dlen(algo);
	unsigned char result[dlen];
	
	gcry_md_hash_buffer(algo, result, (const void *)(wspace->data), sizeof(gsl_complex)*24);
	
	// copy result back to digest
	memcpy(digest, result, dlen);

	gsl_vector_complex_free(wspace);
}
//======================================================================
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
//==============================================================================
void pi_sig_soln_data(gsl_matrix_complex *m, Solution_Data *digest, int algo) {
	
}

