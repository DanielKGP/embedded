#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include<linux/seq_file.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
#include<linux/string.h>
#include<linux/moduleparam.h>


#define BUF_SIZE (1024)

static unsigned long buffer_size = 0;

///// Datas
static char *buffer;
struct proc_dir_entty *qled;
static unsigned int count=0;


// proc open
static int qled_open(struct inode* inode, struct file *file) {
  printk("open %u\n",count);
  if(count ==0 || buffer == NULL) {
    if(buffer_size == 0) {
      buffer_size = BUF_SIZE;
    }
    buffer = kmalloc(buffer_size,GFP_KERNEL);
    if(buffer == NULL) {
      printk("cannot alloc");
      return -1;
    }
    printk("alloc\n");
  }
  try_module_get(THIS_MODULE);
  ++ count;
  return 0;
}


// proc close
static int qled_close(struct inode *inode, struct file *filep)
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

static ssize_t qled_write(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
  int len = -ENOMEM;
  if(buffer) {
    if (*f_pos < buffer_size) {
        len = count + *f_pos > buffer_size ? buffer_size - *f_pos : count;
        copy_from_user(buffer + *f_pos,buf,len);
    }
    else {
        len = count > buffer_size ? buffer_size  : count;
        copy_from_user(buffer + *f_pos,buf,len);
    }
    *f_pos += len;
  }
  return len;
}

static ssize_t qled_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
  int len = 0;
  if(buffer) {
    if (*f_pos < buffer_size) {
        len = count + *f_pos > buffer_size ? buffer_size - *f_pos : count;
        copy_to_user(buf,buffer + *f_pos,len);
    }
    *f_pos += len;
  }
  return len;
}


// file operations
static struct file_operations qled_fops = {
  open:    qled_open,
  release: qled_close,
  read:    qled_read,
  write:   qled_write,
};

//// MODULE INIT
static int init_ledc(void) {
  printk("start!\n");
  qled = proc_create_data("proc",0644,NULL,&qled_fops,"123");
  return 0;
}

//// MODULE CLEANUP
static void exit_ledc(void) {
  printk("exit!\n");
  remove_proc_entry("proc",NULL);
  if(buffer) {
    kfree(buffer);
    printk("free");
  }
}

module_init(init_ledc);
module_exit(exit_ledc);

module_param(buffer_size,ulong,S_IRUGO);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kangkang");
