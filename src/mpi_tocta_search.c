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
	gsl_complex target;
	char base_name[128];	

	
	// get options from command line
	int q_flag = 0;
	int l_flag = 0;
	char *t_value = "0,0";
	int c, sf, r, i;
	
	while ((c = getopt (argc, argv, "qlt:")) != -1)
	switch (c)
		{
		case 'q':
		q_flag = 1;
		break;
		case 'l':
		l_flag = 1;
		break;
		case 't':
		t_value = optarg;
		break;
		case '?':
		if (optopt == 't')
		fprintf (stderr, "Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
		fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		else
		fprintf (stderr,
		"Unknown option character `\\x%x'.\n",
		optopt);
		return 1;
		default:
		abort ();
	}	

	// display flags
	// printf("q_flag: %d     l_flag: %d     t_value: %s \n", q_flag, l_flag, t_value);
	if(q_flag == 1) printf("Quiet flag (q) not implemented.\n");
	if(l_flag == 1) printf("List-target flag (l) not implemented.\n\n");
	sf = sscanf(t_value, "%d,%d", &r, &i);
	if(sf != 2) {
		printf("Could not parse the requested target.\n");
		exit(1);
	} else {
		// set target value
		GSL_SET_COMPLEX(&target, (double)r, (double)i);
	}
	
	prepare_equalsums("../data/equalsums_database.bin", &equalsums, &target);
	printf("equalsums->size: %lu\n", equalsums->size);
	printf("Target: "); PRT_COMPLEX(target); NL;
	
}

