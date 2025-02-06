#include <stdint.h>
#include "stm32f1xx.h"

#include "main.h"

#define USARTi USART1
#define USART_SPEED 19200

void usart_init(void)
{
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9 | GPIO_CRH_MODE9, GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10 | GPIO_CRH_MODE10, GPIO_CRH_CNF10_0);

    SET_BIT(USARTi->CR1, USART_CR1_UE);

    uint32_t PPRE2 = APBPrescTable[READ_BIT(RCC->CFGR, RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos];
    uint32_t PCLK2 = SystemCoreClock >> PPRE2;

    WRITE_REG(USARTi->BRR, PCLK2 / USART_SPEED);
    SET_BIT(USARTi->CR1, USART_CR1_TE);
    SET_BIT(USARTi->CR1, USART_CR1_RE);
}

uint8_t usart_send(uint8_t byte)
{
    uint8_t count = 0;
    if (byte == '\n')
        count += usart_send('\r');
    while(READ_BIT(USARTi->SR, USART_SR_TXE) != USART_SR_TXE);
    WRITE_REG(USARTi->DR, byte);
    return count + 1;
}

uint8_t usart_read(void)
{
    while(READ_BIT(USARTi->SR, USART_SR_RXNE) != USART_SR_RXNE);
    return READ_REG(USARTi->DR);
}

int32_t _write(int32_t handle, char *data, int32_t len)
{
    int32_t count = 0;
    for (uint32_t i = 0; i < len; i++)
        count += usart_send(data[i]);
    return count;
}

int32_t _read(int32_t handle, char *data, int32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        data[i] = usart_read();
    return len;
}
