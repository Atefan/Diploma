#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define USB_VENDOR_ID  0x1A86 // Example Vendor ID
#define USB_PRODUCT_ID 0x7523 // Example Product ID
#define READ_BUFFER_SIZE 512
#define WRITE_BUFFER_SIZE 512

struct usb_device_data {
    struct usb_device *udev;
    struct usb_interface *interface;
    struct urb *read_urb;
    struct urb *write_urb;
    unsigned char *read_buffer;
    unsigned char *write_buffer;
    spinlock_t lock;
};

static void usb_read_callback(struct urb *urb)
{
    struct usb_device_data *dev_data = urb->context;
    unsigned long flags;

    if (urb->status) {
        pr_err("Read URB failed with status: %d\n", urb->status);
        return;
    }

    spin_lock_irqsave(&dev_data->lock, flags);
    // Process the received data here
    pr_info("Read %d bytes from hardware\n", urb->actual_length);
    // Data is available in dev_data->read_buffer
    spin_unlock_irqrestore(&dev_data->lock, flags);

    // Resubmit the read URB
    usb_submit_urb(urb, GFP_ATOMIC);
}

static void usb_write_callback(struct urb *urb)
{
    if (urb->status) {
        pr_err("Write URB failed with status: %d\n", urb->status);
    } else {
        pr_info("Write URB completed successfully\n");
    }
}

static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    struct usb_device_data *dev_data;
    int retval;

    pr_info("USB device connected: Vendor=0x%x, Product=0x%x\n",
            id->idVendor, id->idProduct);

    // Allocate memory for device data
    dev_data = kzalloc(sizeof(*dev_data), GFP_KERNEL);
    if (!dev_data) {
        pr_err("Failed to allocate memory for device data\n");
        return -ENOMEM;
    }

    dev_data->udev = udev;
    dev_data->interface = interface;
    spin_lock_init(&dev_data->lock);

    // Allocate read buffer
    dev_data->read_buffer = kzalloc(READ_BUFFER_SIZE, GFP_KERNEL);
    if (!dev_data->read_buffer) {
        pr_err("Failed to allocate read buffer\n");
        retval = -ENOMEM;
        goto error;
    }

    // Allocate write buffer
    dev_data->write_buffer = kzalloc(WRITE_BUFFER_SIZE, GFP_KERNEL);
    if (!dev_data->write_buffer) {
        pr_err("Failed to allocate write buffer\n");
        retval = -ENOMEM;
        goto error;
    }

    // Set up read URB
    dev_data->read_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!dev_data->read_urb) {
        pr_err("Failed to allocate read URB\n");
        retval = -ENOMEM;
        goto error;
    }

    usb_fill_bulk_urb(dev_data->read_urb, udev,
                      usb_rcvbulkpipe(udev, 1),
                      dev_data->read_buffer, READ_BUFFER_SIZE,
                      usb_read_callback, dev_data);

    // Submit the read URB
    retval = usb_submit_urb(dev_data->read_urb, GFP_KERNEL);
    if (retval) {
        pr_err("Failed to submit read URB: %d\n", retval);
        goto error;
    }

    // Set up write URB
    dev_data->write_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!dev_data->write_urb) {
        pr_err("Failed to allocate write URB\n");
        retval = -ENOMEM;
        goto error;
    }

    usb_set_intfdata(interface, dev_data);
    return 0;

error:
    if (dev_data->read_buffer)
        kfree(dev_data->read_buffer);
    if (dev_data->write_buffer)
        kfree(dev_data->write_buffer);
    if (dev_data->read_urb)
        usb_free_urb(dev_data->read_urb);
    if (dev_data->write_urb)
        usb_free_urb(dev_data->write_urb);
    kfree(dev_data);
    return retval;
}

static void usb_disconnect(struct usb_interface *interface)
{
    struct usb_device_data *dev_data = usb_get_intfdata(interface);

    pr_info("USB device disconnected\n");

    if (dev_data) {
        usb_kill_urb(dev_data->read_urb);
        usb_kill_urb(dev_data->write_urb);
        usb_free_urb(dev_data->read_urb);
        usb_free_urb(dev_data->write_urb);
        kfree(dev_data->read_buffer);
        kfree(dev_data->write_buffer);
        kfree(dev_data);
    }
}

static int usb_write_to_device(struct usb_device_data *dev_data, const char *data, size_t len)
{
    unsigned long flags;
    size_t write_len;

    spin_lock_irqsave(&dev_data->lock, flags);
    write_len = min(len, (size_t)WRITE_BUFFER_SIZE);
    memcpy(dev_data->write_buffer, data, write_len);
    spin_unlock_irqrestore(&dev_data->lock, flags);

    usb_fill_bulk_urb(dev_data->write_urb, dev_data->udev,
                      usb_sndbulkpipe(dev_data->udev, 1),
                      dev_data->write_buffer, write_len,
                      usb_write_callback, dev_data);

    return usb_submit_urb(dev_data->write_urb, GFP_KERNEL);
}

static struct usb_device_id usb_id_table[] = {
    { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
    {}
};
MODULE_DEVICE_TABLE(usb, usb_id_table);

static struct usb_driver usb_driver = {
    .name = "usb_driver_example",
    .id_table = usb_id_table,
    .probe = usb_probe,
    .disconnect = usb_disconnect,
};

module_usb_driver(usb_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple USB Driver");
