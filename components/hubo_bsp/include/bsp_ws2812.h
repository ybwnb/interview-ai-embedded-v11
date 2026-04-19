#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_WS2812_LED_COUNT 10  /*!< 板载 WS2812B 灯珠数量 */

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ws2812_rgb_t;

/** 初始化 WS2812 驱动（RMT 后端，GPIO18） */
esp_err_t bsp_ws2812_init(void);

/** 所有灯珠设置为同一颜色并刷新 */
esp_err_t bsp_ws2812_set_all(ws2812_rgb_t color);

/** 设置单个灯珠颜色（需手动调用 bsp_ws2812_refresh() 生效） */
esp_err_t bsp_ws2812_set_pixel(uint8_t index, ws2812_rgb_t color);

/** 用数组设置所有灯珠并刷新（colors 长度须为 BSP_WS2812_LED_COUNT） */
esp_err_t bsp_ws2812_set_pixels(const ws2812_rgb_t *colors);

/** 关闭所有灯珠 */
esp_err_t bsp_ws2812_clear(void);

/** 将缓冲区数据发送到灯珠（set_pixel 后必须调用） */
esp_err_t bsp_ws2812_refresh(void);

/** 构造 RGB 颜色（内联辅助函数） */
static inline ws2812_rgb_t ws2812_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    ws2812_rgb_t c = {r, g, b};
    return c;
}

/**
 * @brief HSV → RGB 转换（FastLED 整数算法）
 * @param h 色调 0-359
 * @param s 饱和度 0-100
 * @param v 亮度 0-100
 */
ws2812_rgb_t ws2812_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/** 线性空间缩放亮度后应用 Gamma 2.2 校正（适合静态颜色） */
ws2812_rgb_t ws2812_scale_brightness(ws2812_rgb_t color, uint8_t brightness);

/** 纯线性亮度缩放，无 Gamma（适合动画渐变） */
ws2812_rgb_t ws2812_scale_brightness_linear(ws2812_rgb_t color, uint8_t brightness);

#ifdef __cplusplus
}
#endif
