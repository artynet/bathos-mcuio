/*
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <bathos/stdio.h>
#include <bathos/pipe.h>

/* Each platform will export one pipe named "adc", driving the global adc
 * system.
 * ioctl must be called with bathos_ioctl_data->data filled with the
 * struct adc_data below.
 * To read channel-specific data, set ch id for the desired channel.
 */

struct adc_data {
	uint8_t ch; /* adc channel: used by ioctl >= IOCTL_ADC_RD_VAL */
	void *val;
};

/* These ioctls are for global adc control */
#define IOCTL_ADC_RD_NUM		0 /* Read number of existing adcs */
#define IOCTL_ADC_ENABLE		1 /* Global Enable */
#define IOCTL_ADC_DISABLE		2 /* Global Disable */
#define IOCTL_ADC_RD_ENABLED		3 /* Read global enable status */

/* Channel-specific ioctls */
#define IOCTL_ADC_RD_VAL		16 /* Read sampled value */
#define IOCTL_ADC_RD_FLAGS		17 /* Read flags */
#define ADC_FLAG_NONE			0
#define ADC_FLAG_SIGNED		(1 << 0)
#define IOCTL_ADC_RD_VREF		18 /* Read Vref */

#endif
