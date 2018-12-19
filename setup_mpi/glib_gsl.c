/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2006      Cisco Systems, Inc.  All rights reserved.
 *
 * Additional checks for the glib-2.0 and gsl libraries
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_vector.h>
#include <glib.h>

#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];
    char pname[MPI_MAX_PROCESSOR_NAME];
    

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_library_version(version, &len);
    MPI_Get_processor_name(pname, &len);
    
    if (rank != 0) {
		
		// -----Test Block----- //
		gsl_complex gc = gsl_complex_rect(1.2,3.4);
		gsl_vector_complex* p_gvc = gsl_vector_complex_alloc(256);
		gsl_vector_complex_free(p_gvc);
		//---------------------//
		
		printf("Glib2 & Gsl test - I am %d of %d \t(%s)\n",rank, size, pname);
		fflush(stdout);
		
	} else {
		
		// -----Test Block----- //
		gsl_complex gc = gsl_complex_rect(1.2,3.4);
		gsl_vector_complex* p_gvc = gsl_vector_complex_alloc(256);
		gsl_vector_complex_free(p_gvc);
		//---------------------//
				
		printf("Glib2 & Gsl test - I am Root of %d \t(%s)\n",size, pname);
		fflush(stdout);
	}
	
    MPI_Finalize();

    return 0;
}
