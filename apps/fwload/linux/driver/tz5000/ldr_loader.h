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
*** File Name  : ldr_loader.h
***
*** File Description:
*** This file has defines/declarations related to UCCP interaction. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _LDR_LOADER_H_
#define _LDR_LOADER_H_

/******************************* Variables ************************************/
#include "uccp.h"


/******************************* Defines **************************************/


/******************************* Enumerations *********************************/


/******************************* Typedefs *************************************/


/******************************* Structures ***********************************/


/******************************* Function Prototypes **************************/
void SoftReset(struct uccp_priv *upriv);
void SingleWrite(unsigned int addr, unsigned int data, struct uccp_priv *upriv);
unsigned int SingleRead(unsigned int addr, struct uccp_priv *upriv);
void BlockWrite(unsigned int addr, 
		unsigned char *src_data, 
		unsigned int len, 
		struct uccp_priv *upriv);
void BlockRead(unsigned int addr, 
	       unsigned int *pdata, 
	       unsigned int len, 
	       struct uccp_priv *upriv);

#endif /* _LDR_LOADER_H_ */
