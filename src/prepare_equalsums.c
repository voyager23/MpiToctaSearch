/*
 * prepare_equalsums.c
 * 
 * Copyright 2018 mike <mike@mike-XPS-Mint19>
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

#include "../include/prepare_equalsums.h"

void prepare_equalsums(char *fname, gsl_vector_ulong **equalsums, gsl_complex *target) {
	// fname is the input file - (produced by program 'create_database')
	// Target value has been set by input function
	
	FILE *fin;
	// double real, imag;
	int result;
	
	fin = fopen(fname,"rb");
	if(fin == NULL) {
	 printf("Could not open %s for reading.\n", fname);
	 exit(1);
	}
	
	gsl_complex request = *target;
	// scan for target value
	// The database is assumed to have been written with
	// gsl_vector_complex_fwrite(). Each vector has 5 complex values
	// in format total, a, b, c, d.
	gsl_vector_complex *buffer = gsl_vector_complex_calloc(5); // input vector

	gsl_error_handler_t *old_handler = gsl_set_error_handler_off();
	
	// Do an intial scan to establish the match_count
	int match_count = 0;
	while(gsl_vector_complex_fread(fin,buffer) != GSL_EFAILED) { // Pass 1
		int compare = compare_gsl_complex(gsl_vector_complex_ptr(buffer, 0), &request);
		if(compare < 0) continue;
		if(compare > 0) break;
		match_count+=1;
	} // Pass 1
	
	// Allocate the equalsums gsl_vector_ulong
	*equalsums = gsl_vector_ulong_calloc(match_count*24);
	int idx = 0;	// Indicates the position to store data in vector
	rewind(fin);	
	gsl_permutation *vector_perm = gsl_permutation_alloc(4);
	while(gsl_vector_complex_fread(fin,buffer) != GSL_EFAILED) { // Pass 2
		int compare = compare_gsl_complex(gsl_vector_complex_ptr(buffer, 0), &request);
		if(compare < 0) continue;
		if(compare > 0) break;
		// matching entry
		// Add 24 permuted vector_complex(4) to equalsums
		gsl_permutation_init(vector_perm);	// Identity permutation
		//---------------Permutations----------------------
		do {
			gsl_vector_complex *copy = gsl_vector_complex_alloc(4);
			// copy buffer vector to copy vector
			for(int s = 1, d = 0; s < 5; ++s,++d) gsl_vector_complex_set(copy, d, gsl_vector_complex_get(buffer, s)); 
			gsl_permute_vector_complex((const gsl_permutation *)vector_perm, copy);
			//for(int x = 0; x < 4; ++x) PRT_COMPLEX(gsl_vector_complex_get(copy,x));
			//NL;
			// Update equalsums
			gsl_vector_ulong_set(*equalsums, idx++, (uintptr_t)copy);
		} while(gsl_permutation_next(vector_perm) == GSL_SUCCESS);
		// Keep target value updated
		*target = (gsl_vector_complex_get(buffer,0));		
	} // Pass 2
	
	gsl_set_error_handler(old_handler);	// Restore error handler
	printf("Match_count: %d\n", match_count);
	
	// ---------- Sort equalsums----------  //
	printf("Sorting equalsums...");
	qsort((*equalsums)->data, match_count*24, sizeof(uintptr_t), cmp_gsv);
	printf("complete.\n");
	
	// Cleanup code as required
	gsl_vector_complex_free(buffer);
	gsl_permutation_free(vector_perm);
}

