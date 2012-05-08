#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for put_user */
#include "angle.h"

MODULE_AUTHOR("Filipe Roque");
MODULE_LICENSE("GPL");

/*
 *  Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t , loff_t * );

#define SUCCESS 0
/* Dev name as it appears in /proc/devices   */
#define DEVICE_NAME "anglemod"	
#define MODULE_NAME DEVICE_NAME
/* Max length of the message from the device */
#define BUF_LEN 80		

/*
 * Global variables are declared as static, so are global within the file.
 */

static int Major;		/* Major number assigned to our device driver */

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void){
    Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}
	printk(KERN_INFO "%s module inserted\n", MODULE_NAME);
	return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void){
	unregister_chrdev(Major, DEVICE_NAME);
	printk(KERN_INFO "%s module removed\n", MODULE_NAME);
}


static int device_open(struct inode *inode, struct file *filp){

	// script .sh define beta as minor number 0
	if (iminor(inode)==0){
		init_bta(ISAADDRESS_BETA);
	}	else{
		init_bta(ISAADDRESS_ALPHA);
	}

	try_module_get(THIS_MODULE);

	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *filp){
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *filp, 
                           char *buffer, 
                           size_t length,
                           loff_t * offset){
	int angle;
	if ( iminor(filp->f_dentry->d_inode) == 0 ) {
		angle = read_bta(ISAADDRESS_BETA);
	} else {
		angle = read_bta(ISAADDRESS_ALPHA);
	}

// for debug purposes
//	printk(KERN_ALERT "length %d\n", length);

	put_user(angle, (int*)buffer);
	return sizeof(int);
}

static ssize_t device_write(struct file *filp, 
                            const char *buffer, 
                            size_t length, 
                            loff_t * off){
	int angle;
	get_user(angle, (int*) buffer);

	if ( iminor(filp->f_dentry->d_inode) == 0 ) {
		write_bta(ISAADDRESS_BETA,angle);
	}	else {
		write_bta(ISAADDRESS_ALPHA,angle);
	}
	return sizeof(int);
}
