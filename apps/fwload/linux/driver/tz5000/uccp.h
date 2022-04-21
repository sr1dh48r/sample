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
*** File Name  : uccp.h
***
*** File Description:
*** This file has defines/declarations related to UCCP hardware. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _UCCP_H_
#define _UCCP_H_

/******************************* Includes ************************************/


/******************************* Defines *************************************/
#define MTX_REG_INDIRECT(unit,reg) (((reg & 0x7) << 4) | (unit & 0xF))

#define MTX_PC_REG_IND_ADDR        MTX_REG_INDIRECT(5,0)
#define MTX_A0STP_REG_IND_ADDR     MTX_REG_INDIRECT(3,0)


#define MTX_PCX_REG_IND_ADDR    MTX_REG_INDIRECT(5,1)
#define MTX_TXMASK_REG_IND_ADDR  MTX_REG_INDIRECT(7,1)
#define MTX_TXMASKI_REG_IND_ADDR MTX_REG_INDIRECT(7,3)
#define MTX_TXPOLL_REG_IND_ADDR MTX_REG_INDIRECT(7,4)
#define MTX_TXPOLLI_REG_IND_ADDR MTX_REG_INDIRECT(7,6)
#define MTX_TXSTAT_REG_IND_ADDR MTX_REG_INDIRECT(7,0)
#define MTX_TXSTATI_REG_IND_ADDR MTX_REG_INDIRECT(7,2)

#define REG_IND_READ_FLAG (1<<16)

#define MTX_TXPRIVEXT_ADDR 	   0x048000E8
#define MTX_TXSTATUS_ADDR 	   0x48000010
#define	MTX_TXENABLE_ADDR          0x04800000
#define	MTX_START_EXECUTION        1
#define	MTX_STOP_EXECUTION         0

#define MTX_TXUXXRXDT              0x0480FFF0
#define MTX_TXUXXRXRQ              0x0480FFF8

#define MSLV_BASE_ADDR	           0x0203C000


/* DATA Exchange Register */
#define MSLVDATAX                  MSLV_BASE_ADDR + 0x2000

/* DATA Transfer Register */
#define MSLVDATAT                  MSLV_BASE_ADDR + 0x2040

/* Control Register 0 */
#define MSLVCTRL0                  MSLV_BASE_ADDR + 0x2080

/* Soft Reset register */
#define MSLVSRST                   MSLV_BASE_ADDR + 0x2600

#define SLAVE_ADDR_MODE_MASK       0xFFFFFFFC
#define SLAVE_SINGLE_WRITE	   0x00
#define SLAVE_SINGLE_READ	   0x01
#define SLAVE_BLOCK_WRITE          0x02
#define SLAVE_BLOCK_READ           0x03

/* Control Register 1 */
#define MSLVCTRL1                  MSLV_BASE_ADDR + 0x20c0

#define MSLVCTRL1_POLL_MASK        0x07000000
#define MSLAVE_READY(v)            ((v & MSLVCTRL1_POLL_MASK) == \
				    MSLVCTRL1_POLL_MASK)

#define LTP_THREAD_NO 0 /* Since, only one thread exists */

#define WLN_WIFI_T1_BASE         0xF2030000
#define WLN_WIFI_T1_LEN          0x1000
#define WLN_WIFI_T0_BASE         0xF6000000
#define WLN_WIFI_T0_LEN          0x1000000

#define WLAN_UCCP_SYSBUS_REG     0x02
#define WLAN_UCCP_GRAM_PACKED    0xB7
#define WLAN_UCCP_GRAM_MSB       0xB4
#define WLAN_UCCP_OFFSET_MASK    0x00FFFFFF

/* -----wifi_T1(wln_cgn) register----- */
#define WLAN_CGN_OFFSET_OFS      0x820
/* -----wifi_T1(wln_cgn) register----- */

/******************************* Enumerations *********************************/
enum uccp_mem_region
{
	CORE_REGION,
	DIRECT_REGION,
	ERROR_REGION
};


/******************************* Typedefs ************************************/


/******************************* Structures ***********************************/
struct uccp_priv {
	struct class *uccp_class;
	int uccp_major;
	struct device *device;
	/*
	 * Add members to hold base addresses of the following UCCP memory 
	 * regions 
	 * UCCP Slave port:  Start: 0x0203C000, Length to be mapped: 0x4000
	 * UCCP packed GRAM: Start: 0xB7000000, Length to be mapped: 0x66CC0
	 * UCCP GRAM:        Start: 0xB4000000, Length to be mapped: 0x66CC0	
	*/
	unsigned long wifiT0_mem_addr;/* wifi_T0(UCCP) IO memory addresses    */
	unsigned long wifiT1_mem_addr;/* wifi_T1(wln_cgn) IO memory addresses */
	unsigned char offset;         /* wln_cgn[WLN_CGN_OFFSET] offset value */
};

/******************************* Function Prototypes**************************/

#endif /* _UCCP_H_ */
