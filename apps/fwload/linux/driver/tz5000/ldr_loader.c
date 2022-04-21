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
*** File Name  : ldr_loader.c
***
*** File Description:
*** This file implements API's for interaction with UCCP. 
***
******************************************************************************
*END**************************************************************************/
#include <linux/delay.h>

#include "ldr_loader.h"
#include "uccp_io.h"
#include "debug.h"


void SoftReset(struct uccp_priv *upriv)
{
#if UCCP420_HW
	unsigned int val, temp;
	unsigned int retries = 3;

	/* If the thread is running, then stop it and clear the registers, 
	 * otherwise do nothing. */
	val = SingleRead(MTX_TXENABLE_ADDR, upriv);

	img_info("Resetting UCCP420\n");

	/* Soft Reset */
	val = img_readl(MSLVSRST, upriv);
	img_writel(MSLVSRST,(val | 1), upriv);

	/* Wait for 16 core clock cycles. Core runs at 320MHz */
	udelay(10);

	/* Clear the Soft Reset */
	img_writel(MSLVSRST,(val & 0xFFFFFFFE), upriv);

	/* Give additional 20 ms for the DA to do its own reset */
	mdelay(20);

	/* From here, everything is as in the 'reset' function of the 
	 * target-common-meta.c */

	/* Clear the Minim Bit in PrivExt */
	SingleWrite(MTX_TXPRIVEXT_ADDR, 0, upriv);

	/* Set the PCX value i to 0 */
	SingleWrite(MTX_TXUXXRXDT, 0, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_PCX_REG_IND_ADDR, upriv);


	/*
	 * Clear TXPOLL{I} to clear TXSTAT{I}
	 * Writing 0xFFFFFFFF clears TXSTATI, but TXMASKI must
	 * be all set too for this to work.
	 */
	SingleWrite(MTX_TXUXXRXDT, 0xFFFFFFFF, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_TXMASK_REG_IND_ADDR, upriv);

	SingleWrite(MTX_TXUXXRXDT, 0xFFFFFFFF, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_TXMASKI_REG_IND_ADDR, upriv);

	SingleWrite(MTX_TXUXXRXDT, 0xFFFFFFFF, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_TXPOLL_REG_IND_ADDR, upriv);

	SingleWrite(MTX_TXUXXRXDT, 0xFFFFFFFF, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_TXPOLLI_REG_IND_ADDR, upriv);

	/* Clear TXMASK and TXMASKI */
	SingleWrite(MTX_TXUXXRXDT, 0x0, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_TXMASK_REG_IND_ADDR, upriv);


	SingleWrite(MTX_TXUXXRXDT, 0x0, upriv);
	SingleWrite(MTX_TXUXXRXRQ, MTX_TXMASKI_REG_IND_ADDR, upriv);

	/* Ensure all kicks are cleared */
	SingleWrite(MTX_TXUXXRXRQ, 
		    MTX_TXPOLLI_REG_IND_ADDR | REG_IND_READ_FLAG, 
		    upriv);

	temp = SingleRead(MTX_TXUXXRXDT, upriv);

	while (temp && retries--) {
		SingleWrite(MTX_TXUXXRXDT, 0x2, upriv);
		SingleWrite(MTX_TXUXXRXRQ, 
			    MTX_TXPOLLI_REG_IND_ADDR, 
			    upriv);

		SingleWrite(MTX_TXUXXRXRQ, 
			    MTX_TXPOLLI_REG_IND_ADDR | 
			    REG_IND_READ_FLAG, 
			    upriv);

		temp = SingleRead(MTX_TXUXXRXDT, upriv);
	}

	/* Reset TXSTATUS */
	SingleWrite(MTX_TXSTATUS_ADDR, 0x00020000, upriv);

#endif
	img_info("Soft Reset core\n");
}


void SingleWrite(unsigned int addr, unsigned int data, struct uccp_priv *upriv)
{
#if UCCP420_HW
	/* Poll MSLVCTRL1 */
	while (!MSLAVE_READY(img_readl(MSLVCTRL1, upriv)));

	img_writel(MSLVCTRL0, 
		   ((addr & SLAVE_ADDR_MODE_MASK) | SLAVE_SINGLE_WRITE), upriv);
	img_writel(MSLVDATAT, data, upriv);
#endif
}


unsigned int SingleRead(unsigned int addr, 
			struct uccp_priv *upriv)
{
#if UCCP420_HW
	unsigned int val =0, wrval = 0, rval = 0;
	val = img_readl(MSLVCTRL1, upriv);
	
	img_dbg_info("Mslave ctrl1 0x%X\n", val);

	/* Poll MSLVCTRL1 */
	while (!MSLAVE_READY(val))
		val = img_readl(MSLVCTRL1, upriv);

	wrval = ((addr & SLAVE_ADDR_MODE_MASK) | SLAVE_SINGLE_READ);

	img_dbg_info("Addr val 0x%X\n", wrval);

	img_writel(MSLVCTRL0, wrval, upriv);

	val = img_readl(MSLVCTRL1, upriv);

	img_dbg_info("Mslave ctrl1 0x%X\n", val);

	/* Poll MSLVCTRL1 */
	while (!MSLAVE_READY(val)) {
		val = img_readl(MSLVCTRL1, upriv);

		img_dbg_info("Mslave ctrl1 0x%X\n", val);
	}

	rval = img_readl(MSLVDATAX, upriv);

	img_dbg_info("Mslave DataX 0x%X\n", rval);

	return rval;
#else
	return 0;
#endif
}


void BlockWrite(unsigned int addr, 
		unsigned char *pdata, 
		unsigned int len, 
		struct uccp_priv *upriv)
{
#if UCCP420_HW
	unsigned int i=0;
	unsigned int l_len = len / 4;
	unsigned int *src_data = (unsigned int *)pdata;
	
	/* Poll MSLVCTRL1 */
	while (!MSLAVE_READY(img_readl(MSLVCTRL1, upriv)));

	img_writel(MSLVCTRL0, 
		   ((addr & SLAVE_ADDR_MODE_MASK) | SLAVE_BLOCK_WRITE), upriv);

	for (;i < l_len; i++)
	{
		while (!MSLAVE_READY(img_readl(MSLVCTRL1, upriv)));
		
		if(pdata != NULL)
			img_writel(MSLVDATAT, src_data[i], upriv);
		else
			img_writel(MSLVDATAT, 0x00, upriv);
	}
	
#endif	
}


void BlockRead(unsigned int addr, 
	       unsigned int *pdata, 
	       unsigned int len, 
	       struct uccp_priv *upriv)
{
#if UCCP420_HW
	unsigned int i=0;

	/* Poll MSLVCTRL1 */
	while (!MSLAVE_READY(img_readl(MSLVCTRL1, upriv)));

	img_writel(MSLVCTRL0, 
		   ((addr & SLAVE_ADDR_MODE_MASK) | SLAVE_BLOCK_READ), upriv);

	for (;i < len-1; i++) {
		while (!MSLAVE_READY(img_readl(MSLVCTRL1, upriv)));
		pdata[i] = img_readl(MSLVDATAT, upriv);
	}

	/* Read the last word */
	while (!MSLAVE_READY(img_readl(MSLVCTRL1, upriv)));
	pdata[len-1] = img_readl(MSLVDATAX, upriv);
#endif
}
