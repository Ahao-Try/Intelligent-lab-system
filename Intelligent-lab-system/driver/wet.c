#include <linux/of_gpio.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/ide.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/timer.h>
#include <linux/irqflags.h>
#include <linux/workqueue.h>

#define NAME "lzh_dht11"
#define HIGH_VALUE 1
#define LOW_VALUE 0

#define DHT11_IOCTL_TYPE 'M'
#define DHT11_CMD_GET_WET _IO(DHT11_IOCTL_TYPE,1)
#define DHT11_CMD_GET_TEM _IO(DHT11_IOCTL_TYPE,2)

struct node{
    struct device_node *tree_node;
    int gpio_num;
};

struct dht11_device{
    int major;
    int minor;
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct node node;
    struct mutex lock;//防止线程抢占资源
    struct timer_list timer;
    struct work_struct work;//用于在驱动程序或内核模块中执行一些不需要立即完成的操作，以提高系统的性能和响应速度。
};

struct dht11_device lzh_dht11;
static unsigned char data[5] = {};
static struct workqueue_struct *my_workqueue;

//设置输出
static void set_out_and_value(int value){
    gpio_direction_output(lzh_dht11.node.gpio_num,value);
}

//设置输入
static void set_in_and_value(void){
    gpio_direction_input(lzh_dht11.node.gpio_num);
}

//获取gpio值
static int get_gpio_value(void){
    return gpio_get_value(lzh_dht11.node.gpio_num);
}

//dht11初始化
static int dht11_init(void){
    int time = 0;//计时，是否相应时间达标 先低80us，再高80us

    //初始状态输出1
    set_out_and_value(HIGH_VALUE);
    udelay(2);

    //主机发出开始信号，把总线拉低大于18ms
    set_out_and_value(LOW_VALUE);
    mdelay(20);

    //拉高电平延时等待20-40us后, 读取DHT11的响应信号
    set_out_and_value(HIGH_VALUE);
    udelay(30);

    //等待主机开始信号结束，发送80us低电平响应信号
    set_in_and_value();
    
    time = 0;
    //第一种情况，set in low error
    while(get_gpio_value() != 0){
        udelay(1);
        time++;
        if(time > 100){
            printk("init error ,set in low error\r\n");
            return -1;
        }
    }

    //第二种情况 set in low time > 100
    time = 0;
    while(get_gpio_value() == 0){
        udelay(1);
        time++;
        if(time > 100){
            printk("too long set low in\r\n");
            return -1;
        }
    }

    //第三种情况 set in heigh too long
    time = 0;
    while(get_gpio_value() == 1){
        udelay(1);
        time++;
        if(time > 100){
            printk("set in heigh too long\r\n");
            return -1;
        }
    }

    return 0;
}

//读一个字节
static unsigned char dht11_read_byte(void){
    unsigned char byte = 0;
    unsigned char bit = 0;
    int time = 0;
    int i = 0;
    for(;i < 8;i++){
        time = 0;
        while(get_gpio_value() == 0){
            udelay(1);
            time++;
            if(time > 100){
                printk("read in low time too long\r\n");
                return -1;
            }
        }

        udelay(40);
        byte <<= 1;
        if(get_gpio_value() == 0){
            bit = 0;
        }
        else{
            bit = 1;
            byte |= bit;
            time = 0;
            while(get_gpio_value() == 1){
                udelay(1);
                time++;
                if(time > 100){
                    printk("read in hight time too long\r\n");
                    return -1;
                }

            }
        }     
    }

    return byte;
}

//读出输出数据
static int dht11_read_all(void){
    int i = 0;
    int count = 0;
    // unsigned char temp[5] = {};
    while(dht11_init() != 0){
        count++;
        if(count > 50){
            printk("init error\r\n");
            return -1;
        }
    }

    for(;i < 5;i++){
        data[i] = dht11_read_byte();
    }
    udelay(50);
    set_out_and_value(HIGH_VALUE);

    return 0;
}

