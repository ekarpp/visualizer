#ifndef __FFT_H
#define __FFT_H

#include <stdint.h>

typedef struct {
    int16_t Re;
    int16_t Im;
} complex_t;


/* fft.c */
void fft_compute(complex_t *data, int8_t *bins);
void preprocess(complex_t *data);
void bit_reversal(complex_t *data);
void fft(complex_t *data);
void scale(complex_t *data, int8_t *frame_buffer);
int16_t scaled_mul(int16_t a, int16_t b);

#endif
