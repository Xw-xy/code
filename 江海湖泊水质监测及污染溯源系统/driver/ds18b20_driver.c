#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <generated/autoconf.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h> 
#define DS18B20_NAME "ds18b20"
#define DS18B20_MINOR 144
struct ds18b20_device 
{
    struct device_node  *nd;  /* 设备节点      */ 
    int ds18b20_gpio;
    u8 TL,TH;

};
struct ds18b20_device  ds18b20_dev = {
    .TL = 0,
    .TH = 0,
};
static int ds18b20_dev_restart(struct ds18b20_device *dev)
{
    gpio_direction_output(dev->ds18b20_gpio,1);
    gpio_set_value(dev->ds18b20_gpio,0);
    udelay(900);
    gpio_set_value(dev->ds18b20_gpio,1);
    gpio_direction_input(dev->ds18b20_gpio);
    udelay(30);
    while(gpio_get_value(dev->ds18b20_gpio));
    udelay(60);
    if(gpio_get_value(dev->ds18b20_gpio))
    {
        printk("fail ds18b20 init");
        return -1;
    }
    gpio_direction_output(dev->ds18b20_gpio,1);
    udelay(100);
    //gpio_set_value(dev->ds18b20_gpio,1);

    return 0;
}

static int ds18b20_write_byte(struct ds18b20_device *dev,u8 *data)
{
    u8 d = *data;
    u8 c = 0;
    int i = 0;
    
    for(i = 0;i < 8;i++)
    {
        c = ((d >> i) & 0x01);
        //printk(" c = %d\r\n",c );
        gpio_direction_output(dev->ds18b20_gpio,1);
        gpio_set_value(dev->ds18b20_gpio,0);
        udelay(8);
        gpio_set_value(dev->ds18b20_gpio,c);
        udelay(45);
        gpio_set_value(dev->ds18b20_gpio,1);         /*释放总线*/
    }
    gpio_direction_output(dev->ds18b20_gpio,1);
    return 0;
}

static int ds18b20_read_byte(struct ds18b20_device *dev)
{
    u8 d = 0,c = 0;
    int i = 0;
   
    //gpio_direction_input(dev->ds18b20_gpio);
    for(i = 0;i < 8;i++)
    {
        gpio_direction_output(dev->ds18b20_gpio,1);
        gpio_set_value(dev->ds18b20_gpio,0);
        //gpio_set_value(dev->ds18b20_gpio,1);
        gpio_direction_input(dev->ds18b20_gpio);
        c = gpio_get_value(dev->ds18b20_gpio);
        gpio_direction_output(dev->ds18b20_gpio,1);
        //printk("c = %d\r\n",c);
        udelay(200);
        d |= (c << i);

    }

    //printk("d = %d\r\n",d);
    return d;
}

static int  ds18b20_read_data(struct ds18b20_device *dev)
{
    int i = 0;
    u8 data[2] = {0};
    char buf[3] = {0xcc,0x44,0xbe};
    ds18b20_dev_restart(dev);
    ds18b20_write_byte(dev,&buf[0]);
    ds18b20_write_byte(dev,&buf[1]);
    mdelay(2000);
    ds18b20_dev_restart(dev);
    ds18b20_write_byte(dev,&buf[0]);
    ds18b20_write_byte(dev,&buf[2]);
    for(i = 0;i < 2;i++)
    {
        data[i]=ds18b20_read_byte(dev);

    }
    dev->TL = data[0];
    dev->TH = data[1];
    return 0;


}
static ssize_t ds18b20_read (struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    int ret = 0;
    u16 data = 0;
    ds18b20_read_data(&ds18b20_dev);

    data = ((((u16)ds18b20_dev.TH )<< 8) + ds18b20_dev.TL);
    ret = copy_to_user(buf,&data,2);
    //printk("data = %d",data);
    //buf = (char __user*) &temp;

    return 0;
}
static int ds18b20_open (struct inode *node, struct file *file)
{
    return 0;
}
static int ds18b20_release (struct inode *node, struct file *file)
{
    return 0;

}
const struct file_operations  ds18b20_fop = {
    .read = ds18b20_read,
    .open = ds18b20_open,
    .release = ds18b20_release,
};
   
struct miscdevice ds18b20_miscdev = {
    .minor = DS18B20_MINOR,
    .fops = &ds18b20_fop,
    .name = DS18B20_NAME,
};
static int ds18b20_probe(struct platform_device *dev)
{
    int ret = 0;
    ds18b20_dev.nd = of_find_node_by_path("/ds18b20");
    if(ds18b20_dev.nd == NULL)
    {
        printk("fail find node");
        return -EINVAL;
    }
    ds18b20_dev.ds18b20_gpio = of_get_named_gpio(ds18b20_dev.nd,"ds18b20-gpio",0);
    if(ds18b20_dev.ds18b20_gpio < 0)
    {
        printk("fail get gpio");
        return -EINVAL;
    }
    gpio_request(ds18b20_dev.ds18b20_gpio,DS18B20_NAME);
    ret = misc_register(&ds18b20_miscdev);
    if(ret < 0)
    {
        printk("fail misc reg!");
        return -EINVAL;
    }
    return 0;

}
static int ds18b20_remove(struct platform_device *dev)
{
    gpio_free(ds18b20_dev.ds18b20_gpio);
    misc_deregister(&ds18b20_miscdev);

    return 0;

}
const struct platform_device_id ds18b20_device_id[] = {
    {"alientek-ds18b20",0},
    {}
};
const struct of_device_id ds18b20_of_match_id[] = {
    {.compatible = "alientek-ds18b20"},
    { }
};
struct platform_driver ds18b20_drv = {
    .driver = {
        .name  = DS18B20_NAME,
        .owner = THIS_MODULE,
        .of_match_table = ds18b20_of_match_id,
    },
    .probe = ds18b20_probe,
    .remove = ds18b20_remove,
    .id_table = ds18b20_device_id,

 };
static int __init ds18b20_init(void)
{
    platform_driver_register(&ds18b20_drv);
    return 0;
}
static void __exit ds18b20_exit(void)
{
    platform_driver_unregister(&ds18b20_drv);
}

module_init(ds18b20_init);
module_exit(ds18b20_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XBC");