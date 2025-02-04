#include <stdint.h>
#include "stm32f1xx.h"

#include "ssd1306.h"
#include "main.h"


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

void ssd1306_update_frame(uint16_t *data)
{
    ssd1306_send_cmd(SSD1306_COL_ADDR);
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(0x7F);

    ssd1306_send_cmd(SSD1306_PAGE_ADDR);
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(0x07);

    uint16_t id = 0;
    int16_t v = 0;
    uint8_t i,j;

    for (i = 0; i < 62; i++)
    {
	i2c_start(SSD1306_ADDRESS);
	i2c_send(0x40);

	for (j = 0; j < 16; j++)
	{
	    if (j%8 == 0)
		v = data[id];

	    if (i%2 == 1 && j > 7)
		/* empty space between each bin */
		i2c_send(0x00);
	    else
	    {
		if (v >= 8)
		{
		    i2c_send(0xFF);
		    v -= 8;
		}
		else
		{
		    i2c_send((1 << v) - 1);
		    v = 0;
		    while (j%8 != 7)
		    {
			i2c_send(0x00);
			j++;
		    }
		}
	    }
	}
	/* next bin every 4 width */
	if (i%2 == 1)
	    id += 1;
	i2c_stop();
    }

    for (i = 0; i < 2; i++)
    {
	i2c_start(SSD1306_ADDRESS);
	i2c_send(0x40);
	for (j = 0; j < 16; j++)
	    i2c_send(0x00);
	i2c_stop();
    }

}
