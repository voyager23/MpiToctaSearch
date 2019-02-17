/*
 * auto_search_2.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../mpi_include/auto_search_2.h"


int main( int argc, char *argv[] )
{

	FILE *ftarg, *fresult;
	char path[256];
	char target_buffer[256];
	char solns_buffer[256];
	int result, match = 0;
	int found,real, imag, nsolns;
	char search_str[256];
	char command[256];
	
	/* Open the targets file for reading */
	ftarg = fopen("../data/search_targets.txt", "r");
	if(ftarg == NULL) {
	  printf("Error: Targets file not found.\n");
	  exit(1);
	}
	

	while( fgets(target_buffer, 255, ftarg) != NULL) {
		
		found = sscanf(target_buffer, "Total: (%d,%d) ", &real, &imag);
		if(found != 2) {
			printf("Error: sscanf did not find (real,imag).\n");
			exit(1);
		}
		printf("\nReal: %d Imag: %d\n", real, imag);

		sprintf(command, "mpirun -host jupiter:6,node1:8,node2:8,node3:8,node4:8 ../bin/mts -t %d,%d\n", real, imag);
		printf("Command string: %s", command);
		result = system(command);
		if ( result == 0 )
			puts ( "Command succeded" );
		else
			perror( "Command return non-zero result" );
		
		// Assume the output from each round is stored in ../results/solution_data.txt
		sprintf(solns_buffer, "../results/solns_tocta_%d_%d.txt", real, imag);
		result = rename("../results/solution_data.txt", solns_buffer);
		if ( result == 0 )
			puts ( "File successfully renamed" );
		else
			perror( "Error renaming file" );

	}


	// Cleanup code
	fclose(ftarg);
}
