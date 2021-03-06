/*
 * mpi_tocta_search.h
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
 
#ifndef __MPI_TOCTA_SEARCH_H
#define __MPI_TOCTA_SEARCH_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <ctype.h>
	#include <getopt.h>
	#include <glib.h>
	#include <gsl/gsl_vector.h>
	#include <gsl/gsl_complex.h>
	#include <gsl/gsl_complex_math.h>
	#include <gsl/gsl_matrix.h>
	
	#include <gcrypt.h>
	
	#include "../mpi_include/toolbox.h"	
	#include "../mpi_include/get_options.h"
	#include "../mpi_include/compact_equalsums.h"
	#include "../mpi_include/classify.h"
	
	typedef int Soln[4];
	
	int solution_test(gsl_matrix_complex** wspace, p_gvu* equalsums, gsl_complex* target);

	int cmp_solns(const void *left, const void *right);
	
	
#endif
