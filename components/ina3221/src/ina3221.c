#include "ina3221.h"
#include "bsp_i2c.h"
#include "esp_log.h"

static const char *TAG = "INA3221";

/* ── 寄存器定义（对照 reference/ina3221_arduino.ino）── */
#define INA3221_ADDR  0x40

#define REG_CONFIG    0x00
#define REG_CH1_SHUNT 0x01
#define REG_CH1_BUS   0x02
#define REG_MFID      0xFE

#define SHUNT_RESISTOR_OHM 0.1f
#define SHUNT_LSB_UV       40.0f
#define BUS_LSB_MV         8.0f

/* ── 模块内设备句柄 ─────────────────────────────────────── */
/* TODO: 参考 bsp_i2c.h 中 bsp_i2c_add_device() 的用法，声明 i2c_master_dev_handle_t 句柄 */

/* =========================================================
 * TODO A-1: 实现 ina3221_init()
 *
 * 参考 reference/ina3221_arduino.ino 中的 setup()：
 *   - readReg16(REG_MFID)       → i2c_master_transmit_receive()
 *   - writeReg16(REG_CONFIG, …) → i2c_master_transmit()
 *
 * ESP-IDF I2C 操作模板（参考 bsp_qmi8658.c）：
 *
 *   // 1. 挂载设备（INA3221 支持最高 2.56 MHz，400kHz 即可）
 *   i2c_master_dev_handle_t dev;
 *   ESP_ERROR_CHECK(bsp_i2c_add_device(INA3221_ADDR, 400000, &dev));
 *
 *   // 2. 读 16-bit 寄存器（big-endian）
 *   uint8_t reg = REG_MFID;
 *   uint8_t buf[2];
 *   i2c_master_transmit_receive(dev, &reg, 1, buf, 2, 100);
 *   uint16_t val = (buf[0] << 8) | buf[1];
 *
 *   // 3. 写 16-bit 寄存器
 *   uint16_t cfg = 0x7427;
 *   uint8_t cmd[3] = { REG_CONFIG, (uint8_t)(cfg >> 8), (uint8_t)(cfg & 0xFF) };
 *   i2c_master_transmit(dev, cmd, 3, 100);
 * ========================================================= */
esp_err_t ina3221_init(void)
{
    ESP_LOGW(TAG, "ina3221_init: NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}

/* =========================================================
 * TODO A-2: 实现 ina3221_read_channel()
 *
 * 参考 reference/ina3221_arduino.ino 中的 getBusVoltage_V() 和 getCurrent_mA()：
 *   寄存器地址规律：
 *     shunt_reg = REG_CH1_SHUNT + (channel - 1) * 2
 *     bus_reg   = REG_CH1_BUS   + (channel - 1) * 2
 *
 *   换算公式（与 Arduino 版完全一致）：
 *     raw >>= 3                          // bits[15:3] 有效
 *     bus_v = raw * (BUS_LSB_MV / 1000)
 *     shunt_v = raw * (SHUNT_LSB_UV * 1e-6f)  // int16_t，支持负值
 *     current_ma = (shunt_v / SHUNT_RESISTOR_OHM) * 1000
 * ========================================================= */
esp_err_t ina3221_read_channel(uint8_t channel, float *bus_v, float *current_ma)
{
    ESP_LOGW(TAG, "ina3221_read_channel: NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}
