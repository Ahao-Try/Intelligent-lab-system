#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/mutex.h>

#define NAME "lzh_ds18b20"

#define DS18B20_IOCTL_TYPE 'D'
#define DS18B20_CMD_SET_TEM _IO(DS18B20_IOCTL_TYPE,1)
#define DS18B20_CMD_GET_TEM _IO(DS18B20_IOCTL_TYPE,2)
#define DS18B20_CMD_GET_NUM _IO(DS18B20_IOCTL_TYPE,3)

unsigned char data[9];

struct gpio_node{
    struct device_node *node; 
    int gpio_num;
};
struct ds18b20_dev{
    int major;
    int minor;
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct gpio_node gpio_node;
    struct mutex lock;
};

struct ds18b20_dev lzh_ds18b20 = {};

//设置输入输出模式，ds18b20要进行通信，输入和输出都要
static void set_out_mode(int value){
    
    gpio_direction_output(lzh_ds18b20.gpio_node.gpio_num,value);
}

static void set_in_mode(int value){
    gpio_direction_input(lzh_ds18b20.gpio_node.gpio_num);
    gpio_set_value(lzh_ds18b20.gpio_node.gpio_num,value);
}

//获取当前io口值
static unsigned char get_gpio_value(void){
    return gpio_get_value(lzh_ds18b20.gpio_node.gpio_num);
}

// //校验
uint8_t crc8(const uint8_t *data, uint8_t len) {
    // uint8_t crc = 0;
    // uint8_t i, j;
    
    // for (i = 0; i < len; ++i) {
    //     crc ^= data[i];
    //     for (j = 0; j < 8; ++j) {
    //         if (crc & 0x01)
    //             crc = (crc >> 1) ^ 0x8C;
    //         else
    //             crc >>= 1;
    //     }
    // }
    
    // return crc;
     uint8_t crc = 0, inbyte, i, mix;

    while (len--)
    {
        // inbyte 存储当前参与计算的新字节
        inbyte = *data++;

        for (i = 8; i; i--) 
        {
            // 将新字节与CRC从低位到高位, 依次做异或运算, 每次运算完CRC右移一位
            // 如果运算结果值为1, 则将CRC与 1000 1100 作异或
            // 第3,4位代表流程图中的异或运算, 第7位其实就是运算结果移入的1
            mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) 
            {
                crc ^= 0x8C;
            }
            inbyte >>= 1;
        }
    }
    return crc;
}

//初始化
static int ds18b20_init(void){
    //1.初始设置输出高
    set_out_mode(1);

    //2.拉低输入电平 >480us
    set_out_mode(0);
    udelay(500);

    //3.释放总线，拉高电平
    set_out_mode(1);

    //4.初始化输入设置高
    set_in_mode(1);

    //5.等待15-60us
    udelay(50);
    
    int ret = get_gpio_value();

    //6.等待60-240us会自动释放总线
    udelay(500);

    return ret;//0表示成功  1表示失败
}

//读一个比特数据
static unsigned char ds18b20_read_bit(void){
    unsigned char bit = 0;
    //读时段通过主设备将总线拉低超过1us再释放总线来实现初始化
    set_out_mode(0);
    udelay(1);
    set_out_mode(1);
    udelay(1);

    //初始化输入先给他一个高
    set_in_mode(1);

    bit = get_gpio_value();
    return bit;
}

//读一个字节数据
static unsigned char ds18b20_read_byte(void){
    unsigned char byte = 0x00;

    int i = 0;
    for(;i < 8;i++){
        byte >>= 1;
        if(ds18b20_read_bit()){
            byte |= 0x80;
        }
        udelay(60);//每个读时段最小必须有60us的持续时间且独立的写时段间至少有1us的恢复时间
    }
    return byte;
}

//写一个比特
static unsigned char ds18b20_write_bit(unsigned char bit){
    if(bit){
        //为了形成写1时段，在将1-Wire总线拉低后，主设备必须在15us之内释放总线
        set_out_mode(0);
        udelay(8);
        set_out_mode(1);
        udelay(55);
    }
    else{
        //为了形成写0时段，在将1-Wire总线拉低后，在整个时段期间主设备必须一直拉低总线（至少60us）
        set_out_mode(0);
        udelay(55);
        set_out_mode(1);
        udelay(8);
    }
    return bit;
}

//写一个字节数据
static unsigned char ds18b20_write_byte(unsigned char byte){
    int i = 0;
    for(;i < 8;i++){
        ds18b20_write_bit(byte & 0x01);
        byte >>= 1;
    }
    return byte;
}

