#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TCA9555 按键状态
 *
 * 引脚映射（P0 端口，低电平有效）：
 *   P0.0=F1(joy_up)    P0.1=F2(joy_left)  P0.2=F3(joy_down)
 *   P0.3=F4(joy_right) P0.4=F5(joy_center)
 *   P0.5=KEY2(btn_prev) P0.6=KEY1(btn_next)
 */
typedef struct {
    bool joy_up;
    bool joy_down;
    bool joy_left;
    bool joy_right;
    bool joy_center;
    bool btn_prev;
    bool btn_next;
    uint8_t raw_p0;
} tca9555_button_state_t;

/** 初始化 TCA9555（I2C 地址 0x20，P0/P1 全部配置为输入） */
esp_err_t bsp_tca9555_init(void);

/** 读取当前按键状态 */
esp_err_t bsp_tca9555_read_buttons(tca9555_button_state_t *state);

/** 返回 TCA9555 是否已初始化 */
bool bsp_tca9555_is_initialized(void);

#ifdef __cplusplus
}
#endif
