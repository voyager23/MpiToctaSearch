/*
 * compact_equalsums.c
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

#include "../mpi_include/compact_equalsums.h"

void compact_equalsums(char *fname, gsl_vector_complex **compact, gsl_complex *target) {
	
	// fname is the input file - (produced by program 'create_database')
	// Target value has been set by input function
	
	FILE *fin;
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
	gsl_vector_ulong *local_eqsums = gsl_vector_ulong_calloc(match_count*24);
	
	int idx = 0;			// Position to store data in vector of pointers
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
			gsl_vector_complex *working = gsl_vector_complex_alloc(4);
			// copy buffer vector to working vector
			for(int s = 1, d = 0; s < 5; ++s,++d) gsl_vector_complex_set(working, d, gsl_vector_complex_get(buffer, s)); 
			gsl_permute_vector_complex((const gsl_permutation *)vector_perm, working);
			//for(int x = 0; x < 4; ++x) PRT_COMPLEX(gsl_vector_complex_get(copy,x));
			//NL;
			// Update equalsums
			gsl_vector_ulong_set(local_eqsums, idx, (uintptr_t)working);			
			// Increment index
			idx += 1;			
		} while(gsl_permutation_next(vector_perm) == GSL_SUCCESS);
		
		// Keep target value updated
		*target = (gsl_vector_complex_get(buffer,0));
				
	} // Pass 2
	
	gsl_set_error_handler(old_handler);	// Restore error handler
	printf("Match_count: %d\n", match_count);
	
	// ---------- Sort equalsums----------  //
	printf("Sorting local_eqsums...%d items ", match_count*24);
	qsort(local_eqsums->data, match_count*24, sizeof(uintptr_t), cmp_gsv);
	printf("complete.\n");
	
	// Write the contiguous version of equalsums for use by MPI Broadcast
	*compact = gsl_vector_complex_calloc(match_count*24*4);
	double *src = NULL;
	double *dst = NULL;
	for(int i = 0; i < local_eqsums->size; ++i) {
		gsl_vector_complex *p_gvc = (gsl_vector_complex*)gsl_vector_ulong_get(local_eqsums,i);
		src = p_gvc->data;
		dst = (double*)gsl_vector_complex_ptr(*compact, i*4);
		memcpy(dst, src, sizeof(double)*2*4);
	}

#if(0)
		// debug print of complex for confirmation
		for(int i = 0; i < (*compact)->size; i += 4) {
			printf("%03d)\t", i);
			for(int j = 0; j < 4; ++j) PRT_COMPLEX(gsl_vector_complex_get(*compact,(i+j)));
			NL;
		}
#endif

		printf("Compact->size: %ld double_count: %ld\n", (*compact)->size, 2*(*compact)->size);


	// ---------------Cleanup code as required--------------- //
	gsl_vector_complex_free(buffer);
	gsl_permutation_free(vector_perm);
	// free local_equalsums which is a vector of pointers to vector_complex(4);
	for(int i = 0; i < local_eqsums->size; ++i) {
		gsl_vector_complex *p_gvc = (gsl_vector_complex*)gsl_vector_ulong_get(local_eqsums,i);
		gsl_vector_complex_free(p_gvc);
	}
	gsl_vector_ulong_free(local_eqsums);
	// ---------------Done---------------- //
}

