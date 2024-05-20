#include <linux/of_gpio.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/iio/iio.h>
#include <linux/iio/types.h>
#include <linux/iio/buffer.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>

#define NAME "gl5506_dev"

//内存映射后地址差几位
#define GL5506_ADC_HC0  0x00
#define GL5506_ADC_HS   0x08
#define GL5506_ADC_R0   0x0C
#define GL5506_ADC_CFG  0x14
#define GL5506_ADC_GC   0x18
#define GL5506_ADC_GS   0x1C
#define GL5506_ADC_CV   0x20
#define GL5506_ADC_OFS  0x24
#define GL5506_ADC_CLA  0x28

struct gl5506_dev{
    struct device *device;
    struct mutex lock;

    struct clk *clk;
    u32 vref;
    u32 vref_user_v;
    void __iomem *regs;

    int value;
    int value_scale;

};
struct gl5506_dev *gl5506_dev;
//选择channel
//当使用触发缓冲区的时候， scan_index 是扫描索引
//为‘u’表示数据为无符号类型，为‘s’的话为有符号类型
// data12位，所以要给两个字节
//这三个轴的 IIO_CHAN_INFO_RAW 属性是相互独立的
//使能 IIO_CHAN_INFO_SCALE 这个属性，表示这三个通道的分辨率是共用的，  另一个是表示采样率信息

#define GL5506_ADC_CHAN(idx,_type) { 	\
    .type = (_type),	\
    .indexed = 1, \
    .channel = (idx), \
    .scan_index = (idx), \
    .scan_type = { \
        .sign = 'u', \
        .realbits = 12, \
        .storagebits = 16,  \
        .shift = 0, \
    }, \
    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),  \
    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE) | BIT(IIO_CHAN_INFO_SAMP_FREQ), \
}

static const struct iio_chan_spec gl5506_channels[] = {
    GL5506_ADC_CHAN(3, IIO_VOLTAGE),
    GL5506_ADC_CHAN(4, IIO_VOLTAGE),
};

static void adc_cfg_init(struct gl5506_dev* gl5506_dev){
    /* CFG寄存器
    * bit16 0 关闭复写功能
    * bit15:14 00 硬件平均设置为默认值，00的时候4次平均，
    * 但是得ADC_GC寄存器的AVGE位置1来使能硬件平均
    * bit13 0 软件触发
    * bit12:1 00 参考电压为VREFH/VREFL，也就是3.3V/0V
    * bit10 0 正常转换速度
    * bit9:8 00 采样时间2/12，ADLSMP=0(短采样)的时候为2个周期
    * ADLSMP=1(长采样)的时候为12个周期
    * bit7 1 低功耗模式
    * bit6:5 00 ADC时钟源1分频
    * bit4 0 短采样
    * bit3:2 10 12位ADC
    * bit1:0 00 ADC时钟源选择ADACK
    */

    int cfg_data = 0;

    //使用默认时钟源
    cfg_data |= 0x00;

    //12为精度
    cfg_data |= 0x08;

    //短采样
        //不变
    //adc时钟源1分频

    //低功耗模式
    cfg_data |= 0x80;

    //采样时间两个周期

    //正常运转速度

    //设置参考电压

    //软件触发

    //设置4次样本平均

    //打开复写功能
    cfg_data |= 0x10000;

//以上功能可以写成switch语句，进行系统选择
    writel(cfg_data, gl5506_dev->regs + GL5506_ADC_CFG);
}

static void adc_gc_init(struct gl5506_dev* gl5506_dev){
    /* GC寄存器
    * bit7 0 先关闭校准功能，后面会校准
    * bit6 0 关闭持续转换
    * bit5 1 关闭硬件平均功能
    * bit4 0 关闭比较功能
    * bit3 0 关闭比较的Greater Than功能
    * bit2 0 关闭比较的Range功能
    * bit1 0 关闭DMA
    * bit0 0 关闭ADACK
    */
   int gc_data = 0;
   gc_data |= 0x20;
   writel(gc_data,gl5506_dev->regs + GL5506_ADC_GC);
}

