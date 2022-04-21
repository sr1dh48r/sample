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
*** File Name  : ldr_api.c
***
*** File Description:
*** This file implements API's for LDR commands. 
***
******************************************************************************
*END**************************************************************************/
#include <linux/io.h>
#include <linux/kernel.h>

#include "ldr_api.h"
#include "ldr_loader.h"
#include "debug.h"
#include "uccp_io.h"


void ldr_load_mem(unsigned int DstAddr, 
		 unsigned int Length, 
		 unsigned char *pSrcBuf, 
		 struct uccp_priv *upriv)
{
	enum uccp_mem_region mem_region = ERROR_REGION;
	int i = 0;

	mem_region = ldr_check_region(DstAddr, Length);	
	
	img_dbg_info("ldr_load_mem DstAddr = 0x%X, length = 0x%X, "
		     "srcaddr = 0x%X\n", 
		     DstAddr, Length, (unsigned int)pSrcBuf);

	img_dbg_info("Dump upto 16 bytes\n");

	if(0 != (DstAddr % 4))
		img_dbg_info("Destination Address is not 4 - byte aligned\n");
				
	for(i = 0; i < 16; i += 2)
		img_dbg_dump("0x%X \t 0x%X\n", pSrcBuf[i], pSrcBuf[i + 1]);

	switch(mem_region)
	{
		case CORE_REGION: 
			BlockWrite(DstAddr, pSrcBuf, Length, upriv);
			break;
		
		case DIRECT_REGION:
			img_memcpy(DstAddr, pSrcBuf, Length, upriv);
			break;
			
		default:
			img_dbg_err("Region unknown. Skipped writing\n");
			break;
	}
}

void ldr_start_thread(unsigned int ThreadNumber, 
		      unsigned int StackPointer,
		      unsigned int ProgramCounter,
		      unsigned int CatchStateAddress,
		      struct uccp_priv *upriv)
{
	(void) ThreadNumber;
	(void) CatchStateAddress;

	img_dbg_info("ldr_start_thread PC = 0x%X,\tSP = 0x%X\n", 
		     ProgramCounter,StackPointer);
			
	/* Program Counter */
	SingleWrite(MTX_TXUXXRXDT, ProgramCounter, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_PC_REG_IND_ADDR, upriv);
	
	/* Stack Pointer */
	SingleWrite(MTX_TXUXXRXDT, StackPointer, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_A0STP_REG_IND_ADDR, upriv);
		
	/* Thread Enable */
	SingleWrite(MTX_TXENABLE_ADDR, MTX_START_EXECUTION, upriv);
	img_info("Thread is Enabled\n");
}

void ldr_stop_thread(unsigned int ThreadNumber, 
		     struct uccp_priv *upriv)
{
	unsigned int val;
	(void)ThreadNumber;
					
	/* Thread Disable */
	SingleWrite(MTX_TXENABLE_ADDR, MTX_STOP_EXECUTION, upriv);
	val = SingleRead(MTX_TXENABLE_ADDR, upriv);
	
#if UCCP420_HW
	while ((val & 0x2) != 0x2) {
		val = SingleRead(MTX_TXENABLE_ADDR, upriv);

	img_dbg_info("ldr_stop_thread val = 0x%X\n",val);
	
	}	
#endif
	
	img_dbg_info("TXENABLE = 0x%X\n",val);
	img_info("Thread is Stopped\n");
}


void ldr_soft_reset(unsigned int ThreadNumber, 
		    struct uccp_priv *upriv)
{
	(void)ThreadNumber;
	
	img_dbg_info("ldr_soft_reset\n");	
		
	SoftReset(upriv);	
}


void ldr_zero_mem(unsigned int DstAddr, 
		 unsigned int Length, 
		 struct uccp_priv *upriv)
{
	int mem_region = 0;

	img_dbg_info("ldr_zero_mem DestAddr = 0x%X, length = 0x%X\n", 
		     DstAddr, Length);

	if(0 != (DstAddr % 4))
		img_dbg_info("Destination Address is not 4 - byte aligned");

	mem_region = ldr_check_region(DstAddr, Length);		

	switch(mem_region)
	{
		case CORE_REGION: 
			BlockWrite(DstAddr, NULL, Length, upriv);
			break;
		
		case DIRECT_REGION:
			img_memset(DstAddr, 0x00, Length, upriv);
			break;
							
		default:
			img_dbg_err("Region unknown. Skipped setting\n");
			break;
	}
}


unsigned int ldr_config_read(unsigned int DstAddr,
			     struct uccp_priv *upriv)
{
	int mem_region = 0;
	
	img_dbg_info("ldr_config_read DstAddr = 0x%X\n", DstAddr);
		
	if(0 != (DstAddr % 4))
		img_dbg_info("Destination Address is not 4 - byte aligned");	
	
	mem_region = ldr_check_region(DstAddr, 0);

	switch(mem_region)
	{
		case CORE_REGION: 
			return SingleRead(DstAddr, upriv);
					
		case DIRECT_REGION:
			return img_readl(DstAddr, upriv);
			
		default:
			img_dbg_err("Region unknown. Skipped reading\n");
			return 0;
	}
		
	return 0;
}

void ldr_config_write(unsigned int DstAddr, 
		      unsigned int Value, 
		      struct uccp_priv *upriv)
{
	int mem_region = 0;
	
	img_dbg_info("ldr_config_write DstAddr = 0x%X,\tValue = 0x%X\n", 
		     DstAddr,Value);
		
	if(0 != (DstAddr % 4))
		img_dbg_info("Destination Address is not 4 - byte aligned");	
		
	mem_region = ldr_check_region(DstAddr, 0);			
		
	
	switch(mem_region)
	{
		case CORE_REGION: 
			SingleWrite(DstAddr, Value, upriv);
			break;
		
		case DIRECT_REGION:
			img_writel(DstAddr, Value, upriv);
			break;
		
		default:
			img_dbg_err("Region unknown. Skipped writing\n");
			break;
	}

}

enum uccp_mem_region ldr_check_region(unsigned int SrcAddr, int length)
{
	unsigned int DstAddr = SrcAddr + length;
	
	if(((SrcAddr >= 0x03000000) && (SrcAddr <= 0x04FFFFFF))  || 
	   ((SrcAddr >= 0x80000000) && (SrcAddr <= 0x87FFFFFF))) {
		if(length != 0) {
			if(((DstAddr >= 0x03000000) && (DstAddr <= 0x04FFFFFF)) ||
			   ((DstAddr >= 0x80000000) && (DstAddr <= 0x87FFFFFF)))
				return CORE_REGION;
			else
				return ERROR_REGION;
		}
		return CORE_REGION;
	}	
	else if((SrcAddr & 0xFF000000) == 0xB0000000)
		return ERROR_REGION;
	else
		return DIRECT_REGION;
}		