static void is_same(void);
//进行读
static int ds18b20_read_to_buf(void){
    int ret = 0;
    int i = 0;

    ret = ds18b20_init();
    if(ret != 0)return -1;

    //写如0xCC跳过ROM
    ds18b20_write_byte(0xCC);

    //执行温度转换,写入0x44
    ds18b20_write_byte(0x44);
    mdelay(700);//转换时间最长500ms

    //读温度
    ret = ds18b20_init();
    if(ret != 0)return -1;

    ds18b20_write_byte(0xCC);//跳过rom

    ds18b20_write_byte(0xBE);//写入0XBE，实行RAM指令读

    // data[0] = ds18b20_read_byte();
    // data[1] = ds18b20_read_byte();

    for(;i < 9;i++){
        data[i] = ds18b20_read_byte();
    }
    is_same();
    return 0;
}

//crc校验
static void is_same(void){
    unsigned char crc;
    crc = crc8(data,8);
    while(crc != data[8]){
        ds18b20_read_to_buf();
        crc = crc8(data,8);
    }
}

static int de18b20_open (struct inode *inode, struct file *filp){
    filp->private_data = &lzh_ds18b20;
    return 0;
}

static ssize_t ds18b20_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offset) {
    int ret = ds18b20_read_to_buf();
    if (ret != 0) {
        return -1;
    }

    if (!access_ok(VERIFY_WRITE, buf, sizeof(data))) {
        // 用户空间地址无效，处理错误并返回错误码
        return -EFAULT;
    }

    // 将 double 类型的数据复制到用户空间缓冲区
    if (copy_to_user(buf, &data, sizeof(data)) != 0) {
        // 复制失败，返回错误码
        return -EFAULT;
    }

    return 0;
}


static ssize_t ds18b20_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *offest){
    
    return 0;
}

static long ds18b20_ioctl (struct file *filp, unsigned int cmd, unsigned long arg){
    int ret = 0;
    unsigned char writeArg[3] = {};
    unsigned char returnUser[2];
    unsigned char name[8];
    unsigned long flags;
    
    local_irq_save(flags);
    mutex_lock(&lzh_ds18b20.lock);

    switch(cmd){
        case DS18B20_CMD_SET_TEM:{
            ret = ds18b20_init();  

            if(0 != ret)
            {
                //printk(KERN_ERR"%s ds18b20_init error.\n",__func__);
                return -1;    
            }

            ds18b20_write_byte(0x33); //Read ROM command
            ds18b20_write_byte(0x4E);
            copy_from_user(writeArg,(void*)arg,sizeof(arg));
        }
        break;
        case DS18B20_CMD_GET_TEM:{
            ret = ds18b20_read_to_buf();
            if (ret != 0) {
                return -1;
            }
            returnUser[0] = data[0];
            returnUser[1] = data[1];
            if (!access_ok(VERIFY_WRITE, (void*)arg, sizeof(returnUser))) {
                // 用户空间地址无效，处理错误并返回错误码
                return -EFAULT;
            }

            // 将 double 类型的数据复制到用户空间缓冲区
            if (copy_to_user((void*)arg, &returnUser, sizeof(returnUser)) != 0) {
                // 复制失败，返回错误码
                return -EFAULT;
            }
        }
        break;
        case DS18B20_CMD_GET_NUM:{
            ret = ds18b20_init();  
            int i = 0;
            if(0 != ret)
            {
                return -1;    
            }

            ds18b20_write_byte(0X33);
        
            name[0] = ds18b20_read_byte();
            for (;i<6; i++) 
                name[i + 1] = ds18b20_read_byte();
            name[7] = ds18b20_read_byte();

             if (!access_ok(VERIFY_WRITE, (void*)arg, sizeof(name))) {
                // 用户空间地址无效，处理错误并返回错误码
                return -EFAULT;
            }
            if (copy_to_user((void*)arg, &name, sizeof(name)) != 0) {
                // 复制失败，返回错误码
                return -EFAULT;
            }
        }
        default:
            break;
    }

    mutex_unlock(&lzh_ds18b20.lock);
    local_irq_restore(flags);
    return 0;
}

static int ds18b20_release (struct inode *inode, struct file *filp){
    return 0;
}

const struct file_operations ds18b20_fops = {
    .owner = THIS_MODULE,
    .open = de18b20_open,
    .read = ds18b20_read,
    .write = ds18b20_write,
    .unlocked_ioctl = ds18b20_ioctl,
    .release = ds18b20_release,
};

