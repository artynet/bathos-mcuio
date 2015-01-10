/*
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 */

#include <arch/hw.h>
#include <arch/bathos-arch.h>
#include <arch/gpio.h>
#include <bathos/bathos.h>
#include <bathos/pipe.h>
#include <bathos/init.h>
#include <bathos/errno.h>
#include <bathos/delay.h>
#include <bathos/stdio.h>
#include <bathos/adc.h>
#include <mach/hw.h>

int adc_init()
{
	int i;
	int port;

	regs[REG_SCGC6] |= SIM_SCGC6_ADC0_MASK;

	for (i = 0; i < num_adc; i++) {
		port = GPIO_PORT(adcs[i].hw_idx);
		regs[REG_SIM_SCGC5] |= PORT_CG(port);
	}

	/* FIXME define these flags in mach/hw.h */
	regs[ADC0_CFG1] = 0x9f;
	regs[ADC0_CFG2] = 0x05;

	return 0;
}

core_initcall(adc_init);

void adc_en()
{
}

void adc_dis()
{
}

int adc_enabled()
{
	return 1;
}

uint32_t adc_sample(const struct adc *adc)
{
	regs[ADC0_SC1A] = adc->hw_idx;
	while ((regs[ADC0_SC1A] & (1 << 7)) == 0);
	return regs[ADC0_RA];
}