static void checking(struct work_struct *work){
    int count = 0;
    int ret = 0;
    dht11_read_all();
     //判断校验位
    while(data[4] != data[0] + data[1] + data[2] + data[3]){
        dht11_read_all();
        // ret = dht11_read_all();
        
        // count++;
        // if(count > 50 || ret == -1){
        //     printk("data 4 error\r\n");
        //     count = 0;
        //     return -1;
        // }
    }
    return ;
}

// static void recallfunction(unsigned long arg){
//     // schedule_work(&lzh_dht11.work);
//     queue_work(my_workqueue, &lzh_dht11.work);
//     mod_timer(&lzh_dht11.timer, jiffies + msecs_to_jiffies(10));
// }

static int dht11_open (struct inode *node, struct file *filp){
    filp->private_data = &lzh_dht11;
    return 0;
}

static ssize_t dht11_read (struct file *filp, char __user *buf, size_t cnt, loff_t *ofest){
    checking(&lzh_dht11.work);
    if(copy_to_user(buf, data, sizeof(data)))
        return -1;
    return 0;
}

static long dht11_ioctl (struct file *filp, unsigned int cmd, unsigned long arg){
    int ret = 0;
    unsigned char to_user_buf[2] = {};

    unsigned long flags;
    local_irq_restore(flags); 
    mutex_init(&lzh_dht11.lock);
    mutex_lock(&lzh_dht11.lock);

    // init_timer(&lzh_dht11.timer);
    // lzh_dht11.timer.function = recallfunction;
    // lzh_dht11.timer.expires = jiffies + msecs_to_jiffies(10);
    // lzh_dht11.timer.data = ((unsigned long)0);
    // add_timer(&lzh_dht11.timer);
    // INIT_WORK(&lzh_dht11.work, checking);//当工作队列中的工作被调度执行时，将会调用 checking 函数来执行相应的操作
    
    //  my_workqueue = create_workqueue("my_workqueue");
    // if (!my_workqueue) {
    //     printk(KERN_ERR "Failed to create workqueue\n");
    //     return -ENOMEM;
    // }
   
    

    checking(&lzh_dht11.work);
    // ret = checking();
    // if(ret < 0){
    //     printk("read all error\r\n");
    //     mutex_unlock(&lzh_dht11.lock);
    //     local_irq_restore(flags); 
    //     return -1;
    // }
    
    switch(cmd){
        case DHT11_CMD_GET_WET:{
            to_user_buf[0] = data[0];
            to_user_buf[1] = data[1];
            if(copy_to_user((void*)arg, to_user_buf, sizeof(to_user_buf))){
                local_irq_restore(flags);  
                return -1;
            }
        }
        break;
        case DHT11_CMD_GET_TEM:{
            to_user_buf[0] = data[2];
            to_user_buf[1] = data[3];
            if(copy_to_user((void*)arg, to_user_buf, sizeof(to_user_buf))){
                local_irq_restore(flags);  
                return -1;
            }      
        }
        break;
        default:{
            local_irq_restore(flags); 
            return -1;
        }
    }
    mutex_unlock(&lzh_dht11.lock);  
    // cancel_work_sync(&lzh_dht11.work);
    // del_timer_sync(&lzh_dht11.timer);
    local_irq_restore(flags); 
    return 0;
}
static unsigned int dht11_poll (struct file *filp, struct poll_table_struct *wait){

}

static int dht11_release (struct inode *node, struct file *filp){
    // mutex_lock(&lzh_dht11.lock);
    // del_timer_sync(&lzh_dht11.timer);
    // flush_workqueue(my_workqueue);
    // destroy_workqueue(my_workqueue);

	// cancel_work_sync(&lzh_dht11.work);
    // mutex_unlock(&lzh_dht11.lock);
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dht11_open,
    .read = dht11_read,
    .release = dht11_release,
    .unlocked_ioctl = dht11_ioctl,
};


