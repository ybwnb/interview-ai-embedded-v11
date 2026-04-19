#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/** I2C 引脚（Hubo-S3-Lite V5.1 硬件固定） */
#define BSP_I2C_SDA_PIN  48
#define BSP_I2C_SCL_PIN  47

/**
 * @brief 初始化 I2C 主机总线（SDA=GPIO48, SCL=GPIO47, 400kHz）
 */
esp_err_t bsp_i2c_init(void);

/**
 * @brief 扫描 I2C 总线，打印所有在线设备地址
 */
esp_err_t bsp_i2c_scan(void);

/**
 * @brief 将设备挂载到 I2C 总线
 *
 * @param addr      7位 I2C 地址
 * @param speed_hz  设备通信速率（Hz），典型值 400000
 * @param dev_handle 输出：设备句柄
 */
esp_err_t bsp_i2c_add_device(uint8_t addr, uint32_t speed_hz, i2c_master_dev_handle_t *dev_handle);

/** 获取 I2C 总线递归互斥锁（多任务环境下保护总线） */
void bsp_i2c_lock(void);

/** 释放 I2C 总线锁 */
void bsp_i2c_unlock(void);

/**
 * @brief 总线恢复：发送 9 个 SCL 脉冲 + 重初始化，解除 SDA 被拉低的死锁
 *
 * 须在持有 bsp_i2c_lock() 时调用。
 */
void bsp_i2c_recover(void);

/**
 * @brief 启动 I2C 传输看门狗（在 i2c_master_transmit* 之前调用）
 *
 * 超时后强制复位 I2C 外设，针对 ESP-IDF v5.4.x NACK 死循环 bug。
 *
 * @param timeout_ms 超时毫秒（建议 100ms）
 */
void bsp_i2c_watchdog_arm(uint32_t timeout_ms);

/** 停止看门狗（i2c_master_transmit* 返回后立即调用） */
void bsp_i2c_watchdog_disarm(void);

#ifdef __cplusplus
}
#endif
