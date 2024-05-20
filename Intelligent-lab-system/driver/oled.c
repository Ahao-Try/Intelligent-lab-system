#include <linux/io.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>
#include <linux/cdev.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/gpio.h>

#define NAME "lzh_oled"
#define OLED_CMD 0
#define OLED_DATA 1

static u8 buffer[1024] = {0};

struct oled_dev {
    int major;
    int minor;
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;

    void* private_data;
    struct device_node *node;
    int cs_gpio;
    int dc_gpio;

    struct mutex lock;
};

struct oled_dev oled_dev;

static s32 oled_write_regs(struct oled_dev *dev, u8 *buf, u8 len) {
    int ret = 0;

    struct spi_message *m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;

    mutex_lock(&dev->lock);
    // gpio_set_value(dev->cs_gpio, 0);

    m = kmalloc(sizeof(struct spi_message),GFP_KERNEL);//GFP_KERNEL是内核内存分配时最常用的，无内存可用时可引起休眠
    if (!m) {
        printk("Failed to allocate spi_message\n");
        mutex_unlock(&dev->lock);
        // gpio_set_value(dev->cs_gpio, 1);
        return -ENOMEM;
    }

    // 使用kmalloc动态分配spi_transfer和txdata
    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
    if (!t) {
        printk("Failed to allocate spi_transfer\n");
        kfree(m);
        mutex_unlock(&dev->lock);
        // gpio_set_value(dev->cs_gpio, 1);
        return -ENOMEM;
    }

    unsigned char *txdata = kmalloc(len, GFP_KERNEL); // 动态分配len个字节的内存
    if (!txdata) {
        printk("Failed to allocate txdata buffer\n");
        kfree(t);
        kfree(m);
        mutex_unlock(&dev->lock);
        // gpio_set_value(dev->cs_gpio, 1);
        return -ENOMEM;
    }
    memcpy(txdata, buf, len); // 将buf中的数据复制到txdata

    t->tx_buf = txdata;
    t->len = len;

    spi_message_init(m);
    spi_message_add_tail(t, m);
    ret = spi_sync(spi, m);

    // gpio_set_value(dev->cs_gpio, 1);

    kfree(txdata); // 传输完成后释放txdata
    kfree(t);      // 释放spi_transfer结构体
    kfree(m);
    mutex_unlock(&dev->lock);
    return ret;
}

static s32 oled_write_onereg(struct oled_dev *dev,u8 data,u8 cmd){
    u8 buf = data;
    gpio_set_value(dev->dc_gpio,cmd);//写数据

    oled_write_regs(dev,&buf,1);

    return 0;
}

//oled是竖着显示128*64个像素点   128*8个字节，一个字节包含竖下来8个点，只有亮和不亮，一个bit表示一个像素点
static void oled_refresh(struct oled_dev *dev){
    int i ,j;

    for(i = 0;i < 8;i++){
        oled_write_onereg(&oled_dev,0xb0+i,OLED_CMD);//设置起始行地址，0xb0是控制命令
        oled_write_onereg(&oled_dev,0x00,OLED_CMD);//设置低列起始地址
        oled_write_onereg(&oled_dev,0x10,OLED_CMD);//设置高列起始地址
        for(j = 0;j < 128;j++){
            oled_write_onereg(&oled_dev,buffer[i * 128 + j],OLED_DATA);
        }
    }
}

static void oled_clear(struct oled_dev *dev){
    int i,j;
    for(i = 0;i < 8;i++){
        for(j = 0;j < 128;j++){
            buffer[i * 128 + j] = 0;
        }
    }
    oled_refresh(&oled_dev);
}


