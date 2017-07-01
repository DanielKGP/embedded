#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>

#define BUFFER_SIZE (1024)

struct led_dev {
  struct cdev cdev;
};

static unsigned int count=0;
static dev_t dev_id;
static unsigned int led_major = 0;
static struct led_dev* led_devs;

static char * buffer;
static unsigned long buf_size;

static int rwbuf_open(struct inode *inode, struct file *filep) {
  if(count == 0 || buffer == NULL) {
    buffer = kmalloc(BUFFER_SIZE,GFP_KERNEL);
    if(buffer == NULL){
      printk("cannot alloc\n");
      return -1;
    }
    printk("alloc\n");
    buf_size=BUFFER_SIZE;
  }
  try_module_get(THIS_MODULE);
  ++ count;
  return 0;
}

// the close
static int rwbuf_close(struct inode *inode, struct file *filep)
{
  --count;
  printk("close %u",count);
  if(count == 0) {
    if(buffer) {
      kfree(buffer);
      printk("free\n");
      buffer = NULL;
    }
  }
  module_put(THIS_MODULE);
  return 0;
}

static ssize_t rwbuf_write(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
  int len = -ENOMEM;
  if(buffer) {
    if (*f_pos < buf_size) {
        len = count + *f_pos > buf_size ? buf_size - *f_pos : count;
        copy_from_user(buffer + *f_pos,buf,len);
    }
    else {
        len = count > buf_size ? buf_size  : count;
        copy_from_user(buffer + *f_pos,buf,len);
    }
    *f_pos += len;
  }
  return len;
}

static ssize_t rwbuf_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
  int len = 0;
  if(buffer) {
    if (*f_pos < buf_size) {
        len = count + *f_pos > buf_size ? buf_size - *f_pos : count;
        copy_to_user(buf,buffer + *f_pos,len);
    }
    *f_pos += len;
  }
  return len;
}


// file operations
static struct file_operations rwbuf_fops = {
  open:    rwbuf_open,
  release: rwbuf_close,
  read:    rwbuf_read,
  write:   rwbuf_write,
};

//// MODULE INIT
static int init_ledc(void) {
  buffer = NULL;
  buf_size = 0;
  count = 0;
  printk("start!\n");
  if(alloc_chrdev_region(&dev_id,0,1,"rwbuf")<0) {
    printk("fail alloc devices\n");
    return -1;
  }
  led_major = MAJOR(dev_id);
  led_devs = (struct led_dev*)kzalloc(sizeof(struct led_dev),GFP_KERNEL);
  if (led_devs == NULL) {
    printk("fail to create devs");
    return -1;
  }
  dev_t devno = MKDEV(led_major,0);
  cdev_init(&led_devs->cdev,&rwbuf_fops);
  led_devs->cdev.owner = THIS_MODULE;
  if(cdev_add(&led_devs->cdev,devno,1)) {
    printk("fail to add to devno");
    return -31;
  }
  return 0;
}

//// MODULE CLEANUP
static void exit_ledc(void) {
  printk("exit!\n");
  cdev_del(&led_devs->cdev);
  unregister_chrdev_region(dev_id,1);
  kfree(led_devs);
  if(buffer) {
    kfree(buffer);
    printk("free");
  }
}

module_init(init_ledc);
module_exit(exit_ledc);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kangkang");
