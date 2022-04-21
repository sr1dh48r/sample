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
*** File Name  : app.c
***
*** File Description:
*** This file implements API's for UCCP device handling. 
***
******************************************************************************
*END**************************************************************************/
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "app.h"

struct uccp *uccp_new(const char *dev_name)
{
	return uccp_new_fd(open(dev_name, O_RDWR));
}

struct uccp *uccp_new_fd(int fd)
{
	struct uccp *uccp_dev;

	if (!(uccp_dev = calloc(1, sizeof(struct uccp))))
		return NULL;

	uccp_dev->m_fd = fd;

	return uccp_dev;
}

void uccp_destroy(struct uccp *uccp_dev)
{
	if (uccp_valid(uccp_dev)) {
		close(uccp_dev->m_fd);
	}

	free(uccp_dev);
} 
 
bool uccp_valid(struct uccp *uccp_dev)
{
	return uccp_dev->m_fd >= 0;
}
