#ifndef __FFT_H
#define __FFT_H

#include <stdint.h>

typedef struct {
    int32_t Re;
    int32_t Im;
} complex_t;


/* fft.c */
void fft_compute(complex_t *data, int8_t *bins);
void preprocess(complex_t *data);
void bit_reversal(complex_t *data);
void fft(complex_t *data);
void scale(complex_t *data, int8_t *frame_buffer);
int32_t scaled_mul(int32_t a, int32_t b);

#endif
