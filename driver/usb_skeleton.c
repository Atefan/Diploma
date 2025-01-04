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

	if (usb_submit_urb(dev->irq_urb, GFP_KERNEL))
		return -EIO;

	return 0;
}

static int skel_release(struct inode *inode, struct file *file)
{
	struct usb_skel *dev;

	dev = (struct usb_skel *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	usb_kill_urb(dev->irq_urb);
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

static void skel_write_bulk_callback(struct urb *urb)
{
	struct usb_skel *dev = urb->context;

	/* sync/async unlink faults aren't errors */
	if (urb->status && 
	    !(urb->status == -ENOENT || 
	      urb->status == -ECONNRESET ||
	      urb->status == -ESHUTDOWN)) {
		dev_dbg(&dev->interface->dev,
			"%s - nonzero write bulk status received: %d",
			__FUNCTION__, urb->status);
	}

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			urb->transfer_buffer, urb->transfer_dma);
}

static ssize_t skel_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_skel *dev;
	int retval = 0;
	struct urb *urb = NULL;
	char *urb_buf = NULL, *buf = NULL;
	unsigned int val;

	dev = (struct usb_skel *)file->private_data;

	if (count == 0)
		goto exit;

	buf = kzalloc(count + 1, GFP_KERNEL);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}

	if (copy_from_user(buf, user_buffer, count)) {
		retval = -EFAULT;
		goto error;
	}

	retval = kstrtouint(buf, 0, &val);
	if (retval < 0)
		goto error;

	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	urb_buf = usb_alloc_coherent(dev->udev, sizeof(val), GFP_KERNEL, &urb->transfer_dma);
	if (!urb_buf) {
		retval = -ENOMEM;
		goto error;
	}

	memcpy(urb_buf, &val, sizeof(val));

	usb_fill_bulk_urb(urb, dev->udev,
			  usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
			  &val, sizeof(val), skel_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		pr_err("%s - failed submitting write urb, error %d\n", __FUNCTION__, retval);
		goto error;
	}

	usb_free_urb(urb);
	kfree(buf);
exit:
	return count;

error:
	if (urb) {
		usb_free_coherent(dev->udev, count, urb_buf, urb->transfer_dma);
		usb_free_urb(urb);
	}
	if (buf)
		kfree(buf);
	return retval;
}

unsigned int skel_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct usb_skel *dev = filp->private_data;

	poll_wait(filp, &dev->wq, wait);
	if (atomic_dec_and_test(&dev->can_rd)) {
		pr_debug("Now fd can be read\n");
		mask |= POLLIN | POLLRDNORM;
	}

	return mask;
}

static struct file_operations skel_fops = {
	.owner =	THIS_MODULE,
	.read =		skel_read,
	.write =	skel_write,
	.poll =		skel_poll,
	.open =		skel_open,
	.release =	skel_release,
};

static struct usb_class_driver skel_class = {
	.name	= "usb/skel%d",
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
		msleep_interruptible(1000);

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