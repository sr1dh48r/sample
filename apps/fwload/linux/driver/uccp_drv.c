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
*** File Name  : uccp_drv.c
***
*** File Description:
*** This file implements a char driver for the UCCP. 
***
******************************************************************************
*END**************************************************************************/
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

#include "uccp.h"
#include "debug.h"
#include "ldr_api.h"


int thread_started = 0;
static struct uccp_priv priv;
 
int img_uccp_open(struct inode *in, struct file *fp)
{
	img_info("UCCP char device opened\n");
	return 0;
}

int img_uccp_release(struct inode *in, struct file *fp)
{
	img_info("UCCP char device released\n");
	return 0;
}

long img_uccp_ioctl(struct file *fp, unsigned int cmd_id, unsigned long args)
{
        void __user *argp = (void __user *)args;

	switch(cmd_id)
	{
		case UCCP_SOFT_RESET:
			ldr_soft_reset(LTP_THREAD_NO, &priv);
			break;
			
		case UCCP_LOAD_MEMORY:
			{
				struct img_load_mem lm_v;
				unsigned char *lm_buf = NULL;
				unsigned int l_flags = 
					(in_interrupt()?GFP_ATOMIC:GFP_KERNEL);

				if (copy_from_user(&lm_v, argp, sizeof(lm_v)))
					return -EFAULT;
					
				if((lm_v.Length == 0) || 
				   (lm_v.Length > MAX_LOAD_MEM_LEN))
					return -EFAULT;
								
				lm_buf = (unsigned char *)kzalloc(lm_v.Length, 
								  l_flags);

				if(lm_buf == NULL)
					return -ENOMEM;
										
				if (copy_from_user(lm_buf, 
						   lm_v.pSrcBuf, 
						   lm_v.Length)) {
					kfree(lm_buf);
					lm_buf = NULL;
					return -EFAULT;
				}
					
				ldr_load_mem(lm_v.DstAddr, 
					     lm_v.Length, 
					     lm_buf, 
					     &priv);
						 
				kfree(lm_buf);
				lm_buf = NULL;
			}
			break;
			
		case UCCP_START_THREAD:
			{
				struct img_thrd_info tinfo_v;
				if (copy_from_user(&tinfo_v, 
						   argp, 
						   sizeof(tinfo_v)))
					return -EFAULT;

				thread_started = 1;
				
				ldr_start_thread(tinfo_v.ThreadNumber, 
						 tinfo_v.StackPointer, 
						 tinfo_v.ProgramCounter, 
						 tinfo_v.CatchStateAddress,
						 &priv);
			}
			break;
			
		case UCCP_STOP_THREAD:
			{
				ldr_stop_thread(LTP_THREAD_NO, &priv);
				thread_started = 0;				
			}
			break;
			
		case UCCP_ZERO_MEMORY:
			{
				struct img_load_mem zm_v;
				if (copy_from_user(&zm_v, argp, sizeof(zm_v)))
					return -EFAULT;
					
				ldr_zero_mem(zm_v.DstAddr, zm_v.Length, &priv);
			}
			break;
			
		case UCCP_CONFIG_READ:
			{
				struct img_cfg_rw rw_v;
				if (copy_from_user(&rw_v, argp, sizeof(rw_v)))
					return -EFAULT;
					
				rw_v.Val = ldr_config_read(rw_v.Addr, &priv);
				
				if (copy_to_user(argp, &rw_v, sizeof(rw_v)))
					return -EFAULT;				
			}
			break;
			
		case UCCP_CONFIG_WRITE:
			{
				struct img_cfg_rw rw_v;
				if (copy_from_user(&rw_v, argp, sizeof(rw_v)))
					return -EFAULT;
					
				ldr_config_write(rw_v.Addr, rw_v.Val, &priv);
			}
			break;
			
		default:
			{
				img_err("Unknown Command - IOCTL Failed\n");
			}
			return -EINVAL;
			
	}
	
	return 0;
}
 
static const struct file_operations uccp_fops = {
	.open = img_uccp_open,
	.release = img_uccp_release,
	.unlocked_ioctl	= img_uccp_ioctl
};

static int __init uccp_init(void)
{
	dev_t dev = 0;
	int dev_count = 0;
	int error;
	
	img_info("Driver Init\n");
	
	priv.uccp_class = class_create(THIS_MODULE, "uccp");

	if (IS_ERR(priv.uccp_class)) {
		img_err("Unable to create device class\n");
		error = PTR_ERR(priv.uccp_class);
		goto err_class;
	}
	
	priv.uccp_major = register_chrdev(0, "uccp", &uccp_fops);
	
	if (priv.uccp_major < 0) {
		img_err("Unable to get device major number\n");
		error = priv.uccp_major;
		goto err_major;
	}
	
	dev = MKDEV(priv.uccp_major, dev_count);
	
	priv.device = device_create(priv.uccp_class, NULL, dev,
				    NULL, "uccp%d", dev_count);
	
	if (IS_ERR(priv.device)) {
		img_err("Unable to create device uccp%d\n", dev_count);
		error =  PTR_ERR(priv.device);
		goto err_dev;
	}
	
	/* TODO: Create mappings for the UCCP memory regions described in
	the definition of uccp_priv structure and store them in priv */
	
	img_info("UCCP0 Device is created\n");
	
	return 0;

err_dev:
	unregister_chrdev(priv.uccp_major, "uccp");
err_major:
	class_destroy(priv.uccp_class);
err_class:	
	return error;	
}


static void __exit uccp_exit(void)
{
	dev_t dev = MKDEV(priv.uccp_major, 0);
	
	device_destroy(priv.uccp_class, dev);
	unregister_chrdev(priv.uccp_major, "uccp");
	class_destroy(priv.uccp_class);
	
	img_info("Driver Exit\n");
}

module_init(uccp_init);
module_exit(uccp_exit);

MODULE_AUTHOR("Imagination Technologies Ltd.");
MODULE_DESCRIPTION("IMG UCCP Driver");
MODULE_LICENSE("GPL");
