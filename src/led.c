#include <stdint.h>
#include "stm32f1xx.h"

#include "main.h"

void led_init(void)
{
    MODIFY_REG(GPIOC->CRH, GPIO_CRH_MODE13, GPIO_CRH_MODE13);
    led_off();
}

void led_on(void)
{
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR13);
}

void led_off(void)
{
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS13);
}
