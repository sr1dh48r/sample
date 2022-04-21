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
#include <linux/dma-mapping.h>
#include "uccp.h"
#include "uccp_io.h"
#include "debug.h"

#define WLN_CGN_REG_READ(x)       \
		readl((void __iomem *)upriv->wifiT1_mem_addr + (x))
#define WLN_CGN_REG_WRITE(x, y)   \
		writel((y), (void __iomem *)(upriv->wifiT1_mem_addr + (x)))
#define WLN_UCCP_REG_READ(x)      \
		readl((void __iomem *)upriv->wifiT0_mem_addr + (x))
#define WLN_UCCP_REG_WRITE(x, y)  \
		writel((y), (void __iomem *)(upriv->wifiT0_mem_addr + (x)))

static void
uccp_set_offset(struct uccp_priv *upriv, unsigned char offset)
{
	unsigned int val;

	if (upriv->offset != offset) {
		/* update HOST_SYS_ADDR_OFFSET[7:0] of */
		/* wln_cgn[WLN_CGN_OFFSET] register    */
		val =  WLN_CGN_REG_READ(WLAN_CGN_OFFSET_OFS);
		val =  (val & ~0x000000FF) | offset;
		img_dbg_info("uccp_set_offset() val=0x%X\n", val);
		WLN_CGN_REG_WRITE(WLAN_CGN_OFFSET_OFS, val);

		/* save offset value to internal variable */
		upriv->offset = offset;
	}
}


/* 
 * uccp_io_readl
 *
 * Perform a 32-bit read from the UCCP memory location pointed by 'addr' and 
 * return the value
 */
unsigned int uccp_io_readl(const volatile void *addr, 
			   struct uccp_priv *upriv)
{
	unsigned int val;
	unsigned char host_sys_addr_offset = (unsigned long)addr >> 24;
	unsigned long offset = (unsigned long)addr & WLAN_UCCP_OFFSET_MASK;

	img_dbg_info("readl()");

	/* set a value of upper 8bit of address to wln_cgn[WLN_CGN_OFFSET] */
	uccp_set_offset(upriv, host_sys_addr_offset);

	/* read 4-Bytes value from wifi_T0 */
	val = WLN_UCCP_REG_READ(offset);

	img_dbg_info("addr=%p val=0x%X\n", addr, val);

	return val;
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
	unsigned char host_sys_addr_offset = (unsigned long)addr >> 24;
	unsigned long offset = (unsigned long)addr & WLAN_UCCP_OFFSET_MASK;

	img_dbg_info("writel()");

	/* set a value of upper 8bit of address to wln_cgn[WLN_CGN_OFFSET] */
	uccp_set_offset(upriv, host_sys_addr_offset);

	/* write 4-Bytes value to wifi_T0 */
	WLN_UCCP_REG_WRITE(offset, *((unsigned long *)&(b)));

	img_dbg_info("addr=%p data=0x%X\n", addr, b);

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
	unsigned char host_sys_addr_offset = (unsigned long)dst_addr >> 24;
	unsigned long offset = (unsigned long)dst_addr & WLAN_UCCP_OFFSET_MASK;
	unsigned long *data_addr = (unsigned long *)src_addr;
	int i;

	img_dbg_info("memcpy()");

	/* set a value of upper 8bit of address to wln_cgn[WLN_CGN_OFFSET] */
	uccp_set_offset(upriv, host_sys_addr_offset);

	img_dbg_info("dst_addr=%p src_addr=%p len=%d\n",
			dst_addr, src_addr, len);
	for (i = 0; i < len / 4; i++) {
		img_dbg_info("offset=0x%X data=0x%X\n",
			(unsigned int)offset,
			(unsigned int)*data_addr);

		/* write a value to wifi_T0 per 4-Byte */
		WLN_UCCP_REG_WRITE(offset, *data_addr);

		offset += 4;
		data_addr++;
	}

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
	unsigned char host_sys_addr_offset = (unsigned long)dst_addr >> 24;
	unsigned long offset = (unsigned long)dst_addr & WLAN_UCCP_OFFSET_MASK;
	unsigned long data = (val << 24) | (val << 16) | (val << 8) | val;
	int i;

	img_dbg_info("memset()");

	/* set a value of upper 8bit of address to wln_cgn[WLN_CGN_OFFSET] */
	uccp_set_offset(upriv, host_sys_addr_offset);

	img_dbg_info("dst_addr=%p val=0x%X len=%d\n", dst_addr, val, len);

	for (i = 0; i < len / 4; i++) {
		img_dbg_info("offset=0x%X data=0x%X\n", (unsigned int)offset,
							 (unsigned int)data);

		/* write a value to wifi_T0 per 4-Byte */
		WLN_UCCP_REG_WRITE(offset, data);

		offset += 4;
	}
	return;
}
