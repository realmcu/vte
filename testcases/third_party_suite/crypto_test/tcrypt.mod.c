#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8a8ff15d, "module_layout" },
	{ 0x6980fe91, "param_get_int" },
	{ 0xff964b25, "param_set_int" },
	{ 0x108e8985, "param_get_uint" },
	{ 0x3285cc48, "param_set_uint" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0x90a1004a, "crypto_has_alg" },
	{ 0x5086ac3a, "alg_test" },
	{ 0x9d669763, "memcpy" },
	{ 0x7d11c268, "jiffies" },
	{ 0x9196752d, "mem_map" },
	{ 0x8fe393d4, "crypto_destroy_tfm" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0x5f754e5a, "memset" },
	{ 0x6898a756, "sg_init_table" },
	{ 0x5bfa34b3, "crypto_alloc_base" },
	{ 0xea147363, "printk" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

