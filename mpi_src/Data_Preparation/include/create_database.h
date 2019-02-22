/*
 * create_database.h
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

#ifndef __CREATE_DATABASE_H__
	#define __CREATE_DATABASE_H__

	#include <stdlib.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <math.h>
	#include <gsl/gsl_combination.h>
	#include "../../../mpi_include/toolbox.h"

	int wrap_compare_gsl_complex(const void* left, const void* right);
	_Bool IsPrime(unsigned long number);
	void delete_block(GList **list, GList **first, GList **sentinel);
	int compare_gsl_complex(const void* plc, const void* prc);
#endif
