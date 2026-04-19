#include "bsp_ws2812.h"
#include "led_strip.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "WS2812";

#define WS2812_GPIO 18

static led_strip_handle_t s_strip = NULL;
static ws2812_rgb_t       s_buf[BSP_WS2812_LED_COUNT] = {0};

esp_err_t bsp_ws2812_init(void)
{
    led_strip_config_t strip_cfg = {
        .strip_gpio_num  = WS2812_GPIO,
        .max_leds        = BSP_WS2812_LED_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model       = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };
    led_strip_rmt_config_t rmt_cfg = {
        .clk_src        = RMT_CLK_SRC_DEFAULT,
        .resolution_hz  = 10 * 1000 * 1000,
        .flags.with_dma = false,
    };
    esp_err_t ret = led_strip_new_rmt_device(&strip_cfg, &rmt_cfg, &s_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed: %s", esp_err_to_name(ret));
        return ret;
    }
    bsp_ws2812_clear();
    ESP_LOGI(TAG, "WS2812 ready (%d LEDs, GPIO%d)", BSP_WS2812_LED_COUNT, WS2812_GPIO);
    return ESP_OK;
}

esp_err_t bsp_ws2812_set_all(ws2812_rgb_t color)
{
    for (int i = 0; i < BSP_WS2812_LED_COUNT; i++) s_buf[i] = color;
    return bsp_ws2812_refresh();
}

esp_err_t bsp_ws2812_set_pixel(uint8_t index, ws2812_rgb_t color)
{
    if (index >= BSP_WS2812_LED_COUNT) return ESP_ERR_INVALID_ARG;
    s_buf[index] = color;
    return ESP_OK;
}

esp_err_t bsp_ws2812_set_pixels(const ws2812_rgb_t *colors)
{
    if (!colors) return ESP_ERR_INVALID_ARG;
    memcpy(s_buf, colors, sizeof(s_buf));
    return bsp_ws2812_refresh();
}

esp_err_t bsp_ws2812_clear(void)
{
    return bsp_ws2812_set_all(ws2812_rgb(0, 0, 0));
}

esp_err_t bsp_ws2812_refresh(void)
{
    if (!s_strip) return ESP_ERR_INVALID_STATE;
    for (int i = 0; i < BSP_WS2812_LED_COUNT; i++) {
        esp_err_t ret = led_strip_set_pixel(s_strip, i, s_buf[i].r, s_buf[i].g, s_buf[i].b);
        if (ret != ESP_OK) return ret;
    }
    return led_strip_refresh(s_strip);
}

/* Gamma 2.2 校正表 */
static const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

ws2812_rgb_t ws2812_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v)
{
    h = h % 360;
    uint8_t hue = (h * 255) / 360;
    uint8_t sat = (s * 255) / 100;
    uint8_t val = (v * 255) / 100;
    uint8_t region    = hue / 43;
    uint8_t remainder = (hue - region * 43) * 6;
    uint8_t p = (val * (255 - sat)) >> 8;
    uint8_t q = (val * (255 - ((sat * remainder) >> 8))) >> 8;
    uint8_t t = (val * (255 - ((sat * (255 - remainder)) >> 8))) >> 8;
    uint8_t r, g, b;
    switch (region) {
        case 0: r = val; g = t;   b = p;   break;
        case 1: r = q;   g = val; b = p;   break;
        case 2: r = p;   g = val; b = t;   break;
        case 3: r = p;   g = q;   b = val; break;
        case 4: r = t;   g = p;   b = val; break;
        default: r = val; g = p;  b = q;   break;
    }
    return ws2812_rgb(r, g, b);
}

ws2812_rgb_t ws2812_scale_brightness(ws2812_rgb_t color, uint8_t brightness)
{
    uint8_t sr = (color.r * brightness) / 100;
    uint8_t sg = (color.g * brightness) / 100;
    uint8_t sb = (color.b * brightness) / 100;
    ws2812_rgb_t out = {gamma8[sr], gamma8[sg], gamma8[sb]};
    if (sr && !out.r) out.r = 1;
    if (sg && !out.g) out.g = 1;
    if (sb && !out.b) out.b = 1;
    return out;
}

ws2812_rgb_t ws2812_scale_brightness_linear(ws2812_rgb_t color, uint8_t brightness)
{
    return ws2812_rgb(
        (color.r * brightness) / 100,
        (color.g * brightness) / 100,
        (color.b * brightness) / 100
    );
}
