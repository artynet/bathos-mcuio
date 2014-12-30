/*
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 */

#ifndef __GPIO_BITBANG_I2C_H__
#define __GPIO_BITBANG_I2C_H__

#ifdef CONFIG_MACH_KL25Z

#define setsda __kl25z_bitbang_setsda
#define setscl __kl25z_bitbang_setscl

static char af_sda_inited = 0;
static char af_scl_inited = 0;

static inline void __kl25z_bitbang_setsda(int v)
{
	if (af_sda_inited)
		gpio_dir(GPIO_SDA, 1, v);
	else {
		af_sda_inited = 1;
		gpio_dir_af(GPIO_SDA, 1, v, 1);
	}
	if (v)
		gpio_dir(GPIO_SDA, 0, 1);
}

static inline void __kl25z_bitbang_setscl(int v)
{
	if (af_scl_inited)
		gpio_dir_af(GPIO_SCL, 1, v, 1);
	else {
		af_scl_inited = 1;
		gpio_dir_af(GPIO_SCL, 1, v, 1);
	}
	if (v)
		gpio_dir(GPIO_SCL, 0, 1);
}

#endif

#endif /* __GPIO_BITBANG_I2C_H__ */
