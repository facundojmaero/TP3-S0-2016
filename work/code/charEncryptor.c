/*
 *  chardev.c: Creates a read-only char device that says how many times
 *  you've read from the dev file
 */

#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>		  /* for put_user */
#include <linux/device.h>         // Header to support the kernel Driver Model

#define  DEVICE_NAME "charEncryptor"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "charEncr"        ///< The device class -- this is a character device driver

/*  
 *  Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "charEncryptor"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */


MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Facundo Maero, Gustavo Gonzalez");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Un encriptador de mensajes simple");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users


/* 
 * Global variables are declared as static, so are global within the file. 
 */

static int majorNumber;		/* majorNumber number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 			* Used to prevent multiple access to device */
static int timesOpened = 0;
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

static struct class*  charEncryptorClass  = NULL; ///< The device-driver class struct pointer
static struct device* charEncryptorDevice = NULL; ///< The device-driver device struct pointer

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
	printk(KERN_INFO "charEncryptor: inicializando modulo\n");

	// Asigno dinamicamente un major number
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
		printk(KERN_ALERT "charEncryptor fallo al intentar registrar un major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "charEncryptor se registro correctamente con el major number %d\n", majorNumber);


	// Register the device class
	charEncryptorClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(charEncryptorClass)){                // Check for error and clean up if there is
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Fallo al registrar la device class\n");
		return PTR_ERR(charEncryptorClass);
	}
	printk(KERN_INFO "charEncryptor: device class registrada correctamente\n");


	// Registro el device driver
	charEncryptorDevice = device_create(charEncryptorClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charEncryptorDevice)){               // Clean up if there is an error
		class_destroy(charEncryptorClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Fallo al crear el device\n");
		return PTR_ERR(charEncryptorDevice);
	}
	printk(KERN_INFO "charEncryptor: device class creada correctamente\n"); // Made it! device was initialized
	return 0;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{

	device_destroy(charEncryptorClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(charEncryptorClass);                          // unregister the device class
	class_destroy(charEncryptorClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
	printk(KERN_INFO "charEncryptor: modulo removido.\n");
}

/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	timesOpened++;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset){
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	static int i;
	sprintf(msg, "%s", buff);   // appending received string with its length

	//encriptacion
    for(i = 0; i < len; i++){
        msg[i] += 1;
    }
    msg_Ptr = msg;
	printk(KERN_ALERT "Mensaje a encriptar recibido.\n");
	return 0;
}
