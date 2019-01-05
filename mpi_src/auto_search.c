/*
 * auto_search.c
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

#include "../include/auto_search.h"


int main( int argc, char *argv[] )
{

	FILE *ftarg, *fresult;
	char path[256];
	char target_buffer[256];
	char solns_buffer[256];
	int match = 0;
	int found,real, imag, nsolns;
	char search_str[256];
	char command[256];
	
	time_t rawtime;
	struct tm *timeinfo = NULL;
	char buffer[128];
	
	/* set buffer to unique filename for results */
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 128, "../data/soln_count-%y%m%d-%H%M", timeinfo);
		
	/* Open the targets file for reading */
	ftarg = fopen("../data/short_targets.txt", "r");
	if(ftarg == NULL) {
	  printf("Error: Targets file not found.\n");
	  exit(1);
	}
	/* Open the results file for read/write */
	fresult = fopen(buffer, "a+");
	if(fresult == NULL) {
	  printf("Error: Results file %s not opened.\n",buffer);
	  exit(1);
	}

	while( fgets(target_buffer, 255, ftarg) != NULL) {
		
		found = sscanf(target_buffer, "Total: (%d,%d) ", &real, &imag);
		if(found != 2) {
			printf("Error: sscanf did not find (real,imag).\n");
			exit(1);
		}

		sprintf(search_str, "Total: (%d,%d)", real, imag);
		rewind(fresult);
		match = 0;
		while( fgets(solns_buffer, 255, fresult) != NULL) {
			if(strstr(solns_buffer, search_str) != NULL) {
				match = 1;
				break;
			}
		}
		
		// DEBUG break
		//printf("Debug break\n");
		printf("Filename: %s\n", buffer);
		//break;
		// END DEBUG
		
		if(match == 0) {
			// launch a search
			sprintf(command, "mpirun -host jupiter:7 ../bin/mts -q -t %02d,%02d", real, imag);
			// printf("Command line - %s\n", command);
			
			FILE *fp;
			/* Open the command for reading. */
			fp = popen(command, "r");
			if (fp == NULL) {
			printf("Failed to run command\n" );
			exit(1);
			}
			/* Read the output a line at a time - output it. */
			while (fgets(path, sizeof(path)-1, fp) != NULL) {
			//printf("Path = <%s>", path);
			
			// Format and write this result to the fresult file
			sscanf(path, "Final solution count for (%d,%d) : %d", &real, &imag, &nsolns);
			printf("real: %d	imag: %d	nsolns: %d\n", real, imag, nsolns);
			int written = fprintf(fresult, "Total: (%d,%d) Solutions: %d\n", real, imag, nsolns);
			}
			/* close */
			pclose(fp);
		} else {
			printf("Total: (%d,%d) exists in soln_count.\n", real, imag);
		}
	}
close:	fclose(ftarg);
		fclose(fresult);
}
