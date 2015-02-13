#include <arch/hw.h>
#include <arch/bathos-arch.h>
#include <avr/io.h>
#include <bathos/bathos.h>
#include <bathos/pipe.h>
#include <bathos/init.h>
#include <bathos/bitops.h>
#include <bathos/errno.h>
#include <bathos/delay.h>
#include <bathos/stdio.h>
#include <arch/adc.h>
#include <bathos/adc.h>

#define VRES_UV (5000000 / (1 << 10))

/* Order of this mapping is Arduino YUN / Linino ONE */
const uint8_t hw_idxs[] = {7, 6, 5, 4, 1, 0};

const uint32_t num_adc = ARRAY_SIZE(hw_idxs);

const struct bathos_dev_ops adc_ops;

struct bathos_dev __adc
	__attribute__((section(".bathos_devices"), aligned(2))) = {
	.name = "adc",
	.ops = &adc_ops,
};

int adc_ioctl(struct bathos_pipe *p, struct bathos_ioctl_data *data)
{
	struct adc_data *d = (struct adc_data *)data->data;
	uint32_t *v = (uint32_t*)d->val;
	uint8_t hw_idx;

	/* All adc's in avr have the same vref and the same flags */
	switch(data->code) {
		case IOCTL_ADC_RD_NUM:
			*v = num_adc;
			break;
		case IOCTL_ADC_ENABLE:
			adc_en();
			break;
		case IOCTL_ADC_DISABLE:
			adc_dis();
			break;
		case IOCTL_ADC_RD_ENABLED:
			*v = adc_enabled();
			break;
		case IOCTL_ADC_RD_FLAGS:
			*v = ADC_FLAG_NONE;
			break;
		case IOCTL_ADC_RD_VREF:
			*v = VRES_UV;
			break;
		case IOCTL_ADC_RD_VAL:
		{
			if (d->ch >= num_adc)
				return -EINVAL;
			hw_idx = hw_idxs[d->ch];
			adc_sel_in(hw_idx);
			adc_start();
			while (ADCSRA & (1 << ADSC));
			*v = adc_data();
			break;
		}
		default:
			return -EINVAL;
	}
	return 0;
}

const struct bathos_dev_ops PROGMEM adc_ops = {
	.ioctl = adc_ioctl,
};

int adc_init()
{
	/* set reference to Vcc */
	adc_set_ref(ADMUX_AVCC);

	/* set ADC prescalar to 128 - max resolution */
	adc_set_ps(0x7);

	/* disable all */
	adc_dis();

	return 0;
}

rom_initcall(adc_init);
