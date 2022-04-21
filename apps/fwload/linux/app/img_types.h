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
*** File Name  : img_types.h
***
*** File Description:
*** This file has IMG typedefs for standard data types. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _IMG_TYPES_H_
#define _IMG_TYPES_H_

#define IMG_NULL ((void *)0)

typedef void IMG_VOID;
typedef unsigned char IMG_UINT8, *IMG_PUINT8;
typedef unsigned char IMG_BYTE,	*IMG_PBYTE;
typedef signed char IMG_INT8, *IMG_PINT8;
typedef char IMG_CHAR, *IMG_PCHAR;

typedef unsigned short IMG_UINT16, *IMG_PUINT16;
typedef signed short IMG_INT16,	*IMG_PINT16;

typedef unsigned int IMG_UINT32, *IMG_PUINT32;
typedef signed int IMG_INT32, *IMG_PINT32;

typedef unsigned long long IMG_UINT64, *IMG_PUINT64;
typedef long long IMG_INT64, *IMG_PINT64;
typedef float IMG_FLOAT, *IMG_PFLOAT;
typedef double IMG_DOUBLE, *IMG_PDOUBLE;

typedef int IMG_BOOL, *IMG_PBOOL;

typedef unsigned char IMG_BOOL8, *IMG_PBOOL8;
typedef unsigned short IMG_BOOL16, *IMG_PBOOL16;
typedef unsigned long IMG_BOOL32, *IMG_PBOOL32;

#endif /* _IMG_TYPES_H_ */ 
