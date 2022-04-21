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
*** File Name  : ldr_api.h
***
*** File Description:
*** This file has defines/declarations related to LDR commands. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _LDR_API_H_
#define _LDR_API_H_

/******************************* Includes ************************************/
#include "uccp.h"

/******************************* Defines *************************************/


/******************************* Enumerations *********************************/


/******************************* Typedefs ************************************/


/******************************* Structures ***********************************/


/******************************* Function Prototypes**************************/
void ldr_soft_reset(unsigned int ThreadNumber,
		    struct uccp_priv *upriv);

void ldr_load_mem(unsigned int DstAddr,
		  unsigned int Length,
		  unsigned char *pSrcBuf,
		  struct uccp_priv *upriv);

void ldr_start_thread(unsigned int ThreadNumber,
		      unsigned int StackPointer,
		      unsigned int ProgramCounter,
		      unsigned int CatchStateAddress,
		      struct uccp_priv *upriv);

void ldr_stop_thread(unsigned int ThreadNumber,
		     struct uccp_priv *upriv);

void ldr_zero_mem(unsigned int DstAddr, 
		  unsigned int Length,
		  struct uccp_priv *upriv);

unsigned int ldr_config_read(unsigned int DstAddr,
			     struct uccp_priv *upriv);

void ldr_config_write(unsigned int DstAddr, 
		      unsigned int Value,
		      struct uccp_priv *upriv);

enum uccp_mem_region ldr_check_region(unsigned int SrcAddr, 
				      int length);

#endif /* _LDR_API_H_ */
