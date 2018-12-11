/*
 * get_options.c
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
 * 
 */

#include "../include/get_options.h"

int get_options(int argc, char **argv, gsl_complex *p_target, int *quiet, int *list) {
	
	char *t_value = "0,0";
	int c, sf, r, i;
	
	while ((c = getopt (argc, argv, "qlt:")) != -1)
	switch (c)
		{
		case 'q':
		*quiet = 1;
		break;
		case 'l':
		*list = 1;
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
		fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
		return 1;
		default:
		abort ();
	}	

	if(*quiet == 1) printf("Quiet flag (q) not implemented.\n");
	if(*list == 1) printf("List-target flag (l) not implemented.\n\n");
	sf = sscanf(t_value, "%d,%d", &r, &i);
	if(sf != 2) {
		printf("Could not parse the requested target.\n");
		exit(1);
	} else {
		// set target value
		GSL_SET_COMPLEX(p_target, (double)r, (double)i);
	}
	
}
