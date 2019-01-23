/*
 * gcrypt.c
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
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

int main(int argc, char **argv)
{
#if(0)
	/* Version check should be the very first call because it
	 makes sure that important subsystems are initialized. */
	if (!gcry_check_version (GCRYPT_VERSION))
	{
	  fputs ("libgcrypt version mismatch\n", stderr);
	  exit (2);
	}

	/* Disable secure memory.  */
	gcry_control (GCRYCTL_DISABLE_SECMEM, 0);

	/* ... If required, other initialization goes here.  */

	/* Tell Libgcrypt that initialization has completed. */
	gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
	
	gcry_error_t e;
	gcry_md_hd_t hd;
	e = gcry_md_open(&hd, GCRY_MD_SHA1, 0);
#endif

	char digest[128];
	for(int i = 0; i < 128; ++i) digest[i] = 0xFF;
	char *buffer = "The quick brown fox jumps over the lazy dog";
	gcry_md_hash_buffer (GCRY_MD_SHA1, digest, buffer, 43);
	
	for(int i = 0; i < 20; ++i) printf("%x ", digest[i]&0x00ff);
	printf("\n2f d4 e1 c6 7a 2d 28 fc ed 84 9e e1 bb 76 e7 39 1b 93 eb 12\n");

	// gcry_md_close (hd);
	return 0;
}

