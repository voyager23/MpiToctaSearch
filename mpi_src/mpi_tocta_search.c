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


#include "../include/mpi_tocta_search.h"

#include "mpi.h"

int main(int argc, char* argv[])
{
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
		printf("Hello, world, I am %d of %d \t(%s)\n",rank, size, pname);
		fflush(stdout);
	} else { // Root Process
		printf("Hello, world, I am Root of %d \t(%s) ",size, pname);
		
		PRT_COMPLEX(target); NL;
		
		fflush(stdout);
		
	}
	
    MPI_Finalize();

    return 0;
}
