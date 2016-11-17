/**
 * @file   charDesencryptor.c
 * @author Facundo Maero, Gustavo Gonzalez
 * @date   Noviembre 2016
 * @version 0.1
 * @brief Modulo del Kernel que desencripta el mensaje que recibe, y devuelve el mensaje
 *        decodificado al ser leido.
 *
*/


#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>		  /* for put_user */
#include <linux/device.h>         // Header to support the kernel Driver Model

#define  DEVICE_NAME "charDesencryptor"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "charDesencr"        ///< The device class -- this is a character device driver

//Prototipos
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "charDesencryptor"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */


MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Facundo Maero, Gustavo Gonzalez");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Un encriptador de mensajes simple");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users


/* 
 * Variables globales de tipo estatico, solo son visibles en este archivo.
 */

static int majorNumber;		/* majorNumber number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 			* Used to prevent multiple access to device */
static int timesOpened = 0;
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;
static char key[6] = {'F', 'C', 'E', 'F', 'Y', 'N'};

static struct class*  charDesencryptorClass  = NULL; ///< The device-driver class struct pointer
static struct device* charDesencryptorDevice = NULL; ///< The device-driver device struct pointer

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

  /**
  * @brief Funcion llamada cuando se carga el modulo
  *
  */
int init_module(void)
{
	printk(KERN_INFO "charDesencryptor: inicializando modulo\n");

	// Asigno dinamicamente un major number
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
		printk(KERN_ALERT "charDesencryptor fallo al intentar registrar un major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "charDesencryptor se registro correctamente con el major number %d\n", majorNumber);


	// Register the device class
	charDesencryptorClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(charDesencryptorClass)){                // Checkeo errores
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Fallo al registrar la device class\n");
		return PTR_ERR(charDesencryptorClass);
	}
	printk(KERN_INFO "charDesencryptor: device class registrada correctamente\n");


	// Registro el device driver
	charDesencryptorDevice = device_create(charDesencryptorClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charDesencryptorDevice)){               // Checkeo errores
		class_destroy(charDesencryptorClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Fallo al crear el device\n");
		return PTR_ERR(charDesencryptorDevice);
	}
	printk(KERN_INFO "charDesencryptor: device class creada correctamente\n"); // Made it! device was initialized
	return 0;
}

  /**
  * @brief Funcion llamada cuando se descarga el modulo
  *
  */
void cleanup_module(void)
{

	device_destroy(charDesencryptorClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(charDesencryptorClass);                          // unregister the device class
	class_destroy(charDesencryptorClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
	printk(KERN_INFO "charDesencryptor: modulo removido.\n");
}

//Metodos

  /**
  * @brief Llamado cuando un proceso trata de abrir el device file
  * Por ejemplo, usando "cat /dev/charEncryptor"
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

  /**
  * @brief Llamado cuando un proceso libera el device file
  * 
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

  /**
  * @brief Llamado cuando un proceso, que ya abrio el dev file, trata de leerlo.
  */
static ssize_t device_read(struct file *filp,
						   char *buffer,	// buffer a llenar con datos
						   size_t length,	// longitud del buffer
						   loff_t * offset){
	
	int error_count = 0;
   	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
   	error_count = copy_to_user(buffer, msg, length);

   	return error_count;
}

  /**
  * @brief Llamado cuando un proceso escribe en el dev file
  */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	static int i;
	sprintf(msg, "%s", buff);   // Guardo el mensaje recibido (en buff), en el string msg

	//Encripto el mensaje
    for (i = 0; i < len; i++){
        msg[i] = msg[i] ^ key[i % (sizeof(key))];
    }

    //Hago que el puntero apunte al mensaje para cuando lo lea
    msg_Ptr = msg;
	printk(KERN_ALERT "Mensaje a encriptar recibido.\n");
	return 0;
}
