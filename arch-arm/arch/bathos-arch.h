#ifndef __BATHOS_ARCH_H__
#define __BATHOS_ARCH_H__

#define PROGMEM

#include <generated/autoconf.h>
#include <bathos/string.h>

#ifdef CONFIG_CPU_CORTEX_M0
#include <cpu-cortex-m0/nvic.h>
#endif

static inline void *memcpy_p(void *dst, const void *src, int size)
{
        return memcpy(dst, src, size);
}

static inline void flip4(uint8_t *x)
{
        uint8_t t;
        t = x[0]; x[0] = x[3]; x[3] = t;
        t = x[1]; x[1] = x[2]; x[2] = t;
}

#endif /* __BATHOS_ARCH_H__ */
