#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/display/cfb.h>
#include "ssd1306.h"

static const struct device *cfb_dev = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

int ssd1306_init(void)
{
    if (!device_is_ready(cfb_dev))
    {
        printf("display not ready, exit\n");
        return -1;
    }

	if (display_set_pixel_format(cfb_dev, PIXEL_FORMAT_MONO10) != 0) {
		if (display_set_pixel_format(cfb_dev, PIXEL_FORMAT_MONO01) != 0) {
			printf("Failed to set required pixel format");
			return -1;
		}
	}

	if (cfb_framebuffer_init(cfb_dev)) {
		printf("Framebuffer initialization failed!\n");
		return -1;
	}

    cfb_framebuffer_clear(cfb_dev, true);

    display_blanking_off(cfb_dev);
    cfb_framebuffer_invert(cfb_dev);

    return 0;
}

void ssd1306_update_frame(int8_t *bins)
{
    cfb_framebuffer_clear(cfb_dev, false);
    for (uint16_t i = 0; i < BINS; i++)
    {
        int16_t x = 2*i; int16_t y = bins[i];
        struct cfb_position start = { x, 64 };
        struct cfb_position end = { x, MAX(64 - y, 0) };

        cfb_draw_line(cfb_dev, &start, &end);
    }

    cfb_framebuffer_finalize(cfb_dev);
    return;
}
