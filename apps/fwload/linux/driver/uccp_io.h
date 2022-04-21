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
*** File Name  : uccp_io.h
***
*** File Description:
*** This file has defines/declarations related to UCCP I/O. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _UCCP_IO_H_
#define _UCCP_IO_H_

/******************************* Includes ************************************/


/******************************* Defines *************************************/
#define img_readl(addr, upriv)	\
	uccp_io_readl((volatile void *)(addr), upriv)

#define img_writel(addr, b, upriv) \
	uccp_io_writel((volatile void *)(addr),(b), upriv)

#define img_memcpy(dst_addr, src_addr, len, upriv) \
	uccp_io_memcpy((volatile void *)(dst_addr), \
		       (void *)(src_addr), \
		       len, upriv)
			   
#define img_memset(dst_addr, val, len, upriv) \
	uccp_io_memset((volatile void *)(dst_addr), val, len, upriv)


/******************************* Enumerations *********************************/


/******************************* Typedefs *************************************/


/******************************* Variables ***********************************/


/******************************* Function Prototypes **************************/
unsigned int uccp_io_readl(const volatile void *addr, 
			   struct uccp_priv *upriv);

void uccp_io_writel(const volatile void *addr, 
		    unsigned int val, 
		    struct uccp_priv *upriv);

void uccp_io_memcpy(const volatile void *dst_addr, 
		    void *src_addr, 
		    unsigned int len,
		    struct uccp_priv *upriv);

void uccp_io_memset(const volatile void *dst_addr, 
		    char set_val, 
		    unsigned int len,
		    struct uccp_priv *upriv);

#endif /* _UCCP_IO_H_ */
