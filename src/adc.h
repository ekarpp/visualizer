#ifndef __ADC_H
#define __ADC_H

#include "fft.h"
#include <stdint.h>

#define SAMPLES (1 << SAMPLES_LG)

int adc_init(void);
void adc_mv_conv(int32_t *v);
int adc_sample(complex_t *data);

#endif