static int get_gpio_num(void){
    lzh_ds18b20.gpio_node.node = of_find_node_by_name(NULL,"lzh_ds18b20");
    if(lzh_ds18b20.gpio_node.node == NULL){
        printk("find_node error\r\n");
        return -1;
    }

    lzh_ds18b20.gpio_node.gpio_num = of_get_named_gpio(lzh_ds18b20.gpio_node.node,"ds18b20-gpios",0);
    if(lzh_ds18b20.gpio_node.gpio_num < 0){
        printk("find gpio num error\r\n");
        gpio_free(lzh_ds18b20.gpio_node.gpio_num);
        return -1;
    }

    if(gpio_request(lzh_ds18b20.gpio_node.gpio_num,NAME) < 0){
        printk("request error\r\n");
        gpio_free(lzh_ds18b20.gpio_node.gpio_num);
        return -1;
    }

    if(gpio_direction_output(lzh_ds18b20.gpio_node.gpio_num,1) < 0){
        printk("direction error\r\n");
        gpio_free(lzh_ds18b20.gpio_node.gpio_num);
        return -1;
    }
    gpio_set_value(lzh_ds18b20.gpio_node.gpio_num,1);

    return 0;
}

//注册设备
static int register_device_ds18b20(void){
    int ret = 0;
    lzh_ds18b20.major = 200;
    if(lzh_ds18b20.major){
        lzh_ds18b20.devid = MKDEV(lzh_ds18b20.major,0);
        ret = register_chrdev_region(lzh_ds18b20.devid,1,NAME);
        if(ret < 0){
            printk("register error\r\n");
            goto register_error;
        }
    }
    else {
        ret = alloc_chrdev_region(&lzh_ds18b20.devid,0,1,NAME);
        if(ret < 0){
            printk("register error\r\n");
            goto register_error;
        }
    }

    lzh_ds18b20.cdev.owner = THIS_MODULE;
    ret = cdev_add(&lzh_ds18b20.cdev,lzh_ds18b20.devid,1);
    if(ret < 0){
        printk("cdev error\r\n");
        goto cdev_error;
    }
    cdev_init(&lzh_ds18b20.cdev,&ds18b20_fops);
    
    lzh_ds18b20.class = class_create(THIS_MODULE,NAME);
    if(lzh_ds18b20.class == NULL){
        printk("class error\r\n");
        goto class_error;
    }

    lzh_ds18b20.device = device_create(lzh_ds18b20.class,NULL,lzh_ds18b20.devid,NULL,NAME);
    if(lzh_ds18b20.device == NULL){
        printk("device error\r\n");
        goto device_error;
    }
    ds18b20_init();
    return 0;
device_error:
    device_destroy(lzh_ds18b20.class,lzh_ds18b20.devid);
class_error:
    class_destroy(lzh_ds18b20.class);
cdev_error:
    cdev_del(&lzh_ds18b20.cdev);
register_error:
    unregister_chrdev_region(lzh_ds18b20.devid,1);

    return ret;
}

static int ds18b20_probe(struct platform_device *dev){
    int ret = 0;

    //注册设备
    ret = register_device_ds18b20();
    if(ret < 0){
        printk("register_device error\r\n");
        return -1;
    }

    //获取gpio number
    ret = get_gpio_num();
    if(ret < 0){
        printk("get gpio num error\r\n");
        return -1;
    }

    mutex_init(&lzh_ds18b20.lock);

    return 0;
}

static int ds18b20_remove(struct platform_device *dev){
    gpio_set_value(lzh_ds18b20.gpio_node.gpio_num,1);
    
    gpio_free(lzh_ds18b20.gpio_node.gpio_num);

    device_destroy(lzh_ds18b20.class,lzh_ds18b20.devid);

    class_destroy(lzh_ds18b20.class);

    cdev_del(&lzh_ds18b20.cdev);

    unregister_chrdev_region(lzh_ds18b20.devid,1);
    return 0;
}

const struct platform_device_id ds18b20_id_table[] = {
    {"lzh-ds18b20-gpio",0},
    {}
};

//匹配
const struct of_device_id ds18b20_match[] = {
    {.compatible = "lzh-ds18b20-gpio"},
    {}
};

static struct platform_driver ds18b20_driver = {
    .probe = ds18b20_probe,
    .remove = ds18b20_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "lzh_ds18b20",
        .of_match_table = ds18b20_match,
    },
    .id_table = ds18b20_id_table,//以防probe函数不执行
};

//注册platfrom
static int __init __ds18b20_init(void){
    platform_driver_register(&ds18b20_driver);
    return 0;
}

static void __exit __ds18b20_exit(void){
    platform_driver_unregister(&ds18b20_driver);
}

module_init(__ds18b20_init);
module_exit(__ds18b20_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LZH");