static void oled_regs_init(struct oled_dev *dev){
    int ret = 0;

	oled_write_onereg(dev, 0xAE,OLED_CMD);//先关闭显示
	oled_write_onereg(dev, 0x00,OLED_CMD);//设置列起始地址的低4位
	oled_write_onereg(dev, 0x10,OLED_CMD);//设置列起始地址的高4位
	oled_write_onereg(dev, 0x40,OLED_CMD);//设置显示的起始行
	oled_write_onereg(dev, 0x81,OLED_CMD);//设置对比度
	oled_write_onereg(dev, 0xCF,OLED_CMD);// 设置对比度亮度
	oled_write_onereg(dev, 0xA1,OLED_CMD);//从左到右刷新     0xa0左右反置 0xa1正常
	oled_write_onereg(dev, 0xC8,OLED_CMD);//从上往下扫描   0xc0上下反置 0xc8正常
	oled_write_onereg(dev, 0xA6,OLED_CMD);//正常显示，即GDDRAM中1表示显示，0表示不显示
	oled_write_onereg(dev, 0xA8,OLED_CMD);//设置通道数
	oled_write_onereg(dev, 0x3f,OLED_CMD);//1/64通道数
	oled_write_onereg(dev, 0xD3,OLED_CMD);//设置COM的偏移值
	oled_write_onereg(dev, 0x00,OLED_CMD);//偏移值=0
	oled_write_onereg(dev, 0xd5,OLED_CMD);//设置时钟分频
	oled_write_onereg(dev, 0x80,OLED_CMD);//设置100分频
	oled_write_onereg(dev, 0xD9,OLED_CMD);//设置预充电期
	oled_write_onereg(dev, 0xF1,OLED_CMD);//预充电期设置为 15 个时钟周期，并将放电期设置为 1 个时钟周期
	oled_write_onereg(dev, 0xDA,OLED_CMD);//设置 COM 引脚
	oled_write_onereg(dev, 0x12,OLED_CMD);//为了提高显示质量，
	oled_write_onereg(dev, 0xDB,OLED_CMD);//设置 VCOMH 电压
	oled_write_onereg(dev, 0x40,OLED_CMD);//设置 VCOMH 电压的值
	oled_write_onereg(dev, 0x20,OLED_CMD);//设置页面寻址模式
	oled_write_onereg(dev, 0x02,OLED_CMD);//设置页面寻址模式（可以是 0x00, 0x01 或 0x02），这里设置为 0x02
	oled_write_onereg(dev, 0x8D,OLED_CMD);//设置电荷泵使能/禁用
	oled_write_onereg(dev, 0x14,OLED_CMD);//使能电荷泵，0x10 则是禁用电荷泵。这里选择 0x14 表示使能电荷泵
	oled_write_onereg(dev, 0xA4,OLED_CMD);// 禁用整个显示开启
	oled_write_onereg(dev, 0xA6,OLED_CMD);//禁用反转显示
	oled_write_onereg(dev, 0xAF,OLED_CMD);//最后打开显示

    oled_clear(&oled_dev);
}
    
static int oled_open(struct inode *inode, struct file *filp){
    filp->private_data = &oled_dev;
    return 0;
}

static ssize_t oled_read (struct file *filp, char __user *buf, size_t cnt, loff_t *offset){
    return 0;
}

static ssize_t oled_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *offset){
    int ret = 0;
    struct oled_dev *dev = (struct oled_dev*)filp->private_data;

    oled_clear(dev);

    ret = copy_from_user(buffer,buf,cnt);
    if(ret < 0){
        printk("copy from user error\r\n");
        return -1;
    }
    
    oled_refresh(dev);

    return 0;
}

static int oled_release (struct inode *inode, struct file *filp){
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = oled_open,
    .read = oled_read,
    .write = oled_write,
    .release = oled_release,
};

