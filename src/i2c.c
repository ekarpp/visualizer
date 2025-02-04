#include <stdint.h>
#include "stm32f1xx.h"

#include "main.h"

#define I2Ci I2C1
//#define I2C_SPEED 1000000
#define I2C_SPEED 100000

void i2c_init(void)
{
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF6 | GPIO_CRL_MODE6, GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF7 | GPIO_CRL_MODE7, GPIO_CRL_MODE7 | GPIO_CRL_CNF7);

    CLEAR_BIT(I2Ci->CR1, I2C_CR1_PE);
    SET_BIT(I2Ci->CR1, I2C_CR1_SWRST);
    CLEAR_BIT(I2Ci->CR1, I2C_CR1_SWRST);

    uint32_t PCLK1 = SystemCoreClock >> 8;
    //>> MAX(0, (READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos) - 3);
    MODIFY_REG(I2Ci->CR2, I2C_CR2_FREQ, PCLK1 << I2C_CR2_FREQ_Pos);

    uint32_t CCR = (PCLK1 - 1) / (I2C_SPEED * (9 + 16)) + 1;
    CCR <<= I2C_CCR_CCR_Pos;
    CCR &= I2C_CCR_CCR;
    CCR |= I2C_CCR_DUTY;
    CCR |= I2C_CCR_FS;
    MODIFY_REG(I2Ci->CCR, I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR, CCR);

    uint32_t TRISE = PCLK1 * 300 / 1000 + 1;
    MODIFY_REG(I2Ci->TRISE, I2C_TRISE_TRISE, TRISE);

    SET_BIT(I2Ci->CR1, I2C_CR1_PE);
}

void i2c_start(uint8_t i2c_address)
{
    while (READ_BIT(I2Ci->SR2, I2C_SR2_BUSY) != 0x0);
    CLEAR_BIT(I2Ci->CR1, I2C_CR1_POS);
    SET_BIT(I2Ci->CR1, I2C_CR1_START);

    while (READ_BIT(I2Ci->SR1, I2C_SR1_SB) != I2C_SR1_SB);
    WRITE_REG(I2Ci->DR, i2c_address);

    while (READ_BIT(I2Ci->SR1, I2C_SR1_ADDR) != I2C_SR1_ADDR);
    uint16_t clear = 0x0;
    clear = READ_REG(I2Ci->SR1);
    clear = READ_REG(I2Ci->SR2);
    UNUSED(clear);
}

void i2c_send(uint8_t byte)
{
    while (READ_BIT(I2Ci->SR1, I2C_SR1_TXE) != I2C_SR1_TXE)
    {
	if (READ_BIT(I2Ci->SR1, I2C_SR1_AF) == I2C_SR1_AF)
	    continue;
    }

    WRITE_REG(I2Ci->DR, byte);
}

void i2c_stop(void)
{
    while (READ_BIT(I2Ci->SR1, I2C_SR1_TXE) != I2C_SR1_TXE);
    SET_BIT(I2Ci->CR1, I2C_CR1_STOP);
}
