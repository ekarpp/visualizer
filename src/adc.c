#include <stdint.h>
#include "stm32f1xx.h"

#include "main.h"

#define ADCi ADC1

void adc_init(void)
{
    CLEAR_BIT(ADCi->CR2, ADC_CR2_CONT);

    SET_BIT(ADCi->CR2, ADC_CR2_ADON);
    sleep(10);
    // wait two ADC cycles
    // wait t_stab before exit
    SET_BIT(ADCi->CR2, ADC_CR2_CAL);
    sleep(10);
}


void adc_sample(complex_t *data)
{
    int32_t acc = 0;
    for (uint16_t i = 0; i < SAMPLES; i++)
    {
	SET_BIT(ADCi->CR2, ADC_CR2_ADON);
	while (READ_BIT(ADCi->SR, ADC_SR_EOC) != ADC_SR_EOC);

	data[i].Re = READ_BIT(ADCi->DR, ADC_DR_DATA);
	acc += data[i].Re;
	data[i].Im = 0;
    }
    acc /= SAMPLES;
    for (uint16_t i = 0; i < SAMPLES; i++)
	data[i].Re -= acc;
}
