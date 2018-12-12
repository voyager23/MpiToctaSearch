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


#include "../mpi_include/mpi_tocta_search.h"

#include "mpi.h"

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
		
		int result_code = 0;
		MPI_Status status;
		int double_count = 0;
		gsl_vector_complex* compact = NULL;
		
		result_code = MPI_Bcast(&double_count, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		if(result_code != MPI_SUCCESS) printf("\nWork process %d reports Bcast error.\n", rank);		
		printf("Process %d received %d as double_count.\n", rank, double_count); fflush(stdout);
		compact = gsl_vector_complex_calloc(double_count / 2);
		MPI_Bcast(compact->data, double_count, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
		printf("BCast received.\n");
		
	} else {		
		// Root Process
		
		int result_code;
		gsl_vector_complex *compact = NULL;
		int nDoubles = 0;
		
		// printf("Hello, world, I am Root of %d \t(%s) ",size, pname);
		get_options(argc, argv, &target, &quiet, &list);
		PRT_COMPLEX(target); NL;
		fflush(stdout);
		// Prepare a contiguous vector of complex numbers (compact) using equalsums_database.bin
		// Each group of 4 represents an equalsum of target value.
		// ??Create a checksum??
		
		compact_equalsums("../data/equalsums_database.bin", &compact, &target);
		nDoubles = (compact->size) * 2;			
		// Broadcast number of doubles in main message
		if(MPI_SUCCESS != MPI_Bcast(&nDoubles, 1, MPI_INT, ROOT, MPI_COMM_WORLD)) 
			printf("\nWork process %d reports Bcast error.\n", rank);
		// Broadcast the doubles
		MPI_Bcast(compact->data, nDoubles, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
		printf("Bcast completed.\n");
						
	}
	
    MPI_Finalize();

    return 0;
}
