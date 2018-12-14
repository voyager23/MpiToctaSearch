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
	#define ROOT 0
	
    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];
    char pname[MPI_MAX_PROCESSOR_NAME];
    
    gsl_complex target;
    int quiet = 0;
    int list  = 0;
 
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_library_version(version, &len);
    MPI_Get_processor_name(pname, &len);
    
    if (rank != 0) {		
		// Work Process
		MPI_Status status;		
		int result_code = 0;
		int complex_count = 0;
		int soln_count = 0;
		gsl_vector_complex* compact = NULL;
		gsl_vector_ulong* local_eqsums = NULL;
		
		result_code = MPI_Bcast(&complex_count, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		if(result_code != MPI_SUCCESS) printf("\nWork process %d reports Bcast error.\n", rank);		
		printf("Process %d received %d as complex_count.\n", rank, complex_count); fflush(stdout);
		compact = gsl_vector_complex_calloc(complex_count);
		MPI_Bcast(compact->data, complex_count, MPI_C_DOUBLE_COMPLEX, ROOT, MPI_COMM_WORLD);
		
		// Create a checksum
		uint32_t chksum = crc_32((const unsigned char *)compact->data, (compact->size)*16);
		printf("BCast received @ (%d) - Checksum %0X\n", rank, chksum);
		
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
		// establish the range of indexes used for the 'a' pointer.
		// first = rank-1; limit is local_eqsums->size; stride is world_size-1
		
		soln_count = mpi_subset_search(&local_eqsums, &target);
		
	} else {		
		// Root Process
		
		int result_code;
		gsl_vector_complex *compact = NULL;
		
		// printf("Hello, world, I am Root of %d \t(%s) ",size, pname);
		get_options(argc, argv, &target, &quiet, &list);
		PRT_COMPLEX(target); NL;
		fflush(stdout);
		// Prepare a contiguous vector of complex numbers (compact) using equalsums_database.bin
		// Each group of 4 represents an equalsum of target value.
		
		compact_equalsums("../data/equalsums_database.bin", &compact, &target);			
		// Broadcast number of doubles in main message
		if(MPI_SUCCESS != MPI_Bcast(&(compact->size), 1, MPI_INT, ROOT, MPI_COMM_WORLD)) 
			printf("\nWork process %d reports Bcast error.\n", rank);
		// Broadcast the doubles
		MPI_Bcast(compact->data, compact->size, MPI_C_DOUBLE_COMPLEX, ROOT, MPI_COMM_WORLD);
		printf("Bcast completed.\n");
		// Create a checksum
		uint32_t chksum = crc_32((const unsigned char *)compact->data, (compact->size)*16);
		printf("Root Checksum: %0X\n", chksum);
						
	}
	
    MPI_Finalize();

    return 0;
}
