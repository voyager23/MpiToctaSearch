/*
 * toolbox.c
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
 */

#include "../mpi_include/toolbox.h"

//====================Comparison Function====================
int cmp_gsv(const void* p1, const void* p2) {
	
    int result;
    p_gvc pv1 = (p_gvc)(*(long*)p1);   
    p_gvc pv2 = (p_gvc)(*(long*)p2);

    const gsl_complex *lc, *rc;
    for(int i = 0; i < pv1->size; ++i) {		
		lc = gsl_vector_complex_const_ptr(pv1,i);
		rc = gsl_vector_complex_const_ptr(pv2,i);
		result = compare_gsl_complex(lc, rc);
		if(result != 0) return result;
	}
	return 0;
}

//=============================================================
int compare_gsl_complex(const void* plc, const void* prc) {
	// Use the lexicographic order, comparing the real parts and then
	// comparing the imaginary ones if the real parts are equal.
	// If l < r return -1, if l > r return +1 else return 0;
	
	p_cpx lc = (p_cpx)plc;
	p_cpx rc = (p_cpx)prc;
	
	if(GSL_REAL(*lc) < GSL_REAL(*rc)) return -1;
	if(GSL_REAL(*lc) > GSL_REAL(*rc)) return +1;
	// real values equal
	
	if(GSL_IMAG(*lc) < GSL_IMAG(*rc)) return -1;
	if(GSL_IMAG(*lc) > GSL_IMAG(*rc)) return +1;
	// imag values equal 
	
	// complexes are equal
	return(0);
}

//===============================================================

int compare_gsl_matrix_complex(const void* m1, const void* m2) {
	// Compare 2 equal-size matrixes row by row.Using memcmp we can 'rank'
	// the matrixes in a consistent way, using the return value for qsort
	// and bsearch
	// Return value <0: m1 < m2		>0: m1 > m2		0: m1 == m2
	gsl_matrix_complex* p1 = (gsl_matrix_complex*)m1;
	gsl_matrix_complex* p2 = (gsl_matrix_complex*)m2;
	int result = 0;
	for(int row = 0; row < 4; ++row) {
		result = memcmp(gsl_matrix_complex_ptr(p1,row,0),gsl_matrix_complex_ptr(p2,row,0),sizeof(gsl_complex)*4);
		if(result != 0) return result;
	}
	return 0;
}

//===============================================================
int count_pairs_by_row(gsl_matrix_complex** wspace, int nrows) {
	// Modified to consider all columns

	int pair_count = 0;
	// Sanity check
	if((nrows < 2)||(nrows > 4)||(*wspace == NULL)) {
		printf("Data error: count_pairs_by_row\n");
		return 0;
	}

	// This function is faster
	int r0, c0, r1, c1;
	for(int index0 = 0; index0 < nrows*4; ++index0) {
		r0 = index0/4;
		c0 = index0%4;
		for(int index1 = index0 + 1; index1 < nrows*4; ++index1) {
			r1 = (index1)/4;
			c1 = (index1)%4;
			//printf("(%d,%d) (%d,%d)\n", r0, c0, r1, c1);
			if((r1 < nrows)&&(COMPLEX_EQUAL
					(gsl_matrix_complex_get(*wspace, r0, c0),
					 gsl_matrix_complex_get(*wspace, r1, c1)))) pair_count += 1;
		}
	}
	return pair_count;
}
//===============================================================
int compare_digests(const void *l, const void *r) {
	/* Used by qsort.
	 * compare two 20/32 byte digests
	 * if l<r return -1
	 * if l>r return +1
	 * else return 0
	 * l and r are interpreted as pointers to char*
	 */
	 const void *lptr = (const char*)l;
	 const void *rptr = (const char*)r;
	return memcmp(lptr, rptr, 32);
}
//================================================================
