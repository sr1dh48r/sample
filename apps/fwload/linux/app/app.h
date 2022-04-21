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
*** File Name  : app.h
***
*** File Description:
*** This file has declarations and definitions UCCP device handling API's etc. 
***
******************************************************************************
*END**************************************************************************/
#ifndef _APP_H_
#define _APP_H_

/******************************* Includes ************************************/
#include <stdbool.h>

/******************************* Defines *************************************/
#define UCCP_GRAM_BASE 0xB7000000
#define UCCP_THRD_EXEC_SIG_OFFSET 0x00066CBC 
#define UCCP_THRD_EXEC_SIG 0x5A5A5A5A

/******************************* Variables ***********************************/
struct uccp
{
	int m_fd;	
};

/******************************* Function Prototypes**************************/
struct uccp *uccp_new(const char *dev);
struct uccp *uccp_new_fd(int fd);
bool uccp_valid(struct uccp *dev);
void uccp_destroy(struct uccp *dev);

#endif /* _APP_H_ */
