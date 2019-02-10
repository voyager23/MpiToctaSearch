/*
 * classify.h
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


#ifndef __CLASSIFY_H__
	#define __CLASSIFY_H__
	
	#include <stdio.h>
	#include <glib.h>
	#include <gcrypt.h>
	
	#include "../include/toolbox.h"
	#include "../include/mpi_tocta_search.h"
	
	typedef struct tag {
		char pisig[32];					// good for sha128/sha256
		gsl_matrix_complex *solution;	// Must be allocated prior to use
		gsl_complex target;				// GSL_SET_COMPLEX(&target, x, y)
		unsigned nGroups, index;		
	} Solution_Data;

	void classify_all_solutions(GList** AllSolutions, GList** SolutionLists);	
	void posn_independant_signature(gsl_matrix_complex *m, char *digest, int algo);
	int cmp4complex(const void *left, const void *right);
	void pi_sig_soln_data(gsl_matrix_complex *m, Solution_Data *digest, int algo);

#endif