//获取设备树节点
static int get_tree_node(void){
    int ret = 0;
    lzh_dht11.node.tree_node = of_find_node_by_name(NULL,"lzh_dht11");
    if(lzh_dht11.node.tree_node == NULL){
        printk("find tree node error\r\n");
        return -1;
    }

    lzh_dht11.node.gpio_num = of_get_named_gpio(lzh_dht11.node.tree_node,"dht11-gpios",0);
    if(lzh_dht11.node.gpio_num < 0){
        printk("find gpio num error\r\n");
        gpio_free(lzh_dht11.node.gpio_num);
        return -1;
    }

    ret = gpio_request(lzh_dht11.node.gpio_num,NAME);
    if(ret < 0){
        printk("gpio request error\r\n");
        gpio_free(lzh_dht11.node.gpio_num);
        return -1;
    }

    ret = gpio_direction_output(lzh_dht11.node.gpio_num,1);
    if(ret < 0){
        printk("gpio direction output set error\r\n");
        gpio_free(lzh_dht11.node.gpio_num);
        return -1;
    }
    
    return 0;
}

//注册
static int register_dht11(void){
    int ret = 0;

    if(lzh_dht11.major){
        lzh_dht11.devid = MKDEV(lzh_dht11.major,0);
        ret = register_chrdev_region(lzh_dht11.devid,1,NAME);
        if(ret < 0){
            printk("register chrdev error\r\n");
            goto register_error;
        }
    }
    else{
        ret = alloc_chrdev_region(&lzh_dht11.devid,0,1,NAME);
        if(ret < 0){
            printk("register chrdev error\r\n");
            goto register_error;
        }
    }

    lzh_dht11.cdev.owner = THIS_MODULE;
    ret = cdev_add(&lzh_dht11.cdev,lzh_dht11.devid,1);
    if(ret < 0){
        printk("cdev_error\r\n");
        goto cdev_error;
    }
    cdev_init(&lzh_dht11.cdev,&fops);

    lzh_dht11.class = class_create(THIS_MODULE,NAME);
    if(lzh_dht11.class ==NULL){
        ret = -1;
        printk("class error\r\n");
        goto class_error;
    }

    lzh_dht11.device = device_create(lzh_dht11.class,NULL,lzh_dht11.devid,NULL,NAME);
    if(lzh_dht11.device ==  NULL){
        ret = -1;
        printk("device error\r\n");
        goto device_error;
    }
    
    return 0;
device_error:
    device_destroy(lzh_dht11.class,lzh_dht11.devid);
class_error:
    class_destroy(lzh_dht11.class);
cdev_error:
    cdev_del(&lzh_dht11.cdev);
register_error:
    unregister_chrdev_region(lzh_dht11.devid,1);
    return ret;
}

static int dht11_probe(struct platform_device *dev){
    int ret = 0;
    ret = register_dht11();
    if(ret < 0){
        printk("register dht11 error\r\n");
        return -1;
    }

    ret = get_tree_node();
    if(ret < 0){
        printk("get dht11 tree error\r\n");
        return -1;
    }
     INIT_WORK(&lzh_dht11.work, checking);//当工作队列中的工作被调度执行时，将会调用 checking 函数来执行相应的操作

    return 0;
}


static int dht11_remove(struct platform_device *dev){
    gpio_set_value(lzh_dht11.node.gpio_num,1);
    
    gpio_free(lzh_dht11.node.gpio_num);

    device_destroy(lzh_dht11.class,lzh_dht11.devid);

    class_destroy(lzh_dht11.class);

    cdev_del(&lzh_dht11.cdev);

    unregister_chrdev_region(lzh_dht11.devid,1);
    return 0;
}

const struct platform_device_id id_table[] = {
    {"lzh-dht11-gpio",0},
    {},
};

const struct of_device_id match_table[] = {
    {.compatible = "lzh-dht11-gpio"},
    {},
};

struct platform_driver dht11_driver = {
    .probe = dht11_probe,
    .remove = dht11_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "lzh_dht11",
        .of_match_table = match_table,
    },
    .id_table = id_table,
};

//注册模块
static int __init __dht11_init(void){
    platform_driver_register(&dht11_driver);
    return 0;
}

static void __exit __dht11_exit(void){
    platform_driver_unregister(&dht11_driver);
}

module_init(__dht11_init);
module_exit(__dht11_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LZH");