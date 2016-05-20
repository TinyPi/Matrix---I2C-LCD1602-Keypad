#include "pwm_gpio_ctrl.h"


#define SUCCESS 0
#define DEVICE_NAME "pwm_dev"
#define BUF_LEN 512

/*
* Is the device open right now? Used to prevent
* concurent access into the same device
*/
static int Device_Open = 0;

/*
* The message the device will give when asked
*/
//static char value[BUF_LEN];
static char *value_xy;

static char *value_x_ptr = NULL;
static char *value_y_ptr = NULL;

static int major_num = 0;
static int minor_num = 0;

static struct class *pwm_class = NULL;

static struct pwm_dev *pwm_dev = NULL;

static ssize_t pwm_gpio_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t pwm_gpio_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static DEVICE_ATTR(gpio, S_IRUGO | S_IWUSR, pwm_gpio_show, pwm_gpio_store);

static ssize_t pwm_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t pwm_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static DEVICE_ATTR(pwm, S_IRUGO | S_IWUSR, pwm_show, pwm_store);


/************** Delay s *******************/
void delay(unsigned int z)
{
  unsigned int x,y;
  for(x=z;x>0;x--)
   for(y=110;y>0;y--);
}
/************** Delay us ******************/
void delay_us(unsigned int aa)
{
  while(aa--);	
}

int atoi(char* pstr)
{
    int intVal = 0;
    int sign = 1;
    if(pstr == 0) return 0;
    while(' '== *pstr) pstr++;
    if('-'==*pstr) sign = -1;
    if('-'==*pstr || '+'==*pstr) pstr++;
    while(*pstr >= '0' && *pstr <= '9') {
      intVal = intVal * 10 + (((int)*pstr)-0x30);
      pstr++;
    }

    return intVal * sign;
}


/************** set gpio *******************/
/*
int gpio_set(const char *gpioNum, const char *level)
{
  FILE *fd;

  fd = fopen(gpioNum, "r+");
  if (fd == NULL) {
    printk("Error : Cannot open GPIOB %s direction. \n", gpioNum);
    return -1;
  }

  fwrite(level, sizeof(level), 1, fd);
  fclose(fd);

  return 0;
}
*/

/*************** gpio show/store**************/
static ssize_t pwm_gpio_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct pwm_dev *hdev = (struct pwm_dev *)dev_get_drvdata(dev);

    //return __pwm_get_gpio(hdev, buf);
    return 0;
}

static ssize_t pwm_gpio_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct pwm_dev *hdev = (struct pwm_dev *)dev_get_drvdata(dev);

    //return __pwm_set_gpio(hdev, buf, count);
    return 0;
}

/*************** pwm show/store **************/
static ssize_t pwm_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct pwm_dev *hdev = (struct pwm_dev *)dev_get_drvdata(dev);

    //return __pwm_get(hdev, buf);
    return 0;
}

static ssize_t pwm_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct pwm_dev *hdev = (struct pwm_dev *)dev_get_drvdata(dev);

    //return __pwm_set(hdev, buf, count);
    return 0;
}

/************** gpio init *******************/

int gpio_init()
{

   nxp_soc_gpio_set_io_dir(GPIOB_26, 1);
   udelay(100);
   nxp_soc_gpio_set_io_dir(GPIOB_27, 1);
   udelay(100);
   nxp_soc_gpio_set_io_dir(GPIOB_30, 1);
   udelay(100);
   nxp_soc_gpio_set_io_dir(GPIOB_31, 1);
   udelay(100);

   nxp_soc_gpio_set_out_value(GPIOB_26, 0);
   udelay(100);
   nxp_soc_gpio_set_out_value(GPIOB_27, 0);
   udelay(100);
   nxp_soc_gpio_set_out_value(GPIOB_30, 0);
   udelay(100);
   nxp_soc_gpio_set_out_value(GPIOB_31, 0);
   udelay(100);

   return 0;
}


