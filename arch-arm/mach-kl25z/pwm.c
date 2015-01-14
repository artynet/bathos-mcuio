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
#include <bathos/pwm.h>
#include <bathos/bitops.h>
#include <bathos/errno.h>
#include <bathos/delay.h>
#include <bathos/stdio.h>

#define PWM_DEF_PERIOD 60000 /* FIXME: compute from CPU_FREQ */

struct kl25z_pwm_t {
	int tim_id;
	int ch_id;
	int gpio;
};

static struct pwm_ops __ops;

/* FIXME: pwms list should be configurable. Here, freedog board mapping is
 * temporarly fixed in the src */

static struct kl25z_pwm_t __kl25z_pwms[] = {
	{1, 0, GPIO_NR(PORTA, 12)},	/* D3 */
	{0, 1, GPIO_NR(PORTA, 4)},	/* D4 */
	{0, 2, GPIO_NR(PORTA, 5)},	/* D5 */
	{0, 4, GPIO_NR(PORTC, 8)},	/* D6 */
	{1, 1, GPIO_NR(PORTA, 13)},	/* D8 */
	{0, 5, GPIO_NR(PORTD, 5)},	/* D9 */
	{0, 0, GPIO_NR(PORTD, 0)},	/* D10 */
};

/* pwms are all the same, and pwms array is inited in pwm_init */
struct pwm pwms[ARRAY_SIZE(__kl25z_pwms)] = {
	{.label = "D3"},
	{.label = "D4"},
	{.label = "D5"},
	{.label = "D6"},
	{.label = "D8"},
	{.label = "D9"},
	{.label = "D10"},
};

const uint32_t num_pwm = ARRAY_SIZE(pwms);

static int pwm_init(void)
{
	int i;
	regs[REG_SCGC6] |=
		SIM_SCGC6_TPM0_MASK |
		SIM_SCGC6_TPM1_MASK |
		SIM_SCGC6_TPM2_MASK;
	for (i = 0; i < num_pwm; i++) {
		pwms[i].ops = __ops;
		pwms[i].tim_res_ns = 333;
		pwms[i].tim_max_mul = 65536;
		pwms[i].priv = &__kl25z_pwms[i];
	}
	return 0;
}

core_initcall(pwm_init);

/* Status (enabled/disabled) of each pwm (max 32) */
static uint32_t pwm_stat = 0;
static uint32_t gpio_orig_dir = 0;

int pwm_enabled(int idx)
{
	return (pwm_stat & (1 << idx)) ? 1 : 0;
}

int pwm_set_period(struct pwm *pwm, uint32_t val);

int pwm_en(struct pwm *pwm)
{
	struct kl25z_pwm_t *priv = (struct kl25z_pwm_t *)pwm->priv;
	int output;
	int id = pwm_id(pwm);
	gpio_get_dir_af(priv->gpio, &output, NULL, NULL);
	if (output)
		gpio_orig_dir |= (1 << id);
	else
		gpio_orig_dir &= ~(1 << id);
	gpio_dir_af(priv->gpio, 1, 0, 3);
	pwm_stat |= (1 << id);
	regs[TPM_SC(priv->tim_id)] = 0x0c; /* CMOD=1, PS=4 (divide by 16) */
	regs[TPM_CN_SC(priv->tim_id, priv->ch_id)] =
		0x28; /* Edge aligned PWM - High-true pulses*/
	pwm_set_period(pwm, PWM_DEF_PERIOD);
	return 0;
}

void pwm_dis(struct pwm *pwm)
{
	struct kl25z_pwm_t *priv = (struct kl25z_pwm_t *)pwm->priv;
	int output = 0;
	int id = pwm_id(pwm);
	regs[TPM_CN_SC(priv->tim_id, priv->ch_id)] = 0;
	output = gpio_orig_dir & (1 << id) ? 1 : 0;
	gpio_dir_af(priv->gpio, output, 0, 0);
	pwm_stat &= ~(1 << id);
}

uint32_t pwm_get_period(struct pwm *pwm)
{
	struct kl25z_pwm_t *priv = (struct kl25z_pwm_t *)pwm->priv;
	return regs[TPM_MOD(priv->tim_id)];
}

uint32_t pwm_get_duty(struct pwm *pwm)
{
	struct kl25z_pwm_t *priv = (struct kl25z_pwm_t *)pwm->priv;
	return regs[TPM_CN_V(priv->tim_id, priv->ch_id)] & 0xffff;
}

int pwm_set_period(struct pwm *pwm, uint32_t val)
{
	struct kl25z_pwm_t *priv = (struct kl25z_pwm_t *)pwm->priv;
	regs[TPM_MOD(priv->tim_id)] = val & 0xffff;
	return 0;
}

int pwm_set_duty(struct pwm *pwm, uint32_t val)
{
	struct kl25z_pwm_t *priv = (struct kl25z_pwm_t *)pwm->priv;
	regs[TPM_CN_V(priv->tim_id, priv->ch_id)] = val & 0xffff;
	return 0;
}

static struct pwm_ops __ops = {
	.enable = pwm_en,
	.disable = pwm_dis,
	.get_period = pwm_get_period,
	.get_duty = pwm_get_duty,
	.set_period = pwm_set_period,
	.set_duty = pwm_set_duty,
};
