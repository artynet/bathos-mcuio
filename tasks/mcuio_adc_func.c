/*
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 */

#include <arch/hw.h>
#include <bathos/adc.h>
#include <bathos/bathos.h>
#include <bathos/pipe.h>
#include <bathos/errno.h>
#include <bathos/bitops.h>
#include <bathos/string.h>
#include <bathos/init.h>
#include <tasks/mcuio.h>

#include "mcuio-function.h"

#define MCUIO_ADC_DEVICE 0x1235
#define MCUIO_ADC_VENDOR 0x0001

static const unsigned int PROGMEM u32_length = 4;
static const unsigned int PROGMEM adcs_ctrl_length = 0xfc0;
static const unsigned int PROGMEM adc_chip_ctrl_length = (0x40 - 0x8);

static const struct mcuio_func_descriptor PROGMEM adc_descr = {
	.device = MCUIO_ADC_DEVICE,
	.vendor = MCUIO_ADC_VENDOR,
	.rev = 0,
	/* ADCs class */
	.class = 0x00000003,
};

static const unsigned int PROGMEM adc_descr_length = sizeof(adc_descr);

extern struct mcuio_function adc;
static struct mcuio_function_runtime adc_rt;

static int adc_ctrl_rddw(const struct mcuio_range *r, unsigned offset,
			  uint32_t *out, int fill)
{
	unsigned idx = offset / 0x40;
	unsigned reg = offset % 0x40;
	char id[4];
	struct bathos_pipe *p;
	int ret = 0;
	struct adc_data ad;
	struct bathos_ioctl_data d;
	ad.val = out;
	ad.ch = idx;
	d.data = &ad;

	p = pipe_open("adc", BATHOS_MODE_INPUT, NULL);

	if (!p)
		return -EIO;


	switch(reg) {

		case 0x00: /* identifier, return AX */
			/* FIXME: labels should be configurable.
			* Here, labels are taken from adcs index */
			id[0] = 'A';
			id[1] = (idx >= 10) ? '0' + (idx / 10) : '0' + idx;
			id[2] = (idx >= 10) ? '0' + (idx % 10) : '\0';
			id[3] = '\0';
			memcpy(out, id, sizeof(id));
			flip4((uint8_t*)out);
			break;

		case 0x04: /* flags
			    * bit 0: signed */
			d.code = IOCTL_ADC_RD_FLAGS;
			ret = pipe_ioctl(p, &d);
			break;

		case 0x08: /* voltage resolution (in uV) */
			d.code = IOCTL_ADC_RD_VREF;
			ret = pipe_ioctl(p, &d);
			break;

		case 0x0c: /* data */
			d.code = IOCTL_ADC_RD_VAL;
			ret = pipe_ioctl(p, &d);
			break;

		default:
			ret = -EINVAL;
	}

	pipe_close(p);

	return ret;
}

const struct mcuio_range_ops PROGMEM adc_ctrl_ops = {
	.rd = { NULL, NULL, adc_ctrl_rddw, NULL, },
	.wr = { NULL, NULL, NULL, NULL, },
};

static int adc_chip_ctrl_rddw(const struct mcuio_range *r, unsigned offset,
			  uint32_t *out, int fill)
{
	struct bathos_pipe *p;
	struct adc_data ad;
	struct bathos_ioctl_data d;
	int ret = 0;
	p = pipe_open("adc", BATHOS_MODE_INPUT, NULL);

	if (!p)
		return -EIO;

	d.data = &ad;
	ad.val = out;

	switch (offset) {
		case (0x08 - 0x08):
			d.code = IOCTL_ADC_RD_NUM;
			break;
		/* FIXME dword 0x10, max value for period multiplier
		 * (asynchronous flow of adc inputs unsupported yet) */
		case (0x14 - 0x8):
			d.code = IOCTL_ADC_RD_ENABLED;
			break;
		default:
			ret = -EINVAL;
	}
	if (!ret)
		ret = pipe_ioctl(p, &d);
	pipe_close(p);
	return ret;
}

static int adc_chip_ctrl_wrdw(const struct mcuio_range *r, unsigned offset,
			 const uint32_t *__in, int fill)
{
	struct bathos_pipe *p;
	struct bathos_ioctl_data d;
	int ret = 0;

	if (offset != 0x14 - 0x08)
		return -EINVAL;

	p = pipe_open("adc", BATHOS_MODE_OUTPUT, NULL);

	if (!p)
		return -EIO;

	d.code = (*__in) ? IOCTL_ADC_ENABLE : IOCTL_ADC_DISABLE;
	ret = pipe_ioctl(p, &d);
	pipe_close(p);
	return ret;
}

const struct mcuio_range_ops PROGMEM adc_chip_ctrl_ops = {
	.rd = { NULL, NULL, adc_chip_ctrl_rddw, NULL, },
	.wr = { NULL, NULL, adc_chip_ctrl_wrdw, NULL, },
};

static const struct mcuio_range PROGMEM adc_ranges[] = {
	/* ADC func descriptor */
	{
		.start = 0,
		.length = &adc_descr_length,
		.rd_target = &adc_descr,
		.ops = &default_mcuio_range_ro_ops,
	},
	/* ADC general control */
	{
		.start = 0x008,
		.length = &adc_chip_ctrl_length,
		.rd_target = NULL,
		.ops = &adc_chip_ctrl_ops,
	},

	/* dwords starting from 0x40, ADCs status and control */
	{
		.start = 0x40,
		.length = &adcs_ctrl_length,
		.rd_target = NULL,
		.ops = &adc_ctrl_ops,
	},
};

declare_mcuio_function(adc, adc_ranges, NULL, NULL, &adc_rt);