static void adc_calibration(struct gl5506_dev* gl5506_dev){
    int hc_data = 0;
    int gc_data = 0;
    int gs_data = 0;
    int hs_data = 0;

    hc_data |= 0x80;//为 1 的时候打开转换完成中断
    // hc_data |= 0x1f;//关闭转换
    writel(hc_data,gl5506_dev->regs + GL5506_ADC_HC0);

    gc_data = readl(gl5506_dev->regs + GL5506_ADC_GC);
    gc_data |= 0x80;  //使能校准功能 
    writel(gc_data,gl5506_dev->regs + GL5506_ADC_GC);
    msleep(50);
    gs_data = readl(gl5506_dev->regs + GL5506_ADC_GS);
    if(gs_data & 0x2){
        dev_err(gl5506_dev->device,"gs calibration error\r\n");
        return;
    }
    
    hs_data = readl(gl5506_dev->regs + GL5506_ADC_HS);
    if((hs_data & 0x1)==0){
        dev_err(gl5506_dev->device,"hs calibration error\r\n");
        return;
    }
}

static void gl5506_adc_init(struct gl5506_dev* gl5506_dev){
    mutex_lock(&gl5506_dev->lock);
    adc_cfg_init(gl5506_dev);//cfg寄存器初始化
    adc_gc_init(gl5506_dev);//gc寄存器初始化

    adc_calibration(gl5506_dev);//进行adc之前校准一次
    mutex_unlock(&gl5506_dev->lock);
}
static int temp = 0;

static irqreturn_t gl5506_irq_handler(int irq, void *dev_id){
    struct iio_dev *iio_dev = (struct iio_dev*)dev_id;
    struct gl5506_dev *gl5506_dev = iio_priv(iio_dev);
    int coco = 0;

    coco = readl(gl5506_dev->regs + GL5506_ADC_HS);
    if(coco == 1){
        // mutex_lock(&gl5506_dev->lock);
        temp = readl(gl5506_dev->regs + GL5506_ADC_R0);

        temp &= 0xFFF;//12位精度

        gl5506_dev->value = temp;
        // mutex_unlock(&gl5506_dev->lock);
    }

    return IRQ_HANDLED;
}

static int gl5506_read_raw(struct iio_dev *indio_dev,
			struct iio_chan_spec const *chan,
			int *val,
			int *val2,
			long mask)
{
    struct gl5506_dev *gl5506_dev = iio_priv(indio_dev);
    int hc_data = 0;

    switch (mask){
        case IIO_CHAN_INFO_RAW:{
            mutex_lock(&gl5506_dev->lock);
            hc_data = readl(gl5506_dev->regs + GL5506_ADC_HC0);
            hc_data |= 0x03;//打开通道3
            writel(hc_data,gl5506_dev->regs + GL5506_ADC_HC0);

            switch(chan->type){
                case IIO_VOLTAGE:{
                    
                    *val = temp;
                }
                break;
                default:
                    break;
            }
            mutex_unlock(&gl5506_dev->lock);
            return IIO_VAL_INT;
        }
            break;
        case IIO_CHAN_INFO_SCALE:{
            //换算mv
            *val = gl5506_dev->vref_user_v / 1000;
            *val2 = 12;//精度12位
            return IIO_VAL_FRACTIONAL_LOG2;//val>>val2

        }
            break;
        default:
            break;
    }

    return 0;
}

static int gl5506_write_raw(struct iio_dev *indio_dev,
			 struct iio_chan_spec const *chan,
			 int val,
			 int val2,
			 long mask)
{


    return 0;
}


struct iio_info gl5506_info = {
    .driver_module = THIS_MODULE,
    .read_raw = gl5506_read_raw,
    .write_raw = gl5506_write_raw,
};


