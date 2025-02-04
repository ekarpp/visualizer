#include <stdint.h>
#include "stm32f1xx.h"

#include "main.h"
#include "tables.h"

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
		twiddle.Re = sin_table[i * sets + SAMPLES / 4];
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

void scale(complex_t *data, uint16_t *frame_buffer)
{
    data++;

    uint64_t mx = 0;
    for (uint16_t i = 0; i < SAMPLES / 2 - 1; i++)
    {
	uint64_t tmp = (uint64_t) ABS(data[i].Re) * ABS(data[i].Re);
	tmp += (uint64_t) ABS(data[i].Im) * ABS(data[i].Im);
	mx = MAX(mx, tmp);
    }
    for (uint16_t i = 0; i < SAMPLES / 2 - 1; i++)
    {
	uint64_t tmp = (uint64_t) ABS(data[i].Re) * ABS(data[i].Re);
	tmp += (uint64_t) ABS(data[i].Im) * ABS(data[i].Im);
	frame_buffer[i] = tmp * 128 / mx;
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
