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
*** File Name  : uccp_drv.h
***
*** File Description:
*** This file has defines/declarations related to char driver for UCCP. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _UCCP_DRV_H_
#define _UCCP_DRV_H_

/******************************* Includes ************************************/
#include <linux/types.h>


/******************************* Defines *************************************/
#define IMG_DRIVER_NAME "[UCCP420CHAR] "
#define MAX_LOAD_MEM_LEN 4096

#define UCCPIO 0xF2

#define UCCP_SOFT_RESET   _IO(UCCPIO, 0x01) 
#define UCCP_LOAD_MEMORY  _IOW(UCCPIO, 0x02, struct img_load_mem) 
#define UCCP_START_THREAD _IOW(UCCPIO, 0x03, struct img_thrd_info)
#define UCCP_STOP_THREAD  _IO(UCCPIO, 0x04)
#define UCCP_ZERO_MEMORY  _IOW(UCCPIO, 0x05, struct img_load_mem)
#define UCCP_CONFIG_READ  _IOR(UCCPIO, 0x06, struct img_cfg_rw)
#define UCCP_CONFIG_WRITE _IOW(UCCPIO, 0x07, struct img_cfg_rw)


/******************************* Enumerations *********************************/


/******************************* Typedefs ************************************/


/******************************* Structures ***********************************/
struct img_load_mem
{
	unsigned int DstAddr;
	unsigned int Length;
	unsigned char  *pSrcBuf;
};

struct img_thrd_info
{
	unsigned int ThreadNumber;
	unsigned int StackPointer;
	unsigned int ProgramCounter;
	unsigned int CatchStateAddress;
};

struct img_cfg_rw
{
	unsigned int Addr;
	unsigned int Val;
};


#endif /* _UCCP_DRV_H_ */
