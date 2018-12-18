/*
 * system.c
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


int main( int argc, char *argv[] )
{

  FILE *fp,ftarg, fresult;
  char path[256];
  
  /* Open the targets file for reading */
  ftarg = fopen("./targets.txt", "r");
  if(ftarg == NULL) {
	  printf("Error: Targets file not found.\n");
	  exit(1);
  }
  /* Open the results file for read/write */
  ftarg = fopen("./results.txt", "rw");
  if(ftarg == NULL) {
	  printf("Error: Results file not found.\n");
	  exit(1);
  }
  /* Scan targets 

  /* Open the command for reading. */
  fp = popen("mpirun -host mike-XPS-Mint19:8 ../bin/mts -q -t 09,33", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  /* Read the output a line at a time - output it. */
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    printf("%s", path);
  }

  /* close */
  pclose(fp);

  return 0;
}
