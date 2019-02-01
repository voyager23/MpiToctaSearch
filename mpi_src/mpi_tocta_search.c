/*
 * mpi_tocta_search.c
 * 
 * Copyright 2018 mike <mike@jupiter>
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
 * NOTE: WHEN WORKING WITH G_ARRAY, THE LEN PARAMETER INDICATES THE NUMBER OF ITEMS NOT BYTES
 */

#include <mpi.h>
#include "../mpi_include/mpi_tocta_search.h"
#include "../libcrc/include/checksum.h"


int cmp_solns(const void *left, const void *right) {
	
	/* qsort()-style comparison function 
	 * 	returns less than zero for first arg is less than second arg, 
	 * 	zero for equal, 
	 * 	greater than zero if first arg is greater than second arg
	 */

	int *l = (int*)left;
	int *r = (int*)right;
	
	if(*l < *r) return -1;
	if(*l++ > *r++) return +1;

	if(*l < *r) return -1;
	if(*l++ > *r++) return +1;

	if(*l < *r) return -1;
	if(*l++ > *r++) return +1;
	
	if(*l < *r) return -1;
	if(*l > *r) return +1;
	
	return 0;
	
}


//----------------------------------------------------------------------
int solution_test(gsl_matrix_complex** wspace, p_gvu* equalsums, gsl_complex* target) {
	
	/*
	 * Calculate a set of values for row3.
	 * if row3 found in equalsums
	 * 		update wspace and check for exactly 4 pairs of values
	 * 		if 4 pairs found return 1 else return 0
	 * else
	 * 		clear row3 in wspace
	 * 		return 0
	 * endif
	 * 
	 * Modified to return index into equalsums if success
	 * 	esle return -1 on fail;
	 */
	
	gsl_complex zero = gsl_complex_rect(0.0,0.0);
	gsl_vector_complex* row3 = gsl_vector_complex_alloc(4);
	
	gsl_vector_complex_set(row3, 0, gsl_matrix_complex_get(*wspace,2,2));	// set d
	gsl_vector_complex_set(row3, 2, gsl_matrix_complex_get(*wspace,0,0));	// set a
	
	// calc and set value y = target - (f + h + j)
	gsl_complex y = *target;
	y = gsl_complex_sub(y, gsl_matrix_complex_get(*wspace, 0, 1));
	y = gsl_complex_sub(y, gsl_matrix_complex_get(*wspace, 1, 1));
	y = gsl_complex_sub(y, gsl_matrix_complex_get(*wspace, 2, 1));
	gsl_vector_complex_set(row3, 1, y);	// set y
	
	// calc and set value x = target - (e + g + i)
	gsl_complex x = *target;
	x = gsl_complex_sub(x, gsl_matrix_complex_get(*wspace, 0, 3));
	x = gsl_complex_sub(x, gsl_matrix_complex_get(*wspace, 1, 3));
	x = gsl_complex_sub(x, gsl_matrix_complex_get(*wspace, 2, 3));
	gsl_vector_complex_set(row3, 3, x);	// set x
	
	// search equalsums for the corresponding row
	
	void* found = bsearch(&row3, (*equalsums)->data, (*equalsums)->size, sizeof(ulong), cmp_gsv);
	
	if((found != NULL)) {

		int i;	// index into equalsums		
		// Using a non-NULL pointer attempt to recover the index into equalsums
		for(i = 0; i < (*equalsums)->size; ++i) {
			if(gsl_vector_ulong_ptr(*equalsums,i) == found) break;
		}
		
		gsl_matrix_complex_set_row(*wspace, 3, row3);
		gsl_vector_complex_free(row3);
		if(count_pairs_by_row(wspace,4) == 4) {
			return i;
		}else{
			return -1;
		}
		
	} else {
		for(int i = 0; i < 4; ++i) gsl_matrix_complex_set(*wspace, 3, i, zero);
		gsl_vector_complex_free(row3);
		return -1;
	}
}

