#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#include <linux/random.h>

#include "/home/stefan/usb-common.h"

#define MODULE_NAME		"usb_skel"

struct usb_skel {
	struct usb_device	*udev;
	struct usb_interface	*interface;
	size_t			bulk_in_size;
	__u8			bulk_in_endpointAddr;
	__u8			bulk_out_endpointAddr;
	unsigned char		*bulk_in_buffer;
	size_t			int_in_size;
	__u8			int_in_endpointAddr;
	unsigned char		*int_in_buffer;
	struct urb		*irq_urb;

	atomic_t can_rd;

	struct kref		kref;
	wait_queue_head_t	wq;

	struct task_struct *polling_thread;
	bool thread_should_stop;
};

#define to_skel_dev(d)	container_of(d, struct usb_skel, kref)

#define USB_SKEL_MINOR_BASE	192

static struct usb_device_id skel_table[] = {
	{ USB_DEVICE(VENDOR, PRODUCT) },
	{ }
};
MODULE_DEVICE_TABLE(usb, skel_table);


static void skel_delete(struct kref *kref)
{
	struct usb_skel *dev = to_skel_dev(kref);

	usb_put_dev(dev->udev);

	if (dev->int_in_buffer)
		usb_free_coherent(dev->udev, 8, dev->int_in_buffer, dev->irq_urb->transfer_dma);

	if (dev->irq_urb)
		usb_free_urb(dev->irq_urb);

	if (dev->bulk_in_buffer)
		kfree(dev->bulk_in_buffer);

	kfree(dev);
}

static struct usb_driver skel_driver;

static int skel_open(struct inode *inode, struct file *file)
{
	struct usb_skel *dev;
	struct usb_interface *interface;
	int subminor;

	pr_info("Stefan Stefan: Befeor skel_open");
	subminor = iminor(inode);
	interface = usb_find_interface(&skel_driver, subminor);
	if (!interface) {
		pr_err("%s - error, can't find device for minor %d",
		       __FUNCTION__, subminor);
		return -ENODEV;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		return -ENODEV;
	}

	kref_get(&dev->kref);

	file->private_data = dev;
	return 0;
}

static int skel_release(struct inode *inode, struct file *file)
{
	struct usb_skel *dev;

	dev = (struct usb_skel *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	kref_put(&dev->kref, skel_delete);

	return 0;
}

static ssize_t skel_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct usb_skel *dev;
	int retval = 0;

	dev = (struct usb_skel*)file->private_data;

	retval = usb_bulk_msg(dev->udev,
			      usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
			      dev->bulk_in_buffer,
			      min(dev->bulk_in_size, count),
			      (int *)&count, HZ * 10);

	pr_debug("count = %ld\n", count);

	if (!retval) {
		if (copy_to_user(buffer, dev->bulk_in_buffer, count)) 
			retval = -EFAULT;
		else
			retval = count;
	}

	return count;
}


static struct file_operations skel_fops = {
	.owner =	THIS_MODULE,
	.read =		skel_read,
	.open =		skel_open,
	.release =	skel_release,
};


static struct usb_class_driver skel_class = {
	.name	= "Entropy",
	.fops	= &skel_fops,
	.minor_base = USB_SKEL_MINOR_BASE,
};


