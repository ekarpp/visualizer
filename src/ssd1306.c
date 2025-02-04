#include <stdint.h>
#include "stm32f1xx.h"

#include "ssd1306.h"
#include "main.h"

#define DRAW_VAR(vv)			\
    for (uint8_t i = 0; i < 8; i++)	\
    {                                   \
	if (vv >= 8)			\
	{				\
	    i2c_send(0xFF);		\
	    vv -= 8;			\
	}				\
	else if (vv > 0)		\
	{				\
	    i2c_send((1 << vv) - 1);	\
	    vv = 0;			\
	}				\
	else				\
	    i2c_send(0x0);		\
    }



void ssd1306_send_cmd(uint8_t byte)
{
    i2c_start(SSD1306_ADDRESS);
    i2c_send(0x00);
    i2c_send(byte);
    i2c_stop();
}

void ssd1306_init(void)
{
    const uint8_t init_seq[] = {
	SSD1306_DISP_OFF,
	SSD1306_SET_MUX, 0x3F,
	SSD1306_SET_DISP_OFFSET, 0x00,
	SSD1306_SET_START_LINE | 0x00,
	SSD1306_SEG_REMAP | 0x01,
	SSD1306_COM_SCAN_INC,
	SSD1306_SET_COM_PINS, 0x12,
	SSD1306_SET_CONTRAST, 0xFF,
	SSD1306_ENTIRE_DISP_OFF,
	SSD1306_DISP_NORMAL,
	SSD1306_MEM_MODE, 0x01,
	SSD1306_SET_CLOCK_DIV, 0x80,
	SSD1306_CHARGE_PUMP, 0x14,
	SSD1306_DISP_ON
    };

    for (uint8_t i = 0; i < sizeof(init_seq); i++)
	ssd1306_send_cmd(init_seq[i]);
}

void ssd1306_update_frame(int16_t *bins)
{
    ssd1306_send_cmd(SSD1306_COL_ADDR);
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(0x7F);

    ssd1306_send_cmd(SSD1306_PAGE_ADDR);
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(0x07);

    for (uint8_t x = 0; x < BINS; x++)
    {
	i2c_start(SSD1306_ADDRESS);
	i2c_send(0x40);

	int16_t h = bins[x];
	DRAW_VAR(h);
	h = 0;
	DRAW_VAR(h);

	i2c_stop();
    }

    i2c_start(SSD1306_ADDRESS);
    i2c_send(0x40);
    int16_t v = 0;
    DRAW_VAR(v);
    DRAW_VAR(v);
    i2c_stop();
}
