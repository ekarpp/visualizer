#ifndef MAIN_H
#define MAIN_H

#define SAMPLES (1 << SAMPLES_LG)

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_ADDRESS 0x78

#define BINS ((SSD1306_WIDTH / 2) - 1)

//assert(SAMPLES >= 2*BINS);

#define UNUSED(x) ((void) x)
#define ABS(x)    (((x) < 0) ? (-(x)) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))

typedef struct {
    int32_t Re;
    int32_t Im;
} complex_t;

/* main.c */
void init(void);
void clock_init(void);
void sleep(uint32_t ms);
int main(void);

/* adc.c */
void adc_init(void);
void adc_sample(complex_t *data);

/* fft.c */
void bit_reversal(complex_t *data);
void fft(complex_t *data);
void scale(complex_t *data, int16_t *frame_buffer);
int32_t scaled_mul(int32_t a, int32_t b);

/* i2c.c */
void i2c_init(void);
void i2c_start(uint8_t i2c_address);
void i2c_send(uint8_t byte);
void i2c_stop(void);

/* led.c */
void led_init(void);
void led_on(void);
void led_off(void);

/* ssd1306.c */
void ssd1306_send_cmd(uint8_t byte);
void ssd1306_init(void);
void ssd1306_update_frame(int16_t *frame_buffer);

/* usart.c */
void usart_init(void);
uint8_t usart_send(uint8_t byte);
uint8_t usart_read(void);

#endif
