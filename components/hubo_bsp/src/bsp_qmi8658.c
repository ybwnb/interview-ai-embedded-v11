#include "bsp_qmi8658.h"
#include "bsp_i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "QMI8658";

#define QMI8658_ADDR     0x6B
#define QMI8658_WHO_AM_I 0x00
#define QMI8658_CTRL1    0x02
#define QMI8658_CTRL2    0x03
#define QMI8658_CTRL3    0x04
#define QMI8658_CTRL7    0x08
#define QMI8658_ACC_X_L  0x35
#define QMI8658_GYRO_X_L 0x3B
#define QMI8658_RESET    0x60

static i2c_master_dev_handle_t s_dev = NULL;

esp_err_t bsp_qmi8658_init(void)
{
    esp_err_t ret = bsp_i2c_add_device(QMI8658_ADDR, 400000, &s_dev);
    if (ret != ESP_OK) return ret;

    /* WHO_AM_I 验证 */
    uint8_t id = 0, reg = QMI8658_WHO_AM_I;
    bsp_i2c_lock();
    bsp_i2c_watchdog_arm(100);
    ret = i2c_master_transmit_receive(s_dev, &reg, 1, &id, 1, 1000);
    bsp_i2c_watchdog_disarm();
    bsp_i2c_unlock();
    if (ret != ESP_OK || id != 0x05) {
        ESP_LOGE(TAG, "WHO_AM_I=0x%02X (expected 0x05)", id);
        return ESP_FAIL;
    }

    /* 软件复位 */
    uint8_t reset[] = {QMI8658_RESET, 0xB0};
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    i2c_master_transmit(s_dev, reset, 2, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    vTaskDelay(pdMS_TO_TICKS(10));

    /* CTRL1: 地址自增 */
    uint8_t ctrl1[] = {QMI8658_CTRL1, 0x40};
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    ret = i2c_master_transmit(s_dev, ctrl1, 2, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    if (ret != ESP_OK) return ret;

    /* CTRL7: 使能加速度计 + 陀螺仪 */
    uint8_t ctrl7[] = {QMI8658_CTRL7, 0x03};
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    ret = i2c_master_transmit(s_dev, ctrl7, 2, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    if (ret != ESP_OK) return ret;

    /* CTRL2: 加速度计 4g / 250Hz */
    uint8_t ctrl2[] = {QMI8658_CTRL2, 0x95};
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    ret = i2c_master_transmit(s_dev, ctrl2, 2, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    if (ret != ESP_OK) return ret;

    /* CTRL3: 陀螺仪 512dps / 250Hz */
    uint8_t ctrl3[] = {QMI8658_CTRL3, 0xD5};
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    ret = i2c_master_transmit(s_dev, ctrl3, 2, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    if (ret != ESP_OK) return ret;

    ESP_LOGI(TAG, "QMI8658 ready: Accel=4g/250Hz  Gyro=512dps/250Hz");
    return ESP_OK;
}

esp_err_t bsp_qmi8658_read_accel(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6], reg = QMI8658_ACC_X_L;
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    esp_err_t ret = i2c_master_transmit_receive(s_dev, &reg, 1, data, 6, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    if (ret == ESP_OK) {
        *x = (int16_t)(data[1] << 8 | data[0]);
        *y = (int16_t)(data[3] << 8 | data[2]);
        *z = (int16_t)(data[5] << 8 | data[4]);
    }
    return ret;
}

esp_err_t bsp_qmi8658_read_gyro(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6], reg = QMI8658_GYRO_X_L;
    bsp_i2c_lock(); bsp_i2c_watchdog_arm(100);
    esp_err_t ret = i2c_master_transmit_receive(s_dev, &reg, 1, data, 6, 1000);
    bsp_i2c_watchdog_disarm(); bsp_i2c_unlock();
    if (ret == ESP_OK) {
        *x = (int16_t)(data[1] << 8 | data[0]);
        *y = (int16_t)(data[3] << 8 | data[2]);
        *z = (int16_t)(data[5] << 8 | data[4]);
    }
    return ret;
}

esp_err_t bsp_qmi8658_read_angle(qmi8658_angle_t *angle)
{
    esp_err_t ret = bsp_qmi8658_read_accel(&angle->acc_x, &angle->acc_y, &angle->acc_z);
    if (ret != ESP_OK) return ret;
    ret = bsp_qmi8658_read_gyro(&angle->gyr_x, &angle->gyr_y, &angle->gyr_z);
    if (ret != ESP_OK) return ret;

    float ax = angle->acc_x, ay = angle->acc_y, az = angle->acc_z;
    angle->angle_x = atanf(ax / sqrtf(ay * ay + az * az)) * 57.29578f;
    angle->angle_y = atanf(ay / sqrtf(ax * ax + az * az)) * 57.29578f;
    angle->angle_z = atanf(sqrtf(ax * ax + ay * ay) / az) * 57.29578f;
    return ESP_OK;
}
