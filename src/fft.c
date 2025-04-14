#include <stdint.h>

#include "adc.h"
#include "fft.h"
#include "ssd1306.h"
#include "tables.h"

#define ABS(x)    (((x) < 0) ? (-(x)) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))

void fft_compute(complex_t *data, int8_t *bins)
{
    preprocess(data);
    bit_reversal(data);
    fft(data);
    scale(data, bins);
}

void preprocess(complex_t *data)
{
    int32_t mean = 0;
    for (uint16_t i = 0; i < SAMPLES; i++)
        mean += data[i].Re;
    mean /= SAMPLES;
    for (uint16_t i = 0; i < SAMPLES; i++)
        data[i].Re -= mean;
}

void bit_reversal(complex_t *data)
{
    uint16_t reversal[SAMPLES];
    reversal[0] = 0;
    reversal[1] = 1;
    for (uint16_t k = 2; k <= SAMPLES_LG; k++)
    {
        const uint16_t r = 1 << k;
        const uint16_t l = r >> 1;

        for (uint16_t i = 0; i < l; i++)
            reversal[i] *= 2;
        for (uint16_t i = l; i < r; i++)
            reversal[i] = reversal[i - l] + 1;
    }
    for (uint16_t i = 1; i < SAMPLES; i++)
    {
        uint16_t j = reversal[i];
        if (j > i)
        {
            complex_t tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }
}

void fft(complex_t *data)
{
    uint32_t lg = 0;
    while (lg < SAMPLES_LG)
    {
        const uint32_t n = 1 << lg;
        const uint32_t sets = SAMPLES / (n << 1);
        complex_t *a = &data[0];
        for (uint32_t i = 0; i < n; i++)
        {
            complex_t twiddle;
            if (i != 0 && i != n / 2)
            {
                twiddle.Re = sin_table[(i * sets + SAMPLES / 4) % (SAMPLES / 2)];
                if (i*sets >= SAMPLES / 4)
                    twiddle.Re = -twiddle.Re;
                twiddle.Im = -sin_table[i * sets];
            }

            for (uint32_t j = 0; j < sets; j++)
            {
                complex_t *b = a + n;
                complex_t c; // = b * twiddle
                if (i == 0)
                {
                    // sin(0) == 0 && cos(0) == sin(pi/2) == 1
                    c.Re = b->Re;
                    c.Im = b->Im;
                }
                else if (i == n / 2)
                {
                    // sin(pi / 2) == 1 && cos(pi / 2) == sin(pi) == 0
                    c.Re = b->Im;
                    c.Im = -b->Re;
                }
                else
                {
                    c.Re = scaled_mul(b->Re, twiddle.Re)
                        - scaled_mul(b->Im, twiddle.Im);

                    c.Im = scaled_mul(b->Im, twiddle.Re)
                        + scaled_mul(b->Re, twiddle.Im);
                }

                b->Re = a->Re - c.Re;
                b->Im = a->Im - c.Im;
                a->Re += c.Re;
                a->Im += c.Im;

                a += 2*n;
            }
            a -= SAMPLES - 1;
        }
        lg++;
    }
}

void scale(complex_t *data, int8_t *bins)
{
    data++;

    const uint8_t step = (SAMPLES >> 1) / BINS;

    for (uint16_t i = 0; i < BINS; i++)
    {
        uint64_t tmp = 0;
        for (uint16_t j = 0; j < step; j++)
        {
            tmp += (uint64_t) ABS(data[i*step + j].Re) * ABS(data[i*step + j].Re);
            tmp += (uint64_t) ABS(data[i*step + j].Im) * ABS(data[i*step + j].Im);
        }
        tmp /= step;
        uint16_t lg2 = 0;
        while(tmp)
        {
            tmp >>= 1;
            lg2++;
        }
        bins[i] = MAX(0, (lg2 << 2) + weights[i] - 77);
    }
}

int32_t scaled_mul(int32_t a, int32_t b)
{
    int8_t negs = 0;
    if (a < 0)
        negs++;

    if (b < 0)
        negs++;

    uint64_t tmp = ABS(a);
    tmp *= ABS(b);
    tmp >>= 31;
    return (negs == 1) ? -(int32_t) tmp : (int32_t) tmp;
}