//GPIOB26 ---> IN1
//GPIOB27 ---> IN2
//GPIOB30 ---> IN5
//GPIOB31 ---> IN6
//int move(char *x, char *y)
int move(int _x, int _y)
{
    //int rc;
    int Integer_sign_x = 1;
    int Integer_sign_y = 1;
    unsigned int uns_int_x, uns_int_y;

    printk("moving....\n");
    
    printk("[%s]: [x]: %d, [y]: %d \n", __func__, _x, _y);

    if(_x < 0)
      Integer_sign_x = -1;
    if(_y < 0)
      Integer_sign_y = -1;

    printk("[%s]: Integer_sign_x: %d, Integer_sign_y: %d \n", __func__, Integer_sign_x, Integer_sign_y);

    uns_int_x = _x >0 ? _x : -_x;
    uns_int_y = _y >0 ? _y : -_y;


    if (Integer_sign_x == -1 && Integer_sign_y == -1) {//back
       //set_pwm(10000, uns_int_x, fd_pwm0);
       nxp_soc_pwm_set_frequency(PWM0, 10000, uns_int_x);
       //set_pwm(10000, uns_int_y, fd_pwm1);
       nxp_soc_pwm_set_frequency(PWM1, 10000, uns_int_y);
       mdelay(5);
       //gpio_set(GPIO26VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_26, 1);
       udelay(100);
       //gpio_set(GPIO27VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_27, 0);
       udelay(100);
       //gpio_set(GPIO30VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_30, 0);
       udelay(100);
       //gpio_set(GPIO31VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_31, 1);

       mdelay(5);
    } else if (Integer_sign_x == 1 && Integer_sign_y == 1) {//front
       //set_pwm(10000, uns_int_x, fd_pwm0);
       nxp_soc_pwm_set_frequency(PWM0, 10000, uns_int_x);
       //set_pwm(10000, uns_int_y, fd_pwm1);
       nxp_soc_pwm_set_frequency(PWM1, 10000, uns_int_y);
       mdelay(5);
       //gpio_set(GPIO26VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_26, 0);
       udelay(100);
       //gpio_set(GPIO27VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_27, 1);
       udelay(100);
       //gpio_set(GPIO30VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_30, 1);
       udelay(100);
       //gpio_set(GPIO31VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_31, 0);
       mdelay(5);
    } else if (Integer_sign_x == -1 && Integer_sign_y == 1) {//L_F
       //set_pwm(10000, uns_int_x, fd_pwm0);
       nxp_soc_pwm_set_frequency(PWM0, 10000, uns_int_x);
       //set_pwm(10000, uns_int_y, fd_pwm1);
       nxp_soc_pwm_set_frequency(PWM1, 10000, uns_int_y);
       mdelay(5);
       //gpio_set(GPIO26VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_26, 1);
       udelay(100);
       //gpio_set(GPIO27VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_27, 0);
       udelay(100);
       //gpio_set(GPIO30VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_30, 1);
       udelay(100);
       //gpio_set(GPIO31VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_31, 0);
       mdelay(5);
    } else if (Integer_sign_x == 1 && Integer_sign_y == -1) {//R_F
       //set_pwm(10000, uns_int_x, fd_pwm0);
       nxp_soc_pwm_set_frequency(PWM0, 10000, uns_int_x);
       //set_pwm(10000, uns_int_y, fd_pwm1);
       nxp_soc_pwm_set_frequency(PWM1, 10000, uns_int_y);
       mdelay(5);
       //gpio_set(GPIO26VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_26, 0);
       udelay(100);
       //gpio_set(GPIO27VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_27, 1);
       udelay(100);
       //gpio_set(GPIO30VA, "0");
       nxp_soc_gpio_set_out_value(GPIOB_30, 0);
       udelay(100);
       //gpio_set(GPIO31VA, "1");
       nxp_soc_gpio_set_out_value(GPIOB_31, 1);
       mdelay(5);    
    }
  //fclose(fd_pwm0);
  //fclose(fd_pwm1);
      printk("[%s]: move end ........\n", __func__);
  return 0;
}

int split_buff(char *buffer)
{

    //int i = 0;
     char *p = buffer;
     char *key_point;
     int sig_int_x, sig_int_y;

     printk("[%s]: [buffer]:%s\n", __func__, buffer);

     key_point = strsep(&p,",");
     printk("[%s]: [key_point]:%s  [p]:%s\n", __func__, key_point, p);

     value_x_ptr= key_point;
     value_y_ptr= p;
     printk("[%s]: [x]:%s  [y]:%s\n", __func__, value_x_ptr, value_y_ptr);

     sig_int_x = atoi(value_x_ptr);

     sig_int_y = atoi(value_y_ptr);
    
     printk("[%s]: uns_int_x: %d, uns_int_y: %d \n", __func__, sig_int_x, sig_int_y);
     move(sig_int_x, sig_int_y);

   return 0;
}



