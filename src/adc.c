#include "adc.h"
#include "fft.h"

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

#define ADC_NODE DT_NODELABEL(adc1)
#define ADC_CHANNEL_NODE DT_CHILD(ADC_NODE, channel_0)

static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);
static const struct adc_channel_cfg adc_chann =
    ADC_CHANNEL_CFG_DT(DT_CHILD(ADC_NODE, channel_0));

enum adc_action adc_sleep(
    const struct device *dev,
    const struct adc_sequence *seq,
    uint16_t idx
)
{
    for (uint16_t i = 0; i < 128; i++)
        __asm volatile("SEV");
    return ADC_ACTION_CONTINUE;
}


const struct adc_sequence_options options = {
    .extra_samplings = SAMPLES - 1,
    .interval_us = 0,
    .callback = adc_sleep,
};

uint16_t buf[SAMPLES];
struct adc_sequence adc_seq = {
    .buffer = buf,
    .buffer_size = sizeof(buf),
    .resolution = 12,
    .channels = BIT(adc_chann.channel_id),
    //.calibrate = true,
    .options = &options,
};

int adc_init(void)
{
    if (!device_is_ready(adc_dev))
    {
        printf("adc not ready, exiting\n");
        return -1;
    }

    if (adc_channel_setup(adc_dev, &adc_chann) < 0)
    {
        printf("error configuring adc channel\n");
        return -1;
    }
    return 0;
}

void adc_mv_conv(int32_t *v)
{
    int err = adc_raw_to_millivolts(
        adc_ref_internal(adc_dev), adc_chann.gain,
        adc_seq.resolution, v);

    if (err < 0)
        printf("error converting raw adc value\n");
}

int adc_sample(complex_t *data)
{
    int err = adc_read(adc_dev, &adc_seq);
    if (err < 0)
    {
        printf("adc error (%d)\n", err);
        return -1;
    }

    for (uint16_t i = 0; i < SAMPLES; i++)
    {
        int32_t v = buf[i];
        adc_mv_conv(&v);
        data[i].Re = v;
        data[i].Im = 0;
    }

    return 0;
}
