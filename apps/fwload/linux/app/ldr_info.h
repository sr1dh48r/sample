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
*** File Name  : ldr_info.h
***
*** File Description:
*** This file implements API's for LDR format handling. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _LDR_INFO_H_
#define _LDR_INFO_H_

/***************************** Boot Device Header ****************************/
typedef struct _bootdevhdr_
{
    IMG_UINT32  DevID;               /* Value used to verify access to boot
                                           device */
    IMG_UINT32  SLCode;              /* Offset to secondary loader code */
#define BOOTDEV_SLCSECURE_BIT   0x80000000
#define BOOTDEV_SLCCRITICAL_BIT 0x40000000

    IMG_UINT32  SLData;              /* Offset to data used by secondary
                                           loader */
    unsigned short PLCtrl;              /* Primary loader control */
#define BOOTDEV_PLCREMAP_BITS   0x00FF
#define BOOTDEV_PLCREMAP_S      0

    unsigned short CRC;                 /* CRC value */

} BOOTDEVHDR, *PBOOTDEVHDR;

/****************************** Primary boot records *************************/

#define PLRECORD_WORDS       16         /* 64-bit WORDS */
#define PLRECORD_BYTES       (PLRECORD_WORDS*8)
#define PLRECORD_TRAIL_BYTES 8
#define PLRECORD_DATA_BYTES  (PLRECORD_BYTES-PLRECORD_TRAIL_BYTES)


#endif /* _LDR_INFO_H_ */
