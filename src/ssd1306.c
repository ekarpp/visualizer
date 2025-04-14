#include <zephyr/drivers/i2c.h>
#include "ssd1306.h"

#define DRAW_VAR(vv)                            \
    for (uint8_t i = 0; i < 8; i++)             \
    {                                           \
        if (vv >= 8)                            \
        {                                       \
            buffer[j] = 0xFF;                   \
            vv -= 8;                            \
        }                                       \
        else if (vv > 0)                        \
        {                                       \
            buffer[j] = (1 << vv) - 1;          \
            vv = 0;                             \
        }                                       \
        else                                    \
            buffer[j] = 0x0;                    \
        j++;                                    \
    }
#define I2C_DEV_NODE DT_NODELABEL(i2c1)
static const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));

int ssd1306_send(const uint8_t *bytes, size_t len, bool is_cmd)
{
    uint8_t start_addr = (is_cmd) ? 0x00 : 0x40;
    return i2c_burst_write(i2c_dev, SSD1306_ADDRESS, start_addr, bytes, len);
}

int ssd1306_init(void)
{
    if (!device_is_ready(i2c_dev))
    {
        printf("i2c not ready, exit\n");
        return -1;
    }

    uint8_t byte = SSD1306_DISP_OFF;
    int ret = ssd1306_send(&byte, 1, true);
    if (ret < 0)
        goto error;
    printf("i2c ok\n");
    const uint8_t init_seq[] = {
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
        SSD1306_DISP_ON,
    };

    if (ssd1306_send(init_seq, sizeof(init_seq), true) < 0)
        goto error;

    return 0;


error:
    printf("i2c IO error\n");
    return -1;
}

void ssd1306_update_frame(int8_t *bins)
{
    const uint8_t clear_seq[] = {
        SSD1306_COL_ADDR,
        0x00,
        0x7F,
        SSD1306_PAGE_ADDR,
        0x00,
        0x07,
    };
    ssd1306_send(clear_seq, sizeof(clear_seq), true);

    uint8_t buffer[8 + 8];
    for (uint8_t x = 0; x < BINS; x++)
    {
        uint8_t j = 0;
        int16_t h = bins[x];
        DRAW_VAR(h);
        h = 0;
        DRAW_VAR(h);
        ssd1306_send(buffer, 16, false);
    }

    uint8_t j = 0;
    int16_t v = 0;
    DRAW_VAR(v);
    DRAW_VAR(v);
    ssd1306_send(buffer, 16, false);
}
