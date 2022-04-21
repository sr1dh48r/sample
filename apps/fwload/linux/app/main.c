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
*** File Name  : main.c
***
*** File Description:
*** This file is the entry point for the FW loader application.
***
******************************************************************************
*END**************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "app.h"
#include "ldr_parser.h"
#include "debug.h"

#include "uccp_drv.h"

#define DEV_NODE "/dev/uccp0"

#if FILE_DUMP
char dump_file[MAX_FILE_NAME_LEN];
#endif


static bool uccpld(const char *dev_name, FILE *ldr)
{
	struct uccp *uccp_dev = NULL;
	struct img_cfg_rw rw_v;
	bool result = true;
	
	uccp_dev = uccp_new(dev_name);

	if (!uccp_dev) {
		fprintf(stderr, "Unable to create UCCP device\n");
		return false;
	}

	if (!uccp_valid(uccp_dev)) {
		fprintf(stderr, "Invalid UCCP device\n");
		result = false;
		goto out;
	}
		
	if(ioctl(uccp_dev->m_fd, UCCP_SOFT_RESET)) {
		result = false;
		goto out;
	}
	
	rw_v.Addr = UCCP_GRAM_BASE + UCCP_THRD_EXEC_SIG_OFFSET;
	rw_v.Val = 0x00;

	if(ioctl(uccp_dev->m_fd, UCCP_CONFIG_WRITE, &rw_v)) {
		result = false;
		goto out;
	}
	
	if (FAILED == LdrParser(ldr, uccp_dev)) {
		fprintf(stderr, "Loading LDR file failed\n");
		result = false;
		goto out;
	}
	
out:
	if (uccp_dev)
		uccp_destroy(uccp_dev);
	
	return result;
}

static bool uccp_wait_for_completion(const char *dev_name)
{
	struct uccp *uccp_dev = NULL;
	struct img_cfg_rw rw_v;
	bool result = true;
	unsigned int i = 0;
	
	uccp_dev = uccp_new(dev_name);

	if (!uccp_dev) {
		fprintf(stderr, "Unable to create UCCP device\n");
		return false;
	}

	if (!uccp_valid(uccp_dev)) {
		fprintf(stderr, "Invalid UCCP device\n");
		result = false;
		goto out;
	}
	
#if UCCP420_HW	
	{
		struct img_cfg_rw rw_v;
		rw_v.Addr = UCCP_GRAM_BASE + UCCP_THRD_EXEC_SIG_OFFSET;

		do {
			ioctl(uccp_dev->m_fd, UCCP_CONFIG_READ, &rw_v);

			/* Sleep for 10 ms */
			usleep(10 * 1000); 
			
			i++;

		}while ((UCCP_THRD_EXEC_SIG != rw_v.Val) && (i < 1000));
	}

#endif
	if (i == 1000) {
		fprintf(stderr, "Timed out waiting for Program to execute\n");
		result = false;
	}
	
	rw_v.Addr = UCCP_GRAM_BASE + UCCP_THRD_EXEC_SIG_OFFSET;
	rw_v.Val = 0x00;

	if(ioctl(uccp_dev->m_fd, UCCP_CONFIG_WRITE, &rw_v)) {
		fprintf(stderr, "Reset of Thread Exec Signature "
			"value failed\n");
		result = false;
	}
	
out:
	uccp_destroy(uccp_dev);
	
	return result;
}

static void uccp_stop_thrd(const char *dev_name)
{
	struct uccp *uccp_dev = NULL;
	
	uccp_dev = uccp_new(dev_name);

	if (!uccp_dev) {
		fprintf(stderr, "Unable to create UCCP device\n");
		return;
	}

	if (!uccp_valid(uccp_dev)) {
		fprintf(stderr, "Invalid UCCP device\n");
		goto out;
	}

	ioctl(uccp_dev->m_fd, UCCP_STOP_THREAD);	

out:
	uccp_destroy(uccp_dev);
	
}

static void print_usage()
{
	fprintf(stdout, "uccpld loads LDR image(s) onto the UCCP\n"
		"\n"
		"Usage:\n"
		"\t uccpld [<options>] <LDR1> [<LDR2>]\n"
		"\n"
		"where\n"
		"\t<LDR1> - first LDR image to be loaded "
		"\n"
		"\t<LDR2> - second LDR image to be loaded (optional)"
		"\n\n"
		"\t<options> can be the following:\n"
		"\t\t[-h|--help]  Show this usage information\n"		
		);
}

 
int main(int argc, const char **argv)
{
	const char *dev_name = DEV_NODE;
	FILE *ldr_file1 = NULL;
	FILE *ldr_file2 = NULL;
	
	if ((argc > 3) || (argc == 1)) {
		fprintf(stderr, "Incorrect no.of arguments specified\n");
		print_usage();
		return 1;
	}
	
	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		print_usage();
		return 0;
	}	

	ldr_file1 = fopen(argv[1], "rb");

	if (!ldr_file1) {
		fprintf(stderr, "Failed to open LDR1 (%s)\n", argv[1]);
		goto err;
	}
	
	if (argc == 3) {
		ldr_file2 = fopen(argv[2], "rb");

		if (!ldr_file2) {
			fprintf(stderr, "Failed to open LDR2 (%s)\n", argv[2]);
			goto err;
		}
	}		
	
#if FILE_DUMP
	strcpy(dump_file, "LDR1.dump");
#endif
	
	if(!uccpld(dev_name, ldr_file1)) {
		fprintf(stderr, "Failed to load LDR1 (%s)\n", argv[1]);
		goto err;
	}

	fprintf(stdout, "LDR1 (%s) is loaded\n", argv[1]);

	fclose(ldr_file1);
	ldr_file1 = NULL;
	
	fprintf(stdout, "Waiting for LDR1 (%s) to execute\n", argv[1]);

	if (uccp_wait_for_completion(dev_name) == false) {	
		fprintf(stdout, "uccp_wait_for_completion failed for LDR1 (%s)\n",
				argv[1]);
		goto err;
	}

	fprintf(stdout, "LDR1 (%s) has executed\n", argv[1]);

	if(ldr_file2) {
		/* Stop any previous threads running */
		uccp_stop_thrd(dev_name);

#if FILE_DUMP
		strcpy(dump_file, "LDR2.dump");
#endif
		if (!uccpld(dev_name, ldr_file2)) {
			fprintf(stderr, "Failed to load ldr2 (%s)\n", argv[2]);
			goto err;
		}

		fprintf(stdout, "LDR2 (%s) is loaded\n", argv[2]);

		fclose(ldr_file2);
		ldr_file2 = NULL;

		fprintf(stdout, "Waiting for LDR2 (%s) to execute\n", argv[2]);

		if (uccp_wait_for_completion(dev_name) == false) {	
			fprintf(stdout, "uccp_wait_for_completion failed for LDR2 (%s)\n",
					argv[2]);
			goto err;
		}

		fprintf(stdout, "LDR2 (%s) has executed\n", argv[2]);
	}		

	return 0;

err:
	if(ldr_file1)
		fclose(ldr_file1);

	if(ldr_file2)
		fclose(ldr_file2);

	return 1;
}
