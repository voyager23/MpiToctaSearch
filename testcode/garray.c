/*
 * garray.c
 * 
 * Copyright 2019 mike <mike@mike-XPS-Mint19>
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
 * examine the functioning of a g_array of type int Soln[4]
 * TODO:
 * 		Create a second shorter/longer array of dummy solutions;
 * 		Import the cmp_soln() function
 * 		qsort() the individual arrays
 * 		merge the two arrays
 */


#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <time.h>

typedef int Soln[4];
const int nsoln = 9;

void prt_soln(Soln *p);
void prt_soln(Soln *p) {
	for(int j = 0; j < 4; ++j) printf("%d ", (*p)[j]);
	printf("\n");
}

void prt_g_array(GArray *a);
void prt_g_array(GArray *a) {
	Soln *p = (Soln*)a->data;
	for(int i = 0; i < a->len; ++i) prt_soln(p++);	
}

int cmp_solns(const void *left, const void *right);
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

GArray* merge_soln_arrays(GArray *left, GArray *right);
GArray* merge_soln_arrays(GArray *left, GArray *right) {
	// Merge left and right pre-sorted g_arrays of Soln.
	// Return pointer to new merged array.
	// Input arrays are -not- erased
	int l = 0;
	int r = 0; // working indexes
	GArray *merged = g_array_sized_new(FALSE, FALSE, sizeof(Soln), (left->len + right->len));
	while( (l < left->len) && (r < right->len) ) {
		// get pointers to each solution
		Soln *lptr = (Soln*)g_array_index(left, Soln, l);
		Soln *rptr = (Soln*)g_array_index(right, Soln, r);
		if(cmp_solns(lptr, rptr) < 0) {
			g_array_append_val(merged, *lptr);
			++l;
		} else {
			g_array_append_val(merged, *rptr);
			++r;
		}
	}
	while(l < left->len)  g_array_append_val(merged, g_array_index(left, Soln, l++));
	while(r < right->len) g_array_append_val(merged, g_array_index(right,Soln, r++));
	
	return(merged);
}

//----------------------------------------------------------------------

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	GArray *solutions = g_array_new(FALSE, FALSE, sizeof(Soln));
	
	// Create and append nsoln count of Soln
	for(int i = 0; i < nsoln; ++i) {
		Soln buffer;
		for(int j = 0; j < 4; ++j) buffer[j] = rand()%100 + 1;
		g_array_append_val(solutions,buffer);
	}
	
	printf("\n");
#if(0)	
	printf("Using an int pointer for 1st line.\n");
	int *ip = (int*)solutions->data;
	for(int i = 0; i < nsoln*4; ++i) printf("%d ", *ip++);
#endif	
	// Create second shorter array
	GArray *second = g_array_new(FALSE, FALSE, sizeof(Soln));
	
	// Create and append (nsoln-2) count of Soln to second array
	for(int i = 0; i < nsoln-2; ++i) {
		Soln buffer;
		for(int j = 0; j < 4; ++j) buffer[j] = rand()%100 + 1;
		g_array_append_val(second,buffer);
	}
	qsort(solutions->data, nsoln, sizeof(Soln), cmp_solns);
	qsort(second->data, nsoln-2, sizeof(Soln), cmp_solns);
	
	printf("solutions->data: %p		solutions->len: %u\n", solutions->data, solutions->len);
	prt_g_array(solutions);
	printf("\n");

	printf("second->data: %p		second->len: %u\n", second->data, second->len);
	prt_g_array(second);
	printf("\n");
	
	GArray *merged = merge_soln_arrays(solutions,second);
	g_array_free(solutions,TRUE);
	g_array_free(second,TRUE);
	
	printf("merged->data: %p		merged->len: %u\n", merged->data, merged->len);
	prt_g_array(merged);
	printf("\n");
	
	printf("=====Done=====\n");
	g_array_free(merged, TRUE);
	return 0;
}

