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



static const char ____versions[]
__used __section("__versions") =
	"\x18\x00\x00\x00\x1d\x56\x06\x1b"
	"usb_alloc_urb\0\0\0"
	"\x1c\x00\x00\x00\x48\x9f\xdb\x88"
	"__check_object_size\0"
	"\x18\x00\x00\x00\x7e\xb0\xe4\xb3"
	"usb_free_urb\0\0\0\0"
	"\x18\x00\x00\x00\xc2\x9c\xc4\x13"
	"_copy_from_user\0"
	"\x1c\x00\x00\x00\xb5\x09\x5f\x96"
	"usb_alloc_coherent\0\0"
	"\x1c\x00\x00\x00\x4d\x00\xc4\x97"
	"usb_register_driver\0"
	"\x14\x00\x00\x00\xea\x41\x6c\x3b"
	"kstrtouint\0\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x14\x00\x00\x00\x44\x43\x96\xe2"
	"__wake_up\0\0\0"
	"\x1c\x00\x00\x00\x4b\x18\x29\xf8"
	"__dynamic_dev_dbg\0\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x1c\x00\x00\x00\xfe\xa7\x65\x13"
	"usb_find_interface\0\0"
	"\x24\x00\x00\x00\xeb\xed\xc7\x93"
	"usb_find_common_endpoints\0\0\0"
	"\x24\x00\x00\x00\x97\x70\x48\x65"
	"__x86_indirect_thunk_rax\0\0\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x14\x00\x00\x00\xd8\x4e\xc9\x8b"
	"usb_put_dev\0"
	"\x18\x00\x00\x00\x41\x04\x0d\x86"
	"usb_bulk_msg\0\0\0\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x20\x00\x00\x00\x5f\x69\x96\x02"
	"refcount_warn_saturate\0\0"
	"\x14\x00\x00\x00\xcf\x54\xb0\x36"
	"usb_get_dev\0"
	"\x18\x00\x00\x00\x49\x9c\x1c\x3e"
	"usb_submit_urb\0\0"
	"\x14\x00\x00\x00\xe6\x97\x63\x64"
	"_dev_info\0\0\0"
	"\x14\x00\x00\x00\xf9\x8a\x89\x1e"
	"_dev_err\0\0\0\0"
	"\x1c\x00\x00\x00\x2f\x32\xc0\x30"
	"usb_free_coherent\0\0\0"
	"\x1c\x00\x00\x00\x63\xa5\x03\x4c"
	"random_kmalloc_seed\0"
	"\x1c\x00\x00\x00\xe1\x5f\x9f\xc2"
	"usb_register_dev\0\0\0\0"
	"\x18\x00\x00\x00\x47\x31\x61\x1b"
	"usb_deregister\0\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x18\x00\x00\x00\xe1\xbe\x10\x6b"
	"_copy_to_user\0\0\0"
	"\x20\x00\x00\x00\x54\xea\xa5\xd9"
	"__init_waitqueue_head\0\0\0"
	"\x1c\x00\x00\x00\x65\x62\xf5\x2c"
	"__dynamic_pr_debug\0\0"
	"\x1c\x00\x00\x00\x4e\x55\x42\x38"
	"usb_deregister_dev\0\0"
	"\x18\x00\x00\x00\x15\x61\xca\xc2"
	"usb_kill_urb\0\0\0\0"
	"\x18\x00\x00\x00\x19\x4e\x00\x34"
	"kmalloc_trace\0\0\0"
	"\x14\x00\x00\x00\x45\x3a\x23\xeb"
	"__kmalloc\0\0\0"
	"\x18\x00\x00\x00\x52\x57\xa3\x91"
	"kmalloc_caches\0\0"
	"\x18\x00\x00\x00\x3a\x0a\xd8\xfc"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v1A86p7523d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "3A43CFAC87223B8C7DE2BD2");
