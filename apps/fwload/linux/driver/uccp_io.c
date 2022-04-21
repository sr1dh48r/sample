/*HEADER**********************************************************************
******************************************************************************
***
*** Copyright (c) 2013, 2014 Imagination Technologies Ltd.
***
*** This program is free software; you can redistribute it and/or
*** modify it under the terms of the GNU General Public License
*** as published by the Free Software Foundation; either version 2
*** of the License, or (at your option) any later version.
***
*** This program is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*** GNU General Public License for more details.
***
*** You should have received a copy of the GNU General Public License
*** along with this program; if not, write to the Free Software
*** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
*** USA.
***
*** File Name  : uccp_io.c
***
*** File Description:
*** This file has API's related to UCCP I/O. 
***
******************************************************************************
*END**************************************************************************/
#include "uccp.h"
#include "uccp_io.h"

/* 
 * uccp_io_readl
 *
 * Perform a 32-bit read from the UCCP memory location pointed by 'addr' and 
 * return the value
 */
unsigned int uccp_io_readl(const volatile void *addr, 
			   struct uccp_priv *upriv)
{
	return 0;
}


/* 
 * uccp_io_writel
 *
 * Perform a 32-bit write to the UCCP memory location pointed by 'addr'.
 * The value to be written is contained in 'val'
 */
void uccp_io_writel(const volatile void *addr, unsigned int b, 
		    struct uccp_priv *upriv)
{
	return;
}


/* 
 * uccp_io_memcpy
 *
 * Perform a memcpy of 'len' bytes from 'src_addr' to the UCCP memory location
 * pointed by 'dst_addr'.
 * 
 * dst_addr is always a 4 byte aligned address
 * src_addr is always a 4 byte aligned address
 * len may NOT be a multiple of 4 when dst_addr is of type 0xB7xxxxxx
 * len is always a multiple of 4 when dst_addr is of type 0xB4xxxxxx
 *
 * This function is only invoked for 0xB7xxxxxx and 0xB4xxxxxx regions
 *
 * When dst_addr is of type 0xB4xxxxxx, perform only 32 bit writes to these 
 * locations
 * 
 */
void uccp_io_memcpy(const volatile void *dst_addr, 
		    void *src_addr, 
		    unsigned int len,
		    struct uccp_priv *upriv)
{
	return;
}

/* 
 * uccp_io_memset
 *
 * Perform a memset of 'len' bytes with value of 'val' to the UCCP memory 
 * location pointed by 'dst_addr'.
 * 
 * dst_addr is always a 4 byte aligned address
 * src_addr is always a 4 byte aligned address
 * len may NOT be a multiple of 4 when dst_addr is of type 0xB7xxxxxx
 * len is always a multiple of 4 when dst_addr is of type 0xB4xxxxxx
 *
 * This function is only invoked for 0xB7xxxxxx and 0xB4xxxxxx regions
 *
 * When dst_addr is of type 0xB4xxxxxx, perform only 32 bit writes to these 
 * locations
 * 
 */
void uccp_io_memset(const volatile void *dst_addr, 
		    char val, 
		    unsigned int len,
		    struct uccp_priv *upriv)
{
	return;
}
