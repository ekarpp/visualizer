#include "adc.h"
#include "fft.h"
#include "ssd1306.h"

#include <stdio.h>
#include <zephyr/kernel.h>

#define HEAP_SIZE 8192

K_HEAP_DEFINE(fft_heap, HEAP_SIZE);

int main(void)
{
    if (adc_init() < 0)
    {
        printf("error initializing ADB\n");
        return -1;
    }

    printf("adc init done\n");

    complex_t *data = k_heap_alloc(&fft_heap, SAMPLES * sizeof(complex_t), K_MSEC(1000));
    int8_t bins[BINS];
    for (int16_t i = 0; i < BINS; i++)
        bins[i] = 0x7F;

    if (ssd1306_init() < 0)
    {
        printf("error initializing SSD1306\n");
        return -1;
    }
    printf("ssd1306 init done\n");

    ssd1306_update_frame(bins);

    printf("start sampling\n");
    while (1) {
        if (adc_sample(data) < 0)
        {
            printf("adc error\n");
            return -1;
        }
        fft_compute(data, bins);
        ssd1306_update_frame(bins);
    }
	return 0;
}