static int oled_probe(struct spi_device *spi){
    int ret = 0;

    if (oled_dev.major) {
        oled_dev.devid = MKDEV(oled_dev.major, 0);
        ret = register_chrdev_region(oled_dev.devid, 1, NAME);
        if (ret < 0) {
            printk("register error\n");
            goto register_error;
        }
    } else {
        ret = alloc_chrdev_region(&oled_dev.devid, 0, 1, NAME);
        if (ret < 0) {
            printk("alloc chrdev region error\n");
            goto register_error;
        }
    }

    cdev_init(&oled_dev.cdev, &fops);
    oled_dev.cdev.owner = THIS_MODULE;
    ret = cdev_add(&oled_dev.cdev, oled_dev.devid, 1);
    if (ret < 0) {
        printk("cdev add error\n");
        goto cdev_error;
    }

    oled_dev.class = class_create(THIS_MODULE, NAME);
    if (IS_ERR(oled_dev.class)) {
        printk("class create error\n");
        ret = PTR_ERR(oled_dev.class);
        goto class_error;
    }

    oled_dev.device = device_create(oled_dev.class, NULL, oled_dev.devid, NULL, NAME);
    if (IS_ERR(oled_dev.device)) {
        printk("device create error\n");
        ret = PTR_ERR(oled_dev.device);
        goto device_error;
    }

    // oled_dev.node = of_find_node_by_path("/soc/aips-bus@02000000/spba-bus@02000000/ecspi@02008000");
    // if(oled_dev.node == NULL){
    //     printk("cs-gpios node find error\r\n");
    //     goto node_error;
    // }

    // oled_dev.cs_gpio = of_get_named_gpio(oled_dev.node,"cs-gpios",1);
    //  printk("%d\r\n",oled_dev.cs_gpio);
    // if(oled_dev.cs_gpio < 0){
    //     printk("cs-gpio error\r\n");
    //     goto gpio_error;
    // }

    // ret = gpio_request(oled_dev.cs_gpio, "oled_dc_gpio");
    // printk("%d\r\n",ret);
    // if (ret) {
    //     printk("gpio request error\n");
    //     goto gpio_error;
    // }

    oled_dev.node = of_find_node_by_name(NULL, "oled");
    if (!oled_dev.node) {
        printk("node find error\n");
        ret = -ENODEV;
        goto node_error;
    }

    oled_dev.dc_gpio = of_get_named_gpio(oled_dev.node,"oled_dc_gpio",0);
    printk("%d\r\n",oled_dev.dc_gpio);
    if (oled_dev.dc_gpio < 0) {
        printk("dc gpio error\n");
        ret = oled_dev.dc_gpio;
        goto gpio_error;
    }

    ret = gpio_request(oled_dev.dc_gpio, "oled_dc_gpio");
    printk("%d\r\n",ret);
    if (ret) {
        printk("gpio request error\n");
        goto gpio_error;
    }

    // gpio_direction_output(oled_dev.cs_gpio,1);
    gpio_direction_output(oled_dev.dc_gpio,1);

    spi->mode = SPI_MODE_0;
    ret = spi_setup(spi);
    printk("%d\r\n",ret);
    if (ret) {
        printk("spi setup error\n");
        goto spi_setup_error;
    }
    oled_dev.private_data = spi;

    mutex_init(&oled_dev.lock);
    oled_regs_init(&oled_dev);

    return 0;
spi_setup_error:
    gpio_free(oled_dev.dc_gpio);
    // gpio_free(oled_dev.cs_gpio);
gpio_error:
node_error:
    device_destroy(oled_dev.class, oled_dev.devid);
device_error:
    class_destroy(oled_dev.class);
class_error:
    cdev_del(&oled_dev.cdev);
cdev_error:
    unregister_chrdev_region(oled_dev.devid, 1);
register_error:
    return ret;
}

static int oled_remove(struct spi_device *spi){

    gpio_free(oled_dev.dc_gpio);

    // gpio_free(oled_dev.cs_gpio);

    device_destroy(oled_dev.class,oled_dev.devid);

    class_destroy(oled_dev.class);

    cdev_del(&oled_dev.cdev);

    unregister_chrdev_region(oled_dev.devid,1);
    return 0;
}

static const struct spi_device_id oled_table[] = {
    { "lzh_oled", 0 },
    { }
};

static const struct of_device_id oled_match_table[] = {
    { .compatible = "lzh_oled" },
    { }
};

static struct spi_driver oled_driver = {
    .probe = oled_probe,
    .remove = oled_remove,
    .id_table = oled_table,
    .driver = {
        .name = "lzh_oled",
        .of_match_table = oled_match_table,
    }
};

static int __init oled_init(void) {
    return spi_register_driver(&oled_driver);
}

static void __exit oled_exit(void) {
    spi_unregister_driver(&oled_driver);
}


module_init(oled_init);
module_exit(oled_exit);

MODULE_AUTHOR("LZH");
MODULE_LICENSE("GPL");