static int polling_thread_func(void *data)
{
	struct usb_skel *dev = (struct usb_skel *)data;
	int retval;
	int actual_length;
	char *buffer;

	buffer = kmalloc(dev->bulk_in_size, GFP_KERNEL);
	if (!buffer) {
		pr_err("Failed to allocate buffer for polling\n");
		return -ENOMEM;
	}

	while (!kthread_should_stop()) {
		msleep_interruptible(100000);

		if (dev->thread_should_stop)
			break;

		retval = usb_bulk_msg(dev->udev,
				      usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
				      buffer, dev->bulk_in_size, &actual_length, 5000);
		if (retval) {
			pr_err("Bulk read error: %d\n", retval);
			continue;
		}

		if (actual_length > 0) {
			if (actual_length < sizeof(buffer)) {
				buffer[actual_length] = '\0';
			} else {
				buffer[sizeof(buffer) - 1] = '\0';
			}

			pr_info("Received %d bytes: ", actual_length);
			for (int i = 0; i < actual_length; i++) {
				pr_cont("%c", buffer[i]);
			}
			pr_info("\n");

			//add_hwgenerator_randomness(buffer, actual_length, actual_length , false);

		} else {
			pr_info("No data received\n");
		}
	}

	kfree(buffer);
	return 0;
}
static int skel_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int retval = -ENOMEM;
	struct usb_skel *dev = NULL;
	struct usb_endpoint_descriptor *bulk_in, *bulk_out;

	dev_info(&interface->dev,"probe usb\n");

	dev = kzalloc(sizeof(struct usb_skel), GFP_KERNEL);
	if (!dev) {
		pr_err("Out of memory");
		goto error;
	}
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

    retval = usb_control_msg(dev->udev,
                             usb_sndctrlpipe(dev->udev, 0),
                             USB_REQ_SET_CONFIGURATION, // SET_CONFIGURATION request
                             USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
                             1, // Configuration value
                             0,
                             NULL,
                             0,
                             5000
    );

    if (retval < 0) {
        printk(KERN_ERR "Failed to set device configuration: %d\n", retval);
        return retval;
    }else
		dev_info(&interface->dev,"usb_control_msg Successfull !\n");

	/* Find bulk-in and bulk-out endpoints */
	retval = usb_find_common_endpoints(interface->cur_altsetting,
			&bulk_in, &bulk_out, NULL, NULL);
	if (retval) {
		dev_err(&interface->dev,
			"Could not find both bulk-in and bulk-out endpoints\n");
		goto error;
	}else
		dev_info(&interface->dev,"Endpoints found!\n");
	
	/* Initialize bulk-in endpoint */
	dev->bulk_in_size = usb_endpoint_maxp(bulk_in);
	dev->bulk_in_endpointAddr = bulk_in->bEndpointAddress;
	dev->bulk_in_buffer = kmalloc(dev->bulk_in_size, GFP_KERNEL);
	if (!dev->bulk_in_buffer) {
		retval = -ENOMEM;
		goto error;
	}

	/* Initialize bulk-out endpoint */
	dev->bulk_out_endpointAddr = bulk_out->bEndpointAddress;

	/* Setup waitqueue */
	atomic_set(&dev->can_rd, 0);
	init_waitqueue_head(&dev->wq);

	usb_set_intfdata(interface, dev);

	/* Start polling thread */
	dev->thread_should_stop = false;
	dev->polling_thread = kthread_run(polling_thread_func, dev, "usb_skel_polling");
	if (IS_ERR(dev->polling_thread)) {
		pr_err("Failed to create polling thread\n");
		retval = PTR_ERR(dev->polling_thread);
		goto error;
	}

	/* Register the device */
	retval = usb_register_dev(interface, &skel_class);
	if (retval) {
		pr_err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		dev->thread_should_stop = true;
		kthread_stop(dev->polling_thread);
		goto error;
	}

	dev_info(&interface->dev, "USB Skeleton device now attached to USBSkel-%d", interface->minor);
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, skel_delete);
	return retval;
}

static void skel_disconnect(struct usb_interface *interface)
{
	struct usb_skel *dev;
	int minor = interface->minor;

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* Stop polling thread */
	if (dev->polling_thread) {
		dev->thread_should_stop = true;
		kthread_stop(dev->polling_thread);
	}

	usb_deregister_dev(interface, &skel_class);

	kref_put(&dev->kref, skel_delete);
	dev_info(&interface->dev, "USB Skeleton #%d now disconnected\n", minor);
}


static struct usb_driver skel_driver = {
	.name	= "usb_skeleton",
	.id_table = skel_table,
	.probe	= skel_probe,
	.disconnect = skel_disconnect,
};

static
int __init m_init(void)
{
	int result;
	result = usb_register(&skel_driver);
	if (result)
		pr_err("usb_register failed. Error number %d\n", result);

	return result;
}

static
void __exit m_exit(void)
{
	usb_deregister(&skel_driver);
}


module_init(m_init);
module_exit(m_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("d0u9");
MODULE_DESCRIPTION("USB Driver skel");
