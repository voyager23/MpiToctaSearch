/*
 * toolbox.h
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
 
#include <gsl/gsl_matrix.h>
#include <string.h>

#ifndef __TOOLBOX_H__
	#define __TOOLBOX_H__
	
		typedef unsigned long 		ulong;
	typedef gsl_vector_complex* p_gvc;
	typedef gsl_vector_ulong*   p_gvu;
	typedef gsl_complex*        p_cpx;


	#define PRT_COMPLEX(c) printf("(%2.0f,%2.0f) ", GSL_REAL(c), GSL_IMAG(c));
	#define NL { printf("\n"); }
	// this define returns a p_gvc from a gsl_vector_ulong object
	#define GVC_PTR(gvu, index) ((p_gvc)(*(gvu->data + index)))
	#define COMPLEX_EQUAL(a,b) ((GSL_REAL(a)==GSL_REAL(b))&&(GSL_IMAG(a)==GSL_IMAG(b) ? 1 : 0))
	
	// Declarations
	int cmp_gsv(const void* p1, const void* p2);
	int compare_gsl_complex(const void* plc, const void* prc);
	// Compare 2 equal matrixes row by row.Using memcmp we can 'rank'
	// the matrixes in a consistent way, using the return value for qsort
	// and bsearch
	// Return value <0: m1 < m2		>0: m1 > m2		0: m1 == m2
	int compare_gsl_matrix_complex(const void* m1, const void* m2);
	int count_pairs_by_row(gsl_matrix_complex** wspace, int rows);

#endif

