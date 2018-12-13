/*
 * search_all_solutions.c
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



#include <stdio.h>
#include <stdlib.h>

#include "../mpi_include/mpi_tocta_search.h"
#include "../mpi_include/toolbox.h"
#include "../mpi_include/search_all_solutions.h"

void search_all_solutions(GList** AllSolutions, p_gvu* equalsums, gsl_complex* target) {
	/* 
	 * 		a f b e
	 *		b h c g
	 * 		c j d i
	 * 		d y a x
	 * 
	 * General Strategy.
	 * 	One data structures
	 * 		GList AllSolutions - all solutions found by search
	 * 
	 * 	Standard 3 index search of equalsums
	 * 	Calculate last row of candidate solution
	 * 	Search equalsums for row3, col1 and col3
	 * 	if success, this is a soln
	 * 		Update AllSolutions by insertion sort
	 * 	clear working config as required
	 * 	next index
	 * 
	 * Note: AllSolutions and equalsums are pointers to pointers
	 * gsl_vector.size - number of elements
	 * ulong gsl_vector_ulong_get(const gsl_vector * v, const size_t i)
	 * 
	 * equalsums is gsl_vector_ulong. Entry->data can be cast as 
	 * gsl_vector_complex* which has 4 complex values which sum to Target
	 * 
	 */
	 
	// Sanity check
	if(*equalsums==NULL) {
		printf("search_all_solutions got NULL pointer\n");
		exit(1);
	}
	
	*AllSolutions = NULL;
	const int nsums = (*equalsums)->size;
	gsl_matrix_complex* wspace = gsl_matrix_complex_alloc(4,4);
	gsl_vector_complex* zero = gsl_vector_complex_calloc(4);
	int solutions = 0;
	
	printf("Search of equalsums - %lu lines / index\n", (*equalsums)->size);
	int triples = 0;
	for(int a = 0; a < nsums; ++a) {
		for(int b = 0; b < nsums; ++b) {
			if(b == a) continue;			
			gsl_matrix_complex_set_row(wspace, 0, (p_gvc)gsl_vector_ulong_get(*equalsums, a));
			gsl_matrix_complex_set_row(wspace, 1, (p_gvc)gsl_vector_ulong_get(*equalsums, b));

			if( COMPLEX_EQUAL(gsl_matrix_complex_get(wspace,0,2),
							  gsl_matrix_complex_get(wspace,1,0)) != 1) continue;
			if(count_pairs_by_row(&wspace,2) != 1) continue;
			// third index
			for(int c = 0; c < nsums; ++c) {
				if((c == a)||(c == b)) continue;
				gsl_matrix_complex_set_row(wspace, 2, (p_gvc)gsl_vector_ulong_get(*equalsums, c));
				if( COMPLEX_EQUAL(gsl_matrix_complex_get(wspace,1,2),
								  gsl_matrix_complex_get(wspace,2,0)) != 1) continue;
				if(count_pairs_by_row(&wspace,3) != 2) continue;
				// This triple is a candidate
				++triples;
				
				 if(solution_test(&wspace, equalsums, target) == 1) {
					 solutions += 1;
					 if(solutions % 100 == 0) {
						 printf("\t%d ", solutions);
						 if(solutions % 1000 == 0) printf("\n");
						 fflush(stdout);
					 }
					 gsl_matrix_complex* wspace_copy = gsl_matrix_complex_alloc(4,4);
					 gsl_matrix_complex_memcpy(wspace_copy, wspace);
					 *AllSolutions = g_list_insert_sorted(*AllSolutions, wspace_copy, compare_gsl_matrix_complex);

				 }
			} // for c...
		} // for b...
	} // for a...
	
	gsl_matrix_complex_free(wspace);
	gsl_vector_complex_free(zero);
	printf("\nDone - triples: %d	solutions: %d\n", triples, solutions);
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