static int gl5506_probe(struct platform_device *pdev){
    struct gl5506_dev *gl5506_dev;
    struct iio_dev *iio_dev;
    int ret = 0;
    struct resource *mem;//资源
    int irq = 0;

    //申请iio_dev内存,为私有结构分配的内存空间
    iio_dev = devm_iio_device_alloc(&pdev->dev,sizeof(struct gl5506_dev));
    if(!iio_dev){
        printk("fail alloc iio device\r\n");
        return -1;
    }

    //获取gl5506_dev结构地址
    gl5506_dev = iio_priv(iio_dev);
    gl5506_dev->device = &pdev->dev;

    //iio_dev的其他成员变量
    iio_dev->dev.parent = &pdev->dev;
    iio_dev->info = &gl5506_info;
    iio_dev->name = NAME;
    iio_dev->modes = INDIO_DIRECT_MODE;// 直接模式，提供 sysfs 接口,(也可以用软硬件触发器)
    iio_dev->channels = gl5506_channels;
    iio_dev->num_channels = ARRAY_SIZE(gl5506_channels);

    //获取时钟配置和电源配置
    gl5506_dev->clk = devm_clk_get(gl5506_dev->device,"adc");
    if(IS_ERR(gl5506_dev->clk)){
        dev_err(&pdev->dev,"iio clk error\r\n");
        return -1;
    }
    ret = clk_prepare_enable(gl5506_dev->clk);
    if(ret != 0){
        dev_err(&pdev->dev,"iio clk prepare error\r\n");
        goto clk_prepare_error;
    }
        //请求
    gl5506_dev->vref = devm_regulator_get(&pdev->dev,"vref");//为在设备树中，通常会使用"xxx-supply"的形式来指定设备所需的电源供应，而"xxx"通常是设备需要的某种资源或信号的名称。
    if(IS_ERR(gl5506_dev->vref)){
        dev_err(&pdev->dev,"vref error \r\n");
        return -1;
    }
        //使能
    ret = regulator_enable(gl5506_dev->vref);
    if(ret != 0){
        dev_err(&pdev->dev,"vref enable error\r\n");
        return ret;
    }
        //获取
    gl5506_dev->vref_user_v = regulator_get_voltage(gl5506_dev->vref);

    //内存映射
    mem = platform_get_resource(pdev,IORESOURCE_MEM,0);
    gl5506_dev->regs = devm_ioremap_resource(&pdev->dev,mem);
    if(IS_ERR(gl5506_dev->regs)){
        dev_err(&pdev->dev,"ioremap error \r\n");
        return -1;
    }

    //初始化
    platform_set_drvdata(pdev, iio_dev);
    mutex_init(&gl5506_dev->lock);
    gl5506_adc_init(gl5506_dev);
    msleep(50);


    //中断
    //adc注册中断是gic_spi 100 每次adc数据准备完成或者转换完成就会触发中断，
    //可以写一个中断触发函数，当产生中断把数据给buffer中，前提注册触发器，使能缓冲区
    //本驱动仅读取节点数据,就不涉及buffer，仅赋值
    irq = platform_get_irq(pdev,0);
    if(irq < 0){
        dev_err(&pdev->dev,"get irq error\r\n");
        return -1;
    }

    ret = devm_request_irq(gl5506_dev->device,irq,gl5506_irq_handler,0,NAME,iio_dev);
    if(ret < 0){
        dev_err(&pdev->dev,"request irq error\r\n");
        return -1;
    }


     //注册iio_device
    ret = iio_device_register(iio_dev);
    if(ret != 0){
        dev_err(&pdev->dev,"iio register error\r\n");
        goto register_error;
    }

    return 0;
clk_prepare_error:
    clk_disable_unprepare(gl5506_dev->clk);
register_error:
    iio_device_unregister(iio_dev);
    return ret;
}

static int gl5506_remove(struct platform_device *pdev){
    struct iio_dev *iio_dev = platform_get_drvdata(pdev);
    struct gl5506_dev *gl5506_dev = iio_priv(iio_dev);

    iio_device_unregister(iio_dev);
    clk_disable_unprepare(gl5506_dev->clk);
    regulator_disable(gl5506_dev->vref);
    return 0;
}

static const struct platform_device_id gl5506_id_table[] = {
    {"fsl,imx6ul-adc",0},
    {}
};

const struct of_device_id gl5506_match_table[] = {
    {.compatible = "fsl,imx6ul-adc"},
    {},
};

static struct platform_driver gl5506_driver = {
    .probe = gl5506_probe,
    .remove = gl5506_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "gl5506",
        .of_match_table = gl5506_match_table,
    },
    .id_table = gl5506_id_table,
};

static int __init gl5506_init(void){
    platform_driver_register(&gl5506_driver);
    return 0;
}

static void __exit gl5506_exit(void){
    platform_driver_unregister(&gl5506_driver);
}

module_init(gl5506_init);
module_exit(gl5506_exit);
MODULE_AUTHOR("LZH");
MODULE_LICENSE("GPL");