/*
* This is called whenever a process attempts to open the device file
*/
static int pwm_open(struct inode *inode, struct file *file)
{
        struct pwm_dev *dev;

        printk("[%s]: device_open(%p)\n", __func__, file);

        if (Device_Open) {
          printk("[%s]: pwm devivce has already opened \n", __func__);
          return -EBUSY;
        }
        Device_Open++;

        try_module_get(THIS_MODULE);

        dev = container_of(inode->i_cdev, struct pwm_dev, cdev);

        file->private_data = dev;
        dev->val++;

        return SUCCESS;
}

static int pwm_release(struct inode *inode, struct file *file)
{
        struct pwm_dev *dev;

        printk("[%s]: device_release(%p,%p)\n", __func__, inode, file);
        /*
         * ready for next caller
         */
        Device_Open--;

        dev = container_of(inode->i_cdev, struct pwm_dev, cdev);

        file->private_data = NULL;
        dev->val--;

        module_put(THIS_MODULE);
        return SUCCESS;
}

/*
* This function is called whenever a process which has already opened the
* device file attempts to read from it.
*/
static ssize_t pwm_read(struct file *file,
                           char __user * buffer,
                           size_t length)

{

        int bytes_read = 0,rc;
        char *tmp = NULL;

        printk("[%s]: device_read(%p,%p,%d)\n", __func__, file, buffer, length);

        if (value_x_ptr == NULL || value_y_ptr == NULL) {
        //if (value_x_y == NULL) {
         printk("[%s]: value_x or value_y is NULL, error!\n", __func__);
         return -EFAULT;
        }
        /*
         * Actually put the data into the buffer
         */
        tmp = kmalloc(length, GFP_KERNEL | GFP_DMA);
        if (!tmp) {
        printk("[%s]: kmalloc tmp fail!\n", __func__);
        return -EFAULT;
        }
        printk("[%s]: ([x] %s  [y] %s)!\n", __func__, value_x_ptr, value_y_ptr);
        sprintf(tmp, "%s,%s", value_x_ptr, value_y_ptr);
        bytes_read = strlen(tmp);

        rc = copy_to_user(buffer, tmp, bytes_read);
        if (rc > 0) {
        printk("[%s]: copy to user error, length %d\n", __func__, rc);
        return -EFAULT;
        }

        printk("[%s]: Read %d bytes, %d total\n", __func__, bytes_read, length);

        return bytes_read > length?bytes_read:length;
}

/*
* This function is called when anyone tries to
* write into our device file.
*/
static ssize_t pwm_write(struct file *file,
             const char __user * buffer, size_t length)
{
        int rc;
        //char *ptr;

        if (file == NULL) {
          printk("file is NULL!\n");
          return EINVAL;
        }

        printk("[%s]: device_write([file]:%p, [buffer]:%s, [length]:%d)\n", __func__, file, buffer, length);

        value_xy = kmalloc(length, GFP_KERNEL | GFP_DMA);
        if (value_xy) {
           rc = copy_from_user(value_xy, buffer, length);
           if (rc > 0) {
           printk("[%s]: copy from user error, length %d\n", __func__, rc);
           return -EFAULT;
           }
         split_buff(value_xy);
        } else {
           length = 0;
           kfree(value_xy);
           printk("[%s]: kmalloc value_xy fail!\n", __func__);
        }

      return length;
}


int pwm_ioctl(struct file *file,
              unsigned int cmd,
              long arg)
{
        //int i;
        char argp_x_y[2];
        int _x,_y;

        printk("[%s]: cmd %d, PWM_CMD_MOVE %d \n", __func__, cmd, PWM_CMD_MOVE);

        if (copy_from_user(argp_x_y, arg, sizeof(argp_x_y)) != 0) {
          printk("[%s]: copy from user ERR\n", __func__);
          return -EFAULT;
        }

        /*
         * Switch according to the ioctl called
         */
        switch (cmd) {

         case PWM_CMD_MOVE:
         {
                /*
                 * Receive a pointer to a message (in user space) and set that
                 * to be the device's message. 
                 */

                //value_x_ptr = &argp_x_y[0];
                //value_y_ptr = &argp_x_y[1];
                printk("[%s]: before ([x]:%d, [y]:%d)\n", __func__, argp_x_y[0], argp_x_y[1] );
                _x = argp_x_y[0] > 127?(argp_x_y[0] - 256):argp_x_y[0];
                _y = argp_x_y[1] > 127?(argp_x_y[1] - 256):argp_x_y[1];
                printk("[%s]: after ([x]:%d, [y]:%d)\n", __func__, _x, _y);
                move(_x, _y);
                //move(value_x_ptr, value_y_ptr);
                break;
         }
#if 1
        case PWM_CMD_MAX:
        {
                /*
                 * Give the current message to the calling process -
                 * the parameter we got is a pointer, fill it.
                 */
                //i = pwm_read(file, argp, i);

                /*
                 * Put a zero at the end of the buffer, so it will be
                 * properly terminated
                 */
                //put_user('\0', (argp + i));
                break;
        }
#endif
      }
      printk("[%s] end! \n", __func__);
      return SUCCESS;
}

