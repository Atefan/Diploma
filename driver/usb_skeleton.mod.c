#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x4d0c2afe, "usb_bulk_msg" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x2cf56265, "__dynamic_pr_debug" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x7f9e4b71, "usb_deregister" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x3b6c41ea, "kstrtouint" },
	{ 0x6568b1e4, "usb_alloc_urb" },
	{ 0x25f2604b, "usb_alloc_coherent" },
	{ 0x7669fb50, "usb_submit_urb" },
	{ 0xcdb62c38, "usb_find_interface" },
	{ 0x296695f, "refcount_warn_saturate" },
	{ 0xa3db09f6, "_dev_info" },
	{ 0xbf54de63, "kmalloc_caches" },
	{ 0x8874d531, "kmalloc_trace" },
	{ 0x73fda3a1, "usb_get_dev" },
	{ 0x9a19d93f, "usb_control_msg" },
	{ 0x93c7edeb, "usb_find_common_endpoints" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0xda4ddab, "kthread_create_on_node" },
	{ 0x553fbb5d, "wake_up_process" },
	{ 0x20fadddc, "usb_register_dev" },
	{ 0x8bfa8a13, "_dev_err" },
	{ 0xdc4fde3d, "kthread_stop" },
	{ 0x2736aee3, "usb_kill_urb" },
	{ 0x8e8cf1f, "usb_deregister_dev" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7426d9c5, "usb_register_driver" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5d58db6b, "usb_put_dev" },
	{ 0x7dbcf780, "usb_free_coherent" },
	{ 0xcb9189e2, "usb_free_urb" },
	{ 0x37a0cba, "kfree" },
	{ 0xd7354f77, "__dynamic_dev_dbg" },
	{ 0x6b910e0, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v03F2p03F2d*dc*dsc*dp*ic*isc*ip*in*");
