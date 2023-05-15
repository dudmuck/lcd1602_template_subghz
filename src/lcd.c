#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#include <lcd.h>

// commands
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME   0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL   0x08
#define LCD_CURSORSHIFT   0x10
#define LCD_FUNCTIONSET   0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

// flags for function set
#define LCD_8BITMODE   0x10
#define LCD_4BITMODE   0x00
#define LCD_2LINE   0x08
#define LCD_1LINE   0x00
#define LCD_5x10DOTS   0x04
#define LCD_5x8DOTS   0x00

// flags for display on/off control
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF   0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON   0x01
#define LCD_BLINKOFF   0x00

// flags for display entry mode
#define LCD_ENTRYRIGHT   0x00
#define LCD_ENTRYLEFT   0x02
#define LCD_ENTRYSHIFTINCREMENT   0x01
#define LCD_ENTRYSHIFTDECREMENT . 0x00

// flags for backlight control
#define LCD_BACKLIGHT       0x08
#define LCD_NOBACKLIGHT     0x00

LOG_MODULE_REGISTER(lcd, CONFIG_SIDEWALK_LOG_LEVEL);

const struct device *const lcd_dev = DEVICE_DT_GET_ONE(nxp_pcf8574);

#define LCD_BIT_RS      0x01    /* register select bit */
#define LCD_BIT_RW      0x02    /* read / write bit */
#define LCD_BIT_En      0x04    /* enable bit */

void lcd_strobe(uint8_t data)
{
    gpio_port_set_masked(lcd_dev, 0xff, data | LCD_BIT_En | LCD_BACKLIGHT);
    k_sleep(K_MSEC(5));
    gpio_port_set_masked(lcd_dev, 0xff, (data & ~LCD_BIT_En) | LCD_BACKLIGHT);
    k_sleep(K_MSEC(1));
}

void lcd_write_four_bits(uint8_t data)
{
    gpio_port_set_masked(lcd_dev, 0xff, data | LCD_BACKLIGHT);
    lcd_strobe(data);
}

void lcd_write_Rs(uint8_t cmd)
{
    uint8_t mode = LCD_BIT_RS;
    lcd_write_four_bits(mode | (cmd & 0xf0));
    lcd_write_four_bits(mode | ((cmd << 4) & 0xf0));
}

void lcd_write(uint8_t cmd)
{
    uint8_t mode = 0;
    lcd_write_four_bits(mode | (cmd & 0xf0));
    lcd_write_four_bits(mode | ((cmd << 4) & 0xf0));
}

void lcd_display_string(const char* str, uint8_t line, uint8_t pos)
{
    uint8_t pos_new = pos;
    switch (line) {
        case 2: pos_new = 0x40 + pos; break;
        case 3: pos_new = 0x14 + pos; break;
        case 4: pos_new = 0x54 + pos; break;
    }

    lcd_write(0x80 + pos_new);

    while (*str != 0) {
        lcd_write_Rs(*str);
        str++;
    }
}

int lcd_init()
{
    if (!device_is_ready(lcd_dev)) {
        LOG_ERR("LCD: device not ready.");
        return -1;
    }

    LOG_INF("LCD device is %p, name is %s", lcd_dev, lcd_dev->name);
    int ret;
    for (int i = 0; i < 8; i++) {
        ret = gpio_pin_configure(lcd_dev, i, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
        if (ret) {
            LOG_ERR("Error in configure pin %d on lcd_dev: %d", i, ret);
            return ret;
        }
    }

    lcd_write(0x03);
    lcd_write(0x03);
    lcd_write(0x02);

    lcd_write(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
    lcd_write(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
    lcd_write(LCD_CLEARDISPLAY);
    lcd_write(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
    
    return 0;
}

