#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define MEM_SIZE 0x4000  // driver size
#define GLOBAL_MAJOR 500 
#define MEM_CLEAR 1 //clear signal

MODULE_LICENSE("GPL");
MODULE_AUTHOR("fzj");
MODULE_DESCRIPTION("A new character device driver");

struct mydev {
  struct cdev cdev;
  unsigned char mem[MEM_SIZE];
};

static struct mydev *pmydev;
struct device *device;
struct class *class;

// open driver
static int driver_open(struct inode *inodep, struct file *fp) {
  printk(KERN_INFO "Driver: open\n"); //defineKERN_INFO"<6>"/*提示信息，如驱动程序启动时，打印硬件信息*/ https://www.cnblogs.com/king-77024128/articles/2262023.html
  fp->private_data = pmydev;
  return 0;
}

// release driver
static int driver_release(struct inode *inodep, struct file *fp) {
  printk(KERN_INFO "Driver: release\n");
  return 0;
}

// read from driver
static ssize_t driver_read(struct file *fp, char __user *buf, size_t count, loff_t *offset) { //size_t:__kernel_size_t; ssize_t = signed size_t;
  printk(KERN_INFO "Driver: start read\n");

  int readnum = 0;
  size_t avail = MEM_SIZE - *offset;
  struct mydev *dev = fp->private_data;

  // memory available
  if (count <= avail) {
    if (copy_to_user(buf, dev->mem + *offset, count) != 0) //unsigned long copy_to_user(void *to, const void *from, unsigned long n)
      return -EFAULT;
    *offset += count;
    readnum = count;
  }
  // memory not availabel
  else {
    if (copy_to_user(buf, dev->mem + *offset, avail) != 0)
      return -EFAULT;
    *offset += avail;
    readnum = avail;
  }

  printk(KERN_INFO "Driver: read %u bytes\n", readnum);
  return readnum;
}

// write to driver
static ssize_t driver_write(struct file *fp, const char __user *buf, size_t count, loff_t *offset) {
  printk(KERN_INFO "Driver: start write\n");

  int writenum = 0;
  size_t avail = MEM_SIZE - *offset;
  struct mydev *dev = fp->private_data;
  memset(dev->mem + *offset, 0, avail);

  // memory available
  if (count > avail) {
    if (copy_from_user(dev->mem + *offset, buf, avail) != 0)  //unsigned long copy_from_user(void *to, const void *from, unsigned long n)
      return -EFAULT;
    *offset += avail;
    writenum = avail;
  }
  // memory not available
  else {
    if (copy_from_user(dev->mem + *offset, buf, count) != 0)
      return -EFAULT;
    *offset += count;
    writenum = count;
  }

  printk(KERN_INFO "Driver: write %u bytes\n", writenum);
  return writenum;
}


// clear driver
static long driver_ioctl(struct file *fp, unsigned int cmd, unsigned long arg) {
  printk(KERN_INFO "Driver: start memory clear\n");

  struct mydev *dev = fp->private_data;
  switch (cmd) {
  case MEM_CLEAR:
    memset(dev->mem, 0, MEM_SIZE);
    printk(KERN_INFO "Driver: memory is set to zero\n");
    break;
  default:
    return -EINVAL;
  }
  return 0;
}


// set position
static loff_t driver_llseek(struct file *fp, loff_t offset, int whence) {
  printk(KERN_INFO "Driver: start lseek\n");

  loff_t ret = 0;
  switch (whence) {
  // SEEK_SET
  case 0:
    if (offset < 0 || offset > MEM_SIZE) {
      ret = -EINVAL; //invalid argument 
      break;
    }
    ret = offset;
    break;
  // SEEK_CUR
  case 1:
    if ((fp->f_pos + offset) > MEM_SIZE || (fp->f_pos + offset) < 0) {
      ret = -EINVAL;
      break;
    }
    ret = fp->f_pos + offset;
    break;
  default:
    ret = -EINVAL;
  }

  if (ret < 0)
    return ret;

  fp->f_pos = ret;
  return ret;
}

// set operation pointers
static const struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = driver_open,
  .release = driver_release,
  .read = driver_read,
  .write = driver_write,
  .llseek = driver_llseek,
  .unlocked_ioctl  = driver_ioctl,
};

// initial function for driver
static int __init mydriver_init(void) {
  printk(KERN_INFO "Load module: mydriver\n");

  int ret;
  dev_t devno = MKDEV(GLOBAL_MAJOR, 0);
  ret = register_chrdev_region(devno, 1, "mydriver");
  if (ret < 0) {
    printk(KERN_ALERT "Registering the character device failed with %d\n", ret);
    return ret;
  }

  // alloc memory
  pmydev = kzalloc(sizeof(struct mydev), GFP_KERNEL);
  if (pmydev == NULL) {
    printk(KERN_ALERT "Alloc memory for device failed\n");
    ret = -ENOMEM; //Out of memory
    goto failed;
  }
  memset(pmydev->mem, 0, MEM_SIZE);

  // setup
  cdev_init(&pmydev->cdev, &fops);
  pmydev->cdev.owner = THIS_MODULE;
  cdev_add(&pmydev->cdev, devno, 1);

  // create device file
  class = class_create(THIS_MODULE, "mydriver");
  if (IS_ERR(class)) {
    ret = PTR_ERR(class);
    printk(KERN_ALERT "Creat class for device file failed with %d\n", ret);
    goto failed;
  }
  device = device_create(class, NULL, devno, NULL, "mydriver");
  if (IS_ERR(device)) {
    class_destroy(class);
    ret = PTR_ERR(device);
    printk(KERN_ALERT "Creat device file failed with %d\n", ret);
    goto failed;
  }

  return 0;

 failed:
  unregister_chrdev_region(devno, 1);
  return ret;
}

// exit driver
static void __exit driver_exit(void) {
  printk(KERN_INFO "Unload module: mydriver\n");

  device_destroy(class, MKDEV(GLOBAL_MAJOR, 0));
  class_unregister(class);
  class_destroy(class);

  cdev_del(&pmydev->cdev);
  kfree(pmydev);
  unregister_chrdev_region(MKDEV(GLOBAL_MAJOR, 0), 1);
}

module_init(mydriver_init);
module_exit(driver_exit);
