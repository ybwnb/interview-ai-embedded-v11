#pragma once

#include "esp_err.h"
#include <stdint.h>

/**
 * 初始化 INA3221
 *
 * 执行步骤：
 *   1. 通过 bsp_i2c_add_device() 注册设备句柄
 *   2. 读取 REG_MFID（0xFE），验证值为 0x5449，否则返回 ESP_FAIL
 *   3. 写入 REG_CONFIG = 0x7427（使能全部 3 通道，连续采样）
 *
 * @return ESP_OK / ESP_FAIL（设备未找到）
 */
esp_err_t ina3221_init(void);

/**
 * 读取指定通道的母线电压和电流
 *
 * @param channel     通道编号，取值 1~3
 * @param bus_v       母线电压输出（单位 V），传 NULL 则跳过
 * @param current_ma  电流输出（单位 mA），传 NULL 则跳过
 * @return ESP_OK / ESP_ERR_INVALID_ARG（channel 超范围）/ ESP_FAIL（读取失败）
 *
 * 注：CMakeLists.txt 须自行从零编写，REQUIRES 须声明 hubo_bsp
 */
esp_err_t ina3221_read_channel(uint8_t channel, float *bus_v, float *current_ma);
