#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"

#include "main.h"


void init(void)
{
    clock_init();
    usart_init();

    GPIOC->CRH      &=  ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);    // reset PC13
    GPIOC->CRH      |= (GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0); // config PC13
    GPIOC->BSRR = GPIO_BSRR_BS13;

    i2c_init();
    ssd1306_init();
    adc_init();
}

void clock_init(void)
{
    // latency 2, when 48MHz < sysclock < 72 MHz
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);

    // APB1 clock (I2C), 60 MHz / 2 = 30 MHz < 36 MHz
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);

    // APB2 clock (ADC/USART), 60 MHz / 2 = 30 MHz
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);

    // ADC clock, 30 MHz / 6 = 5 MHz
    MODIFY_REG(RCC->CFGR, RCC_CFGR_ADCPRE, RCC_CFGR_ADCPRE_DIV6);

    // use HSI
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_PLLSRC);
    // 15 * 8MHz / 2 = 60 MHz
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMULL, RCC_CFGR_PLLMULL15);

    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);

    SystemCoreClockUpdate();

    // enable I2C1 clock, ADC1, USART1, GPIO A/B/C clocks
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);

    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);

    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);

    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
}

void sleep(uint32_t ms)
{
    const uint64_t clk = SystemCoreClock;
    uint64_t n = clk * ms / 1000;
    n -= 256;
    n /= 16;
    for (uint64_t i = 0; i < n; i++)
	__asm volatile("SEV");
}

int main(void)
{
    init();

    complex_t ADC_data[SAMPLES] = {};
    int16_t frame_buffer[SAMPLES/2 - 1] = {0};

    while (1)
    {
	adc_sample(ADC_data);

	GPIOC->BSRR = GPIO_BSRR_BR13;
	bit_reversal(ADC_data);
	fft(ADC_data);
	scale(ADC_data, frame_buffer);
	GPIOC->BSRR = GPIO_BSRR_BS13;

	ssd1306_update_frame(frame_buffer);
    }

    return 0;
}
