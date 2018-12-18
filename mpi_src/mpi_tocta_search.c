/*
 * mpi_tocta_search.c.c
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
 * 
 */

#include "mpi.h"
#include "../mpi_include/mpi_tocta_search.h"
#include "../libcrc/include/checksum.h"

int main(int argc, char* argv[])
{
	#define TAG_NDOUBLES 2
	#define TAG_DDATA 4
	#define TAG_NSOLNS 8
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
    
    if (rank != 0) { // =============== Work Process =============== //
		int result_code = 0;
		int complex_count = 0;
		int soln_count = 0;
		gsl_vector_complex* compact = NULL;
		gsl_vector_ulong* local_eqsums = NULL;
		gsl_complex local_target;
		
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
		// Reformat the compact vector into the qualsums format 
		// which is a gsl_vector_ulong of gsl_vector_complex.
		local_eqsums = gsl_vector_ulong_calloc(complex_count / 4);
		for(int i = 0; i < compact->size; i += 4) {
			gsl_vector_complex* p_gvc = gsl_vector_complex_calloc(4);
			double* dest = p_gvc->data;
			double* src  = (double*)gsl_vector_complex_ptr(compact, i);
			memcpy(dest, src, sizeof(double) * 4 * 2);
			gsl_vector_ulong_set(local_eqsums, (i/4), (ulong)p_gvc);
		}
		gsl_vector_complex_free(compact);
		
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
			for(int b = 0; b < nsums; ++b) {
				if(b == a) continue;			
				gsl_matrix_complex_set_row(wspace, 0, (p_gvc)gsl_vector_ulong_get(local_eqsums, a));
				gsl_matrix_complex_set_row(wspace, 1, (p_gvc)gsl_vector_ulong_get(local_eqsums, b));

				if( COMPLEX_EQUAL(gsl_matrix_complex_get(wspace,0,2),
								  gsl_matrix_complex_get(wspace,1,0)) != 1) continue;
				if(count_pairs_by_row(&wspace,2) != 1) continue;
				// third index
				for(int c = 0; c < nsums; ++c) {
					if((c == a)||(c == b)) continue;
					gsl_matrix_complex_set_row(wspace, 2, (p_gvc)gsl_vector_ulong_get(local_eqsums, c));
					if( COMPLEX_EQUAL(gsl_matrix_complex_get(wspace,1,2),
									  gsl_matrix_complex_get(wspace,2,0)) != 1) continue;
					if(count_pairs_by_row(&wspace,3) != 2) continue;
					++triples; // This triple is a candidate
					if(solution_test(&wspace, &local_eqsums, &local_target) == 1) {
					 solutions += 1;
					 if((quiet==0)&&(solutions % 100 == 0)) {
						 printf("\t%d ", solutions);
						 if(solutions % 1000 == 0) printf("\n");
						 fflush(stdout);
					 }
					 gsl_matrix_complex* wspace_copy = gsl_matrix_complex_alloc(4,4);
					 gsl_matrix_complex_memcpy(wspace_copy, wspace);
					}
				} // for c...
			} // for b...
			// if rank==1 output a progress report - a/nsums as %
			if(rank == 1) {
				double progress = ((double)a/(double)nsums) * 100.0;
				printf("\rRank 1: Progress %2.0f          ", progress); fflush(stdout);
			}
		} // for a...
		gsl_matrix_complex_free(wspace);
		gsl_vector_complex_free(zero);	
		
		if(quiet==0) printf("Process %d found %d triples and %d solutions.\n", rank, triples, solutions);
		
		MPI_Send(&solutions, 1, MPI_INT, ROOT, TAG_NSOLNS, MPI_COMM_WORLD);
				
	} else { //================ Root Process ===============//
		
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
		
		printf("\nFinal solution count for ");
		PRT_COMPLEX(target);
		printf(": %d\n", final_count);
		
		// Cleanup code
		free(part_solns);
						
	} // end root
	
	MPI_Finalize();

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
		gsl_matrix_complex_set_row(*wspace, 3, row3);
		gsl_vector_complex_free(row3);
		if(count_pairs_by_row(wspace,4) == 4) {
			return 1;
		}else{
			return 0;
		}
		
	} else {
		for(int i = 0; i < 4; ++i) gsl_matrix_complex_set(*wspace, 3, i, zero);
		gsl_vector_complex_free(row3);
		return 0;
	}
}

