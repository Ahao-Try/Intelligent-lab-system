#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include <linux/i2c.h>

#define NAME "lzh_at24c02"

struct at24c02{
    int major;
    int minor;
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;

    void *private_data;
};

struct at24c02 at24c02;
u8 buffer[256];

static int at24c02_read_regs(struct at24c02 *dev,u8 reg,void *val,int len){
    int ret = 0;
    struct i2c_client *client = (struct i2c_client*)dev->private_data;
    struct i2c_msg msg[2];

    //读寄存器 先写给设备要读的寄存器地址 再读
    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &reg;
    msg[0].len = 1;

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = val;
    msg[1].len = len;

    ret = i2c_transfer(client->adapter,msg,2);
    
    if(ret == 2){
        ret = 0;
    } else {
        printk("i2c rd failed=%d reg=%06x len=%d\n",ret, reg, len);
        ret = -EREMOTEIO;
    }

    return 0;
}  

static int at24c02_write_regs(struct at24c02 *dev,u8 reg,void *val,int len){
    int ret = 0;
    struct i2c_client* client = (struct i2c_client*)dev->private_data;
    struct i2c_msg msg;

    u8 buf[257];
    buf[0] = reg;
    memcpy(&buf[1],val,len);
    //写只要一个信息就行了，直接给出设备地址和和要写入信息
    msg.addr = client->addr;
    msg.flags = 0;
    msg.buf = buf;
    msg.len = len + 1;

    ret = i2c_transfer(client->adapter,&msg,1);
    if(ret != 1){
        printk("write regs error\r\n");
        return -1;
    }

    return 0;
}

static int at24c02_open (struct inode *inode, struct file *filp){
    filp->private_data = &at24c02;
    return 0;
}

static ssize_t at24c02_read (struct file *filp, char __user *buf, size_t cnt, loff_t *offset){
    struct at24c02 *at24c02 = filp->private_data;
    int ret = 0;

    ret = at24c02_read_regs(at24c02,0x00,buffer,cnt);
    if(ret < 0){
        printk("read error\r\n");
        return ret;
    }

    copy_to_user(buf,buffer,cnt);

    return 0;
}

static ssize_t at24c02_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offset) {
    int ret = 0;
    struct at24c02 *at24c02 = filp->private_data;
    u8 temp[cnt]; // 定义一个足够大的 temp 数组来存储数据
    size_t i = 0;
    
    if (copy_from_user(temp, buf, cnt) != 0) {
        return -EFAULT; // 从用户空间复制数据时出错
    }

    // 将数据以每次 8 个字节的大小写入 AT24C02
    for (i = 0; i < cnt; i += 8) {
        msleep(5);
        size_t chunk_size = min(8, cnt - i); // 计算每次写入的块大小
        ret = at24c02_write_regs(at24c02, i, &temp[i], chunk_size);
        if (ret < 0) {
            return ret; // 写入设备时出错
        }
    }

    return 0; // 返回写入的字节数
}


static int at24c02_release (struct inode *inode, struct file *filp){

    return 0;
}

static const struct file_operations fop = {
    .owner = THIS_MODULE,
    .open = at24c02_open,
    .read = at24c02_read,
    .write = at24c02_write,
    .release = at24c02_release,
};

static int at24c02_probe(struct i2c_client *client, const struct i2c_device_id *id){
    int ret = 0;
    
    if(at24c02.major){
        at24c02.devid = MKDEV(at24c02.major,0);
        ret = register_chrdev_region(at24c02.devid,1,NAME);
        if(ret < 0){
            printk("register chrdev error\r\n");
            goto register_error;
        }
    }
    else{
        ret = alloc_chrdev_region(&at24c02.devid,0,1,NAME);
        if(ret < 0){
            printk("register chrdev error\r\n");
            goto register_error;
        }
    }

    at24c02.cdev.owner = THIS_MODULE;
    ret = cdev_add(&at24c02.cdev,at24c02.devid,1);
    if(ret < 0){
        printk("cdev add error\r\n");
        goto cdev_error;
    }
    cdev_init(&at24c02.cdev,&fop);
    
    at24c02.class = class_create(THIS_MODULE,NAME);
    if(at24c02.class == NULL){
        printk("class error\r\n");
        goto class_error;
    }

    at24c02.device = device_create(at24c02.class,NULL,at24c02.devid,NULL,NAME);
    if(at24c02.device == NULL){
        printk("device error\r\n");
        goto device_error;
    }

    at24c02.private_data = client;

    return 0;
device_error:
    device_destroy(at24c02.class,at24c02.devid);
class_error:
    class_destroy(at24c02.class);
cdev_error:
    cdev_del(&at24c02.cdev);
register_error:
    unregister_chrdev_region(at24c02.devid,1);
return ret;
}

static int at24c02_remove(struct i2c_client *client){

    device_destroy(at24c02.class,at24c02.devid);

    class_destroy(at24c02.class);

    cdev_del(&at24c02.cdev);

    unregister_chrdev_region(at24c02.devid,1);

    return 0;
}

const struct i2c_device_id at24c02_id_table[] = {
    {"lzh-at24c02",0},
    {},
}; 

const struct of_device_id at24c02_match_table[] = {
    {.compatible = "lzh-at24c02"},
    {},
};

static struct i2c_driver at24c02_driver = {
    .probe  = at24c02_probe,
    .remove = at24c02_remove,
    .id_table = at24c02_id_table,
    .driver = {
        .owner = THIS_MODULE,
        .name = NAME,
        .of_match_table = at24c02_match_table,
    },
};

static int __init at24c02_init(void){
    return i2c_register_driver(THIS_MODULE,&at24c02_driver);
}

static void __exit at24c02_exit(void){
    i2c_del_driver(&at24c02_driver);
}

module_init(at24c02_init);
module_exit(at24c02_exit);

MODULE_AUTHOR("LZH");
MODULE_LICENSE("GPL");