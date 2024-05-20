#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8d42f378, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xb9347d89, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x22016404, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x867185f, __VMLINUX_SYMBOL_STR(iio_device_register) },
	{ 0x8f56ca4a, __VMLINUX_SYMBOL_STR(devm_request_threaded_irq) },
	{ 0x75a03033, __VMLINUX_SYMBOL_STR(platform_get_irq) },
	{ 0xb14e321c, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x822137e2, __VMLINUX_SYMBOL_STR(arm_heavy_mb) },
	{ 0x84ac7ec1, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xd67bd86a, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x3606f280, __VMLINUX_SYMBOL_STR(devm_ioremap_resource) },
	{ 0xf62a0f4, __VMLINUX_SYMBOL_STR(platform_get_resource) },
	{ 0xb99edfc6, __VMLINUX_SYMBOL_STR(regulator_get_voltage) },
	{ 0x149d818c, __VMLINUX_SYMBOL_STR(regulator_enable) },
	{ 0x46a4e5a1, __VMLINUX_SYMBOL_STR(devm_regulator_get) },
	{ 0x815588a6, __VMLINUX_SYMBOL_STR(clk_enable) },
	{ 0x68c6b359, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x7c9a7371, __VMLINUX_SYMBOL_STR(clk_prepare) },
	{ 0x810526c9, __VMLINUX_SYMBOL_STR(devm_clk_get) },
	{ 0xeb9dd4cc, __VMLINUX_SYMBOL_STR(devm_iio_device_alloc) },
	{ 0x2f135212, __VMLINUX_SYMBOL_STR(regulator_disable) },
	{ 0xb077e70a, __VMLINUX_SYMBOL_STR(clk_unprepare) },
	{ 0xb6e6d99d, __VMLINUX_SYMBOL_STR(clk_disable) },
	{ 0xece4091, __VMLINUX_SYMBOL_STR(iio_device_unregister) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

