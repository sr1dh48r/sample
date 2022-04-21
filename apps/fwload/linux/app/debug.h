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
*** File Name  : debug.h
***
*** File Description:
*** This file has debug related defines/declarations. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

/******************************* Includes ************************************/

/******************************* Defines *************************************/

/* Set this to zero to disable debug information */
#define DEBUG 0

#if (DEBUG)
#define Error(...) printf(__VA_ARGS__)
#define Info(...) printf(__VA_ARGS__)
#else
#define Error(...)
#define Info(...) 
#endif

#if FILE_DUMP
#define MAX_FILE_NAME_LEN 100
extern char dump_file[MAX_FILE_NAME_LEN];
#endif

#endif /* _DEBUG_H_ */
