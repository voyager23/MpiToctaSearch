/*
 * create_database.c
 * 
 * Copyright 2018 mike <mike@mike-XPS-Mint19>
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
 * Comments: Stand alone program based on testcode/combo.c
 * 		Creates a large consolidated database of equalsums.
 * 		Based on an internal limit of (30).
 * 		The database is sorted by the total for each line.
 * 		Blocks of less than 6 equal totals have been removed.
 * 		Output is a file - ../data/equalsums_database.bin
 */

#include "../include/create_database.h"

//=====================================================================
void delete_block(GList **list, GList **first, GList **sentinel) {
	GList *llink;	
	//printf("delete_block called...");
	while(*first != *sentinel) {
		llink = *first;
		*first = (*first)->next;
		*list = g_list_remove_link (*list, llink);
		// free_some_data_that_may_access_the_list_again (llink->data);
		g_list_free (llink);
	}
	//printf("completed.\n");
}

//======================================================================
int wrap_compare_gsl_complex(const void* left, const void* right) {
	
	gsl_vector_complex* p_lv = (gsl_vector_complex*)left;
	gsl_vector_complex* p_rv = (gsl_vector_complex*)right;
	// Compare Total values
	gsl_complex* p_lc = gsl_vector_complex_ptr(p_lv,0);
	gsl_complex* p_rc = gsl_vector_complex_ptr(p_rv,0);
	
	return (compare_gsl_complex(p_lc, p_rc));
}


//======================================================================
_Bool IsPrime(unsigned long number)
{
    if(number<2)
        return false;
    if(number==2)
        return true;
    if(number%2==0)
        return false;
    for(int i=3;i<=sqrt(number);i += 2)
    {
        if(number%i==0)
            return false;
    }
    return true;
}

//======================================================================
int main(int argc, char **argv)
{
	/* create a list of gaussian complex primes using a limit value
	* 		(Glist of gsl_complex_double)
	* create a gsl_combination - all combinations of 4 from nprimes
	* create the eqsums list using the combinations
	* sort list into blocks using total
	* 
	* 		TODO: remove blocks with less than 6 entries
	* 
	* save to file as blocks of equal total ( gsl_complex_fwrite )
	*/

	// const char* fname = "./eqsums.bin";
	FILE* fout = NULL;

	const unsigned long limit = 26.0;
	GList* gprimes = NULL;
	GList* eqsums = NULL;
	
	// ----------Creating a list of Gaussian Primes----------
	for(unsigned long a = 1; a < limit; ++a) {
		for(unsigned long b = 1; b < limit; ++b) {
		 int checksum = ((a*a) + (b*b));
		 if(IsPrime((unsigned long)checksum)) {
			 gsl_complex* new = malloc(sizeof(gsl_complex));
			 GSL_SET_COMPLEX(new, (double)a, (double)b);
			 gprimes = g_list_prepend(gprimes, new);
			}
		}
	}
	long nprimes = g_list_length(gprimes);
	printf("Gprimes list has %ld entries using limit of %ld.\n", nprimes, limit);
	for(GList* working = gprimes; working != NULL; working=working->next) {
	PRT_COMPLEX(*(gsl_complex*)(working->data));
	}
	NL;
	printf("Search space is %ld\n", nprimes*(nprimes-1)*(nprimes-2)*(nprimes-3));

	// ----------Create eqsums, a GList of gsl_vector_complex----------
	
	printf("Creating eqsums list\n");
	
	// Vector has format: {total, a, b, c, d} 
		
	// Setup the combination(n, k)
	gsl_combination* c = gsl_combination_calloc(nprimes,4);
	if(c == NULL) {
		printf("gsl_combination_calloc failed.\n");
		exit(1);
	}
	
	unsigned long count = 0;
	do {
		// allocate a vector
		gsl_vector_complex* vc5 = gsl_vector_complex_calloc(5);	// all elements are zero
		
		// set values in a, b, c, d
		for(int x = 0; x < 4; ++x) {
			
			// get the complex number from gprimes
			gsl_complex ri = *(gsl_complex*)(g_list_nth_data(gprimes, gsl_combination_get(c,x)));			
			
			// set the complex in vector[index x+1]
			gsl_vector_complex_set(vc5, x+1, ri);
			
			// update total
			gsl_vector_complex_set( vc5, 0, gsl_complex_add(gsl_vector_complex_get(vc5,0), ri) );
		}
			
		// prepend to eqsums
		eqsums = g_list_prepend(eqsums, vc5);

		// increment count
		count += 1;
		
	} while(gsl_combination_next(c) == GSL_SUCCESS);

	printf("Sorting eqsums on total value.\n");
	// sort eqsums on total
	eqsums  = g_list_sort(eqsums, wrap_compare_gsl_complex);

	printf("Eqsums length: %u\n", g_list_length(eqsums));

	// Any block of equal totals which has < 6 members can be discarded.
	printf("Starting scan for blocks < 6.\n");
	GList *first = NULL;
	GList *sentinel = NULL;
	FILE* ftargets = fopen("../data/targets.txt","w");
	first = eqsums;
	while(first != NULL) {
		int block_size = 1;
		sentinel = first->next;
		while ((sentinel != NULL)&&(wrap_compare_gsl_complex(sentinel->data,first->data)==0)) {
			block_size += 1;
			sentinel = sentinel->next;
		}
		// drill down to get actual total being considered
		
		gsl_vector_complex* p_lv = (gsl_vector_complex*)(first->data);
		gsl_complex* p_lc = gsl_vector_complex_ptr(p_lv,0);

		//printf("Total: (%2.0f,%2.0f)  Block_size: %d\n", GSL_REAL(*p_lc), GSL_IMAG(*p_lc), block_size);
		
		// delete any small blocks
		if(block_size < 6) {
			delete_block(&eqsums, &first, &sentinel);
		} else {
			// write block data to file
			if(ftargets != NULL) 
				fprintf(ftargets,"Total: (%2.0f,%2.0f)  Block_size: %d\n", 
					GSL_REAL(*p_lc), GSL_IMAG(*p_lc), block_size);
		}
		
		// move pointers
		first = sentinel;
		sentinel = NULL;
	} // while first != NULL....
	printf("Scan complete.\n");	
	printf("Eqsums length: %u\n", g_list_length(eqsums));
	fclose(ftargets);

	// write eqsums to file
	fout = fopen("../data/equalsums_database.bin", "wb");
	if(fout != NULL) {
		printf("Writing to ../data/equalsums_database.bin\n");
		for(first = eqsums; first != NULL; first = first->next) {
			gsl_vector_complex *p_vc = (gsl_vector_complex*)(first->data);
			if(gsl_vector_complex_fwrite(fout, p_vc) != 0)
				printf("Error: Problem writing vector_complex to file.\n");
		}
	} else {
		printf("Error: Could not open %s for writing.\n","../data/equalsums_database.bin");
	}
	// Database integrity check
	fseek(fout, 0, SEEK_END);
	long int file_size = ftell(fout);
	
	if(file_size == (g_list_length(eqsums)* 16 * 5)) {
		printf("File size check: Pass.\n");
	} else {
		printf("File size check: FAIL.\n");
	}
	
	if(file_size % (16 * 5) == 0) {
		printf("File modulus check: Pass.\n");
	} else {
		printf("File modulus check: FAIL.\n");
	}
	
	fclose(fout);
			
	printf("create_database complete.\n");
	
	//==========cleanup code==========
	gsl_combination_free(c);
}