//==============================================================================
int main(int argc, char* argv[])
{
	#define TAG_NDOUBLES 2
	#define TAG_DDATA 4
	#define TAG_NSOLNS 8
	#define TAG_SOLUTIONS 16
	#define ROOT 0
	
    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];
    char pname[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    
    gsl_complex target;
    int quiet = 0;
    int list  = 0;
 
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_library_version(version, &len);
    MPI_Get_processor_name(pname, &len);
    
    MPI_Datatype MPI_SOLN;
    MPI_Type_contiguous(4, MPI_INT, &MPI_SOLN);
    MPI_Type_commit(&MPI_SOLN);
        
    if (rank != 0) { // =============== Work Process =============== //
		int result_code = 0;
		int complex_count = 0;
		int soln_count = 0;
		gsl_vector_complex* compact = NULL;
		gsl_vector_ulong* local_eqsums = NULL;
		gsl_complex local_target;
		
		GArray *soln_array = NULL;
		soln_array = g_array_sized_new(FALSE, TRUE, sizeof(Soln), 1024);
		Soln new_soln;
		
		// Receive the number of complex numbers
		result_code = MPI_Bcast(&complex_count, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		if(result_code != MPI_SUCCESS) printf("\nWork process %d reports Bcast error.\n", rank);

		// Receive the complex numbers
		compact = gsl_vector_complex_calloc(complex_count);
		result_code = MPI_Bcast(compact->data, complex_count, MPI_C_DOUBLE_COMPLEX, ROOT, MPI_COMM_WORLD);
		
		// Receive the local_target value
		result_code = MPI_Bcast(&local_target, 1, MPI_C_DOUBLE_COMPLEX, ROOT, MPI_COMM_WORLD);
		
		// Receive the quiet flag
		result_code = MPI_Bcast(&quiet, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		if(quiet==0)		
			printf("Process %d received %d as complex_count.\n", rank, complex_count); fflush(stdout);		
		
		// Create a checksum
		uint32_t chksum = crc_32((const unsigned char *)compact->data, (compact->size)*16);
		if(quiet==0) printf("BCast received @ (%d) - Checksum %0X\n", rank, chksum);
		if(quiet==0) printf("local_target: "); //PRT_COMPLEX(local_target); NL;
		
		// Reformat the compact vector into the equalsums format 
		// which is a gsl_vector_ulong of gsl_vector_complex.
		
		local_eqsums = gsl_vector_ulong_calloc(complex_count / 4);
		for(int i = 0; i < compact->size; i += 4) {
			gsl_vector_complex* gvc_ptr = gsl_vector_complex_calloc(4);
			double* dest = gvc_ptr->data;
			double* src  = (double*)gsl_vector_complex_ptr(compact, i);
			memcpy(dest, src, sizeof(double) * 4 * 2);
			gsl_vector_ulong_set(local_eqsums, (i/4), (ulong)gvc_ptr);
		}
		gsl_vector_complex_free(compact);
		
		// DEBUG Output the local-equal sums
		printf("Debug output of local_eqsums.\n");
		for(int i = 0; i < local_eqsums->size; ++i) {
			gsl_vector_complex* gvc_ptr = (gsl_vector_complex*)gsl_vector_ulong_get(local_eqsums, i);
			for(int j = 0; j < gvc_ptr->size; ++j) PRT_COMPLEX(gsl_vector_complex_get(gvc_ptr,j));
			NL;
		}
		NL;
		printf("--------------------\n");
		
		// Solution subset search
		// establish the range of indexes used for the 'a' pointer.
		// first = rank-1; limit is local_eqsums->size; stride is world_size-1
				
		const int nsums = (local_eqsums)->size;
		gsl_matrix_complex* wspace = gsl_matrix_complex_alloc(4,4);
		gsl_vector_complex* zero = gsl_vector_complex_calloc(4);
		int solutions = 0;
		
		if(quiet==0) printf("Search of equalsums - %lu lines\n", (local_eqsums)->size);
		int triples = 0;		
		//for(int a = 0; a < nsums; ++a) {
		for(int a = (rank-1); a < nsums; a+=(size-1)) {
			new_soln[0] = a;
			for(int b = 0; b < nsums; ++b) {
				if(b == a) continue;
				new_soln[1] = b;			
				gsl_matrix_complex_set_row(wspace, 0, (p_gvc)gsl_vector_ulong_get(local_eqsums, a));
				gsl_matrix_complex_set_row(wspace, 1, (p_gvc)gsl_vector_ulong_get(local_eqsums, b));

				if( COMPLEX_EQUAL(gsl_matrix_complex_get(wspace,0,2),
								  gsl_matrix_complex_get(wspace,1,0)) != 1) continue;
				if(count_pairs_by_row(&wspace,2) != 1) continue;
				// third index
				for(int c = 0; c < nsums; ++c) {
					if((c == a)||(c == b)) continue;
					new_soln[2] = c;
					gsl_matrix_complex_set_row(wspace, 2, (p_gvc)gsl_vector_ulong_get(local_eqsums, c));
					if( COMPLEX_EQUAL(gsl_matrix_complex_get(wspace,1,2),
									  gsl_matrix_complex_get(wspace,2,0)) != 1) continue;
					if(count_pairs_by_row(&wspace,3) != 2) continue;
					++triples; // This triple is a candidate
					
					// Reuse the result_code variable
					result_code = solution_test(&wspace, &local_eqsums, &local_target);

					if(result_code >= 0 ) { // result_code has index
						solutions += 1;
						if((quiet==0)&&(solutions % 100 == 0)) {
						 printf("\t%d ", solutions);
						 if(solutions % 1000 == 0) printf("\n");
						 fflush(stdout);
						}
						//gsl_matrix_complex* wspace_copy = gsl_matrix_complex_alloc(4,4);
						//gsl_matrix_complex_memcpy(wspace_copy, wspace);
						// Add solution to g_array
						new_soln[3] = result_code;
						g_array_append_val(soln_array, new_soln);
						
						//Debug output of solution matrix using indexes in new_soln[4]
						//
						//------------------------------------------------------------
						
					}
					
				} // for c...
			} // for b...

		} // for a...
		
		
#if(1)
			// if rank==1 output data about the soln_array
			if(rank != 0) {
				printf("\n=====Process %d soln_array=====\n", rank);
				for(int i = 0; i < soln_array->len; ++i) {
					Soln *temp = (Soln*)g_array_index(soln_array, Soln, i);
					for(int j = 0; j < 4; ++j) printf("%d ", *(((int*)temp)+j) );
					printf("\n");
				}
				printf("\n===============================\n");
			}
		
#endif					
		
		gsl_matrix_complex_free(wspace);
		gsl_vector_complex_free(zero);	
		
		if(quiet==0) printf("Process %d found %d triples and %d solutions.\n", rank, triples, solutions);
		if(quiet==0) printf("soln_array has %d items.\n", soln_array->len);
		
		MPI_Send(&solutions, 1, MPI_INT, ROOT, TAG_NSOLNS, MPI_COMM_WORLD);
		
		MPI_Send(soln_array->data, solutions, MPI_SOLN, ROOT, TAG_SOLUTIONS, MPI_COMM_WORLD);
	
		
		g_array_free(soln_array,TRUE);
		
	// end work process
				
	} else { 
		
	//======================== Root Process ==========================//	
		int result_code;
		gsl_vector_complex *compact = NULL;
		
		// printf("Hello, world, I am Root of %d \t(%s) ",size, pname);
		get_options(argc, argv, &target, &quiet, &list);
		//PRT_COMPLEX(target); NL;
		fflush(stdout);
		
		// Prepare a contiguous vector of complex numbers (compact) using equalsums_database.bin
		// Each group of 4 represents an equalsum of target value.
		compact_equalsums("../data/equalsums_database.bin", &compact, &target);			
		
		// Broadcast number of doubles in main message
		if(MPI_SUCCESS != MPI_Bcast(&(compact->size), 1, MPI_INT, ROOT, MPI_COMM_WORLD)) 
			printf("\nWork process %d reports Bcast error.\n", rank);
		
		// Broadcast the doubles
		MPI_Bcast(compact->data, compact->size, MPI_C_DOUBLE_COMPLEX, ROOT, MPI_COMM_WORLD);
		if(quiet==0) printf("Bcast completed.\n");
		
		// Broadcast the target value
		MPI_Bcast(&target, 1, MPI_C_DOUBLE_COMPLEX, ROOT, MPI_COMM_WORLD);
		
		// Broadcast the quiet flag
		result_code = MPI_Bcast(&quiet, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
				
		// Create a checksum
		uint32_t chksum = crc_32((const unsigned char *)compact->data, (compact->size)*16);
		if(quiet==0) printf("Root Checksum: %0X\n", chksum);		
				
		// Receive the partial solution counts
		int *part_solns = malloc(sizeof(int) * (size -1));
		for(int proc = 1; proc < size; ++proc) {
			MPI_Recv(part_solns + proc - 1, 1, MPI_INT, proc, TAG_NSOLNS, MPI_COMM_WORLD, &status);
		}
			
		int final_count = 0;
		for(int proc = 1; proc < size; ++proc) final_count += part_solns[proc - 1];
		
		printf("Final solution count for ");
		PRT_COMPLEX(target);
		printf(": %d\n", final_count);
		
		// Allocate space for solutions then receive index groups from worker nodes.
		
		int *final_solutions = malloc(sizeof(int)*4*final_count);
		int *next = final_solutions;
		for(int proc = 1; proc < size; ++proc) {
			MPI_Recv(next, part_solns[proc-1], MPI_SOLN, proc, TAG_SOLUTIONS, MPI_COMM_WORLD, &status);
			next += (part_solns[proc-1]*4);
		}
		
		qsort(final_solutions, final_count, sizeof(Soln), cmp_solns);
		
		printf("Indexes of all solutions.\n");
		for(int i = 0; i < final_count; ++i) {
			for(int j = 0; j < 4; ++j) printf("%d ", final_solutions[(4*i)+j]);
			printf("\n");
		}
		
		
		printf("Final solution count for ");
		PRT_COMPLEX(target);
		printf(": %d\n", final_count);
	
		
		/*
		 * Code to classify solutions into groups[48]
		 * Data: compact, final_solutions, final_count
		 * 		gsl_vector_complex *compact
		 * 		int final_count = 0;
		 * 		int *final_solutions = malloc(sizeof(int)*4*final_count);
		 * -------------------------------------------------------------------------
		 * Allocate gsl_vector_ulong(final_count) {digest_ptrs}
		 * 		gsl_vector_ulong *digest_ptrs = gsl_vector_ulong_calloc(final_count);
		 * Allocate gsl_matrix-complex as workspace
		 * 		gsl_matrix_complex *wsp = gsl_matrix_complex_alloc(4,4);
		 * for each line of final_solutions
		 * 		allocate memory for digest and save pointer in gsl_vector_ulong
		 * 		get 4 indexes
		 * 		update the workspace
		 * 		call posn_independant_signature(workspace, *digest)	{see gcrypt2.c}
		 * 
		 * for each entry in digest_ptrs
		 * 		print the signature
		 * -------------------------------------------------------------------------
		 */
		 
		gsl_vector_ulong *digest_ptrs = gsl_vector_ulong_calloc(final_count);
		gsl_matrix_complex *wsp = gsl_matrix_complex_alloc(4,4);
		
		for(int i = 0; i < final_count; ++i) {
			// Construct the workspace matrix
			for(int row = 0; row < 4; ++row) {	// row index
				int idx = final_solutions[(4*i)+row];
				// idx references a group of 4 complex values in vector compact
				// extract and save 4 complex values to workspace row j
				NL;
				for(int col = 0; col < 4; ++col) {
					gsl_complex foo = gsl_vector_complex_get(compact, idx+col);
					printf("i:%d   row:%d   idx:%d   col:%d   real:%2.0f   imag:%2.0f\n", i, row, idx, col, GSL_REAL(foo), GSL_IMAG(foo));	
					gsl_matrix_complex_set(wsp, row, col, gsl_vector_complex_get(compact, idx+col));
				}
				// Allocate digest
				gsl_vector_ulong_set(digest_ptrs, i, (ulong)(malloc(sizeof(char)*20)));
				// Calculate the signature
				posn_independant_signature(wsp, (char*)gsl_vector_ulong_get(digest_ptrs, i));
			}
		}
		
		// TODO: qsort the digests
		// Each digest is dynamically allocated so assume non-contiguous
		// Copy digests to a contiguous array for qsort function
		
		char *qsort_array = malloc(sizeof(char)*20*final_count);
		char *dest = qsort_array;
		for(int j = 0; j < digest_ptrs->size; ++j) {
			strncpy(dest, (char*)*gsl_vector_ulong_ptr(digest_ptrs, j), 20);
			dest += 20;
		}
		qsort(qsort_array, final_count, 20, compare_digests);
			
		
		for(int j = 0; j < final_count; ++j) {		
			for(int i = 0; i < 20; ++i) printf("%02x ", *(qsort_array + (j*20 + i))&0x00ff);
			printf("\n");
		}
		// Cleanup code
		free(qsort_array);
		free(final_solutions);
		free(part_solns);
						
	} // end root process

	// Cleanup MPI
    MPI_Type_free(&MPI_SOLN);
	MPI_Finalize();

}