/* Module Declarations */

/*
* This structure will hold the functions to be called
* when a process does something to the device we
* created. Since a pointer to this structure is kept in
* the devices table, it can't be local to
* init_module. NULL is for unimplemented functions.
*/
struct file_operations pwm_gpio_ops = {
	      .owner = THIS_MODULE,
	      .read = pwm_read,
	      .write = pwm_write,
	      .unlocked_ioctl = pwm_ioctl,
	      .open = pwm_open,
	      .release = pwm_release,        /* a.k.a. close */
};

static int pwm_setup_dev(struct pwm_dev *dev)
{
    int err;
    dev_t devno = MKDEV(major_num, minor_num);

    memset(dev, 0, sizeof(struct pwm_dev));
    cdev_init(&(dev->cdev), &pwm_gpio_ops);

    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops   = &pwm_gpio_ops;
  
    err = cdev_add(&(dev->cdev), devno, 1);
    if (err) {
        return err;
    }

    dev->val = 0;

    return 0;
}
/*
* Initialize the module - Register the character device
*/
int pwm_ctrl_init()
{
        int ret_val, err;
        dev_t dev = 0;  
        struct device *temp = NULL;  

        /*
         * Register the character device (atleast try)
         */
        err = alloc_chrdev_region(&dev, 0, 1, DEVICE_FILE_NAME);
        if (err < 0) {
         printk("Failed to alloc char dev region: %d.\n", err);
         return err;
        }

        major_num = MAJOR(dev);
        minor_num = MINOR(dev);
       
        pwm_dev = kmalloc(sizeof(struct pwm_dev), GFP_KERNEL);
        if (!pwm_dev) {
        err = -ENOMEM;
        printk("Failed to alloc pwm_dev: %d.\n",err);
        return err;
        }

        err = pwm_setup_dev(pwm_dev);
        if (err) {
         printk("Failed to setup dev: %d.\n", err);
         return err;
        }
        pwm_class = class_create(THIS_MODULE, DEVICE_FILE_NAME);
        if (IS_ERR(pwm_class)) {
         err = PTR_ERR(pwm_class);
         printk("Failed to create dev class: %d.\n", err);
         return err;
        }

        temp = device_create(pwm_class, NULL, dev, "%s", DEVICE_FILE_NAME);
        if (IS_ERR(pwm_class)) {
         err = PTR_ERR(pwm_class);
         printk("Failed to create dev device: %d.\n", err);
         return err;
        }
        
        err = device_create_file(temp, &dev_attr_gpio);
        if (err < 0) {
         printk("Failed to create gpio attribute: %d .\n", err);
         return err;
        }

        err = device_create_file(temp, &dev_attr_pwm);
        if (err < 0) {
         printk("Failed to create pwm attribute: %d .\n", err);
         return err;
        }

        ret_val = gpio_init();
        if (ret_val) {
          printk("gpio init fail %d\n", ret_val);
          return ret_val;
        }

        printk("Succedded to initialize dev device.\n");  

        return 0;
}

/*
* Cleanup - unregister the appropriate file from /proc
*/
void pwm_ctrl_exit()
{
      dev_t devno = MKDEV(major_num, minor_num);
     /*
      * gpio free
      */
      gpio_free(GPIOB_26);
      gpio_free(GPIOB_27);
      gpio_free(GPIOB_30);
      gpio_free(GPIOB_31);

     //unregister_chrdev(MAJOR_NUM, DEVICE_NAME);


     //destroy device and class
      if (pwm_class) {
        device_destroy(pwm_class, MKDEV(major_num, minor_num));
        class_destroy(pwm_class);
      }


     // delete cdev and free malloced mem  
      if (pwm_dev) {
        cdev_del(&(pwm_dev->cdev));
        kfree(pwm_dev);
      }

    
      //free device ID
      unregister_chrdev_region(devno, 1);
      printk("[%s] succeed!\n", __func__);
}


module_init(pwm_ctrl_init);
module_exit(pwm_ctrl_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.0");