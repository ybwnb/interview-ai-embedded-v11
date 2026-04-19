#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int16_t acc_x;    /*!< 加速度 X 原始值 */
    int16_t acc_y;    /*!< 加速度 Y 原始值 */
    int16_t acc_z;    /*!< 加速度 Z 原始值 */
    int16_t gyr_x;    /*!< 陀螺仪 X 原始值 */
    int16_t gyr_y;    /*!< 陀螺仪 Y 原始值 */
    int16_t gyr_z;    /*!< 陀螺仪 Z 原始值 */
    float angle_x;   /*!< Roll 角（度） */
    float angle_y;   /*!< Pitch 角（度） */
    float angle_z;   /*!< Yaw 角（度） */
} qmi8658_angle_t;

/**
 * @brief 初始化 QMI8658 IMU
 *
 * 配置：加速度计 4g / 250Hz，陀螺仪 512dps / 250Hz
 */
esp_err_t bsp_qmi8658_init(void);

/** 读取加速度计原始值（量程 4g，满量程 ±32768） */
esp_err_t bsp_qmi8658_read_accel(int16_t *x, int16_t *y, int16_t *z);

/** 读取陀螺仪原始值 */
esp_err_t bsp_qmi8658_read_gyro(int16_t *x, int16_t *y, int16_t *z);

/** 读取加速度 + 陀螺仪，并计算 Roll/Pitch/Yaw 角度 */
esp_err_t bsp_qmi8658_read_angle(qmi8658_angle_t *angle);

#ifdef __cplusplus
}
#endif
