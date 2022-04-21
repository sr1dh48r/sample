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
*** File Name  : ldr_parser.h
***
*** File Description:
*** This file has defines/declarations related to LDR parsing. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _LDR_PARSER_H_
#define _LDR_PARSER_H_

/******************************* Includes ************************************/
#include "app.h"

/******************************* Defines *************************************/
#define SUCCESS 0
#define FAILED  1

/******************************************************************************
 L1 / L2 Secondary Loader Records

 See the META Programmers Guide for infomation on L1 & L2 records.
******************************************************************************/

/******************************************************************************
 These constants are used to access various fields within an L1 record as well
 as other constants that are used.
******************************************************************************/

/*! The maximum number of bytes in an L1 record. */
#define L1_MAXSIZE			32

/*! The maximum number of bytes in an L2 record. */
#define L2_MAXSIZE			4096

/*! The size in bytes of the 'cmd' field in an L1 Record. */
#define L1_CMD_SIZE			2

/*! The size in bytes of the 'length' field in an L1 Record. */
#define L1_LENGTH_SIZE		2

/*! The size in bytes of the 'next' field in an L1 Record. */
#define L1_NEXT_SIZE		4

/*! The size in bytes of the 'arg1' field in an L1 Record. */
#define L1_ARG1_SIZE		4

/*! The size in bytes of the 'arg2' field in an L1 Record. */
#define L1_ARG2_SIZE		4

/*! The size in bytes of the 'l2offset' field in an L1 Record. */
#define L1_L2OFFSET_SIZE	4

/*! The size in bytes of the 'xsum' field in an L1 Record. */
#define L1_XSUM_SIZE		2

/*! The offset in bytes of the 'cmd' field in an L1 record. */
#define L1_CMD				0

/*! The offset in bytes of the 'length' field in an L1 record. */
#define L1_LENGTH			(L1_CMD       + L1_CMD_SIZE)

/*! The offset in bytes of the 'next' field in an L1 record. */
#define L1_NEXT				(L1_LENGTH    + L1_LENGTH_SIZE)

/*! The offset in bytes of the 'arg1' field in an L1 record. */
#define L1_ARG1				(L1_NEXT      + L1_NEXT_SIZE)

/*! The offset in bytes of the 'arg2' field in an L1 record. */
#define L1_ARG2				(L1_ARG1      + L1_ARG1_SIZE)

/*! The following is the value used to terminate a chain of L1 records */
#define L1_TERMINATE		0xFFFFFFFF

/******************************************************************************
 These constants are used to access various fields within an L1 record as well
 as other constants that are used.
******************************************************************************/

/*! The size in bytes of the 'cmd' field in an L2 Record. */
#define L2_CMD_SIZE			2

/*! The size in bytes of the 'length' field in an L2 Record. */
#define L2_LENGTH_SIZE		2

/*! The size in bytes of the 'xsum' field in an L2 Record. */
#define L2_XSUM_SIZE		2

/*! The offset in bytes from the beginning of an L2 record to the data
    payload */
#define L2_DATA				(L2_CMD_SIZE  + L2_LENGTH_SIZE)

/******************************************************************************
 Various combined values...
******************************************************************************/

/*! Sizes of common items between L1 and L2 records */
#define L1_L2LENGTH_SIZE	L2_LENGTH_SIZE

/*! The size in bytes of an L1 record when it contains no data */
#define L1_BASIC_SIZE	    (L1_CMD_SIZE      + \
                             L1_LENGTH_SIZE   + \
			     L1_NEXT_SIZE     + \
			     L1_L2OFFSET_SIZE + \
			     L1_L2LENGTH_SIZE + \
			     L1_XSUM_SIZE)

/*! The size in bytes of an L2 record when it contains no data */
#define L2_BASIC_SIZE	    (L2_CMD_SIZE      + \
			     L2_LENGTH_SIZE   + \
			     L2_XSUM_SIZE)


/* Offsets in bytes from the end of an L1 record for various fields */
#define L1_L2LENGTH			(L1_XSUM_SIZE + L1_L2LENGTH_SIZE)
#define L1_L2OFFSET			(L1_L2LENGTH  + L1_L2OFFSET_SIZE)

/******************************* Typedefs ************************************/

/*!****************************************************************************
 This typedef enumerates various Cmd or Tag values used in the L1/L2 records.
******************************************************************************/

typedef enum
{
	/*! Command - L1 LoadMem. */
	e_CTLoadMem     = 0x0000,

	/*! Command - L1 StartThrds. */
	e_CTStartThrds  = 0x0003,

	/*! Command - L1 ZeroMem. */
	e_CTZeroMem     = 0x0004,

	/*! Command - L1 Config. */
	e_CTConfig      = 0x0005,
	
	/*! Command - L1 FileName. */
	e_CTFileName    = 0x0010,

} t_CmdTag;


/*!****************************************************************************
 This typedef enumerates all possible types of configuration commands.
******************************************************************************/

typedef enum
{
	/*! Pause. */
	e_CIPause = 0x0000,
	
	/*! Read. */
	e_CIRead,

	/*! Write. */
	e_CIWrite,

	/*! MemSet. */
	e_CIMemSet,

	/*! MemChk. */
	e_CIMemChk,

	/*! User. */
	e_CIUser,

} t_ConfigInst;

/******************************* Function Prototypes**************************/
int LdrParser(FILE *pLdrFile, struct uccp *uccp_dev);
#endif /* _LDR_PARSER_H_ */
