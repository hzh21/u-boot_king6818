/*
 * (C) Copyright 2017 FriendlyElec Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
 *
 * (C) Copyright 2016 Nexell / Adapted for Rongpin King6818
 *
 * SPDX-License-Identifier:      GPL-2.0+
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <linux/sizes.h>

/*-----------------------------------------------------------------------
 * u-boot-2016.01
 */
#define CONFIG_SYS_LDSCRIPT	"arch/arm/cpu/armv8/u-boot.lds"
#define CONFIG_MACH_S5P6818	1

/*-----------------------------------------------------------------------
 * System memory Configuration
 */
#define CONFIG_SYS_TEXT_BASE		0x43C00000
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_MEM_SIZE		0x40000000
#define CONFIG_SYS_RESERVE_MEM_SIZE	0x02500000 /* 37MB */
#define CFG_SYS_SDRAM_BASE		0x40000000
#define CFG_SYS_SDRAM_SIZE		(CONFIG_SYS_MEM_SIZE - CONFIG_SYS_RESERVE_MEM_SIZE)

/* 核心加载地址定义 */
#define CONFIG_SYS_LOAD_ADDR		0x48000000
#define KERNEL_DTB_ADDR		        0x42000000  /* 修正 DTB 地址，防止与内核重叠 */
#define CONFIG_FB_ADDR			0x46000000
#define BMP_LOAD_ADDR		        0x47000000

/* AARCH64 相关 */
#define COUNTER_FREQUENCY		200000000
#define CPU_RELEASE_ADDR		CONFIG_SYS_INIT_SP_ADDR

/*-----------------------------------------------------------------------
 * System initialize options
 */
#define CONFIG_SYS_HZ			1000
#define CONFIG_ZERO_BOOTDELAY_CHECK

/*-----------------------------------------------------------------------
 *	U-Boot Environments
 */
#define CONFIG_BOOTDELAY		1  /* 给 1 秒机会进入命令行 */

/*-----------------------------------------------------------------------
 * allow to overwrite serial and ethaddr
 */
#define CONFIG_SYS_HUSH_PARSER
#ifdef CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#endif

/*-----------------------------------------------------------------------
 * Etc Command definition
 */
#define CONFIG_CMD_IMI
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_SUPPORT_RAW_INITRD	1
#define CONFIG_REVISION_TAG

/*-----------------------------------------------------------------------
 * serial console configuration
 */
#define CONFIG_S5P6818_SERIAL_INDEX		0
#define CONFIG_S5P6818_SERIAL_CLOCK		50000000
#define CONFIG_S5P6818_SERIAL_PORT		(void *)0xC00A1000 /* 显式指定 UART0 地址 */

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE \
		{ 9600, 19200, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * SD/MMC 基础设置
 */
#define CONFIG_ROOT_DEV			2   /* 默认从 eMMC (2) 启动 */
#define CONFIG_BOOT_PART		7   /* 你的内核 Image 和 DTB 放在第 7 分区 */
#define CONFIG_ROOT_PART		8   /* 你的 rootfs 放在第 8 分区 */

/*-----------------------------------------------------------------------
 * ENV 启动命令核心适配
 */
#define CONFIG_SYS_EXTRA_ENV_RELOC

/* 启动参数适配 Docker (cgroup 支持) */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0xffffffffffffffff\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"kernel=Image\0" \
	"dtb_name=s5p6818-king6818.dtb\0" \
	"loadaddr=0x48000000\0" \
	"dtb_addr=" __stringify(KERNEL_DTB_ADDR) "\0" \
	\
	/* 基础启动参数（不含 root） */ \
	"base_args=console=ttyAMA0,115200n8 rootwait rw cgroup_enable=memory swapaccount=1\0" \
	\
	/* 通用加载与启动脚本 */ \
	"mmcboot_logic=if ext4load mmc ${devnum}:${bootpart} ${loadaddr} ${kernel}; then " \
			"ext4load mmc ${devnum}:${bootpart} ${dtb_addr} ${dtb_name}; " \
			"booti ${loadaddr} - ${dtb_addr}; " \
		"fi\0" \
	\
	/* SD 卡启动路径 (mmc 0, 分区通常是 1 和 2) */ \
	"sd_boot=echo Checking SD Card...; " \
		"if mmc dev 0; then " \
			"setenv devnum 0; setenv bootpart 1; setenv rootpart 2; " \
			"setenv bootargs ${base_args} root=/dev/mmcblk1p${rootpart}; " \
			"run mmcboot_logic; " \
		"fi\0" \
	\
	/* eMMC 启动路径 (mmc 2, 荣品结构 P7 和 P8) */ \
	"emmc_boot=echo Checking eMMC...; " \
		"if mmc dev 2; then " \
			"setenv devnum 2; setenv bootpart 7; setenv rootpart 8; " \
			"setenv bootargs ${base_args} root=/dev/mmcblk0p${rootpart}; " \
			"run mmcboot_logic; " \
		"fi\0" \
	\
	/* 最终引导命令：先跑 SD，跑不通（或没卡）再跑 eMMC */ \
	"bootcmd=run sd_boot; run emmc_boot\0"

#endif /* __CONFIG_H__ */
