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

//======================================================================
int main(int argc, char **argv)
{

	gsl_vector_ulong* equalsums = NULL;
	int solns;
	GList* AllSolutions = NULL;
	GList* SolutionLists = NULL;

	// get these options from command line
	gsl_complex target;
	int q_flag = 0;
	int l_flag = 0;
	
	get_options(argc, argv, &target, &q_flag, &l_flag);
	
	// display flags
	// printf("q_flag: %d     l_flag: %d\n", q_flag, l_flag);
	
	prepare_equalsums("../data/equalsums_database.bin", &equalsums, &target);
	printf("equalsums->size: %lu\n", equalsums->size);
	printf("Target: "); PRT_COMPLEX(target); NL;
	
}

