#include "bsp_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "BSP_I2C";

#define I2C_FREQ_HZ  400000

static i2c_master_bus_handle_t s_bus = NULL;
static SemaphoreHandle_t       s_lock = NULL;
static esp_timer_handle_t      s_wdog = NULL;

/* 看门狗回调：超时后强制复位 I2C 外设，针对 ESP-IDF v5.4.x NACK 死循环 bug */
static void wdog_cb(void *arg)
{
    if (s_bus) {
        ESP_LOGW(TAG, "I2C watchdog: transmit stuck, forcing bus reset");
        i2c_master_bus_reset(s_bus);
    }
}

esp_err_t bsp_i2c_init(void)
{
    i2c_master_bus_config_t cfg = {
        .i2c_port              = I2C_NUM_0,
        .sda_io_num            = BSP_I2C_SDA_PIN,
        .scl_io_num            = BSP_I2C_SCL_PIN,
        .clk_source            = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt     = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t ret = i2c_new_master_bus(&cfg, &s_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C bus: %s", esp_err_to_name(ret));
        return ret;
    }

    s_lock = xSemaphoreCreateRecursiveMutex();
    if (!s_lock) {
        i2c_del_master_bus(s_bus);
        return ESP_ERR_NO_MEM;
    }

    esp_timer_create_args_t wdog_cfg = {.callback = wdog_cb, .name = "i2c_wdog"};
    esp_timer_create(&wdog_cfg, &s_wdog);

    ESP_LOGI(TAG, "I2C bus ready: SDA=GPIO%d SCL=GPIO%d @%dHz",
             BSP_I2C_SDA_PIN, BSP_I2C_SCL_PIN, I2C_FREQ_HZ);
    return ESP_OK;
}

esp_err_t bsp_i2c_scan(void)
{
    ESP_LOGI(TAG, "Scanning I2C bus...");
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        if (i2c_master_probe(s_bus, addr, 20) == ESP_OK) {
            ESP_LOGI(TAG, "  Found 0x%02X", addr);
            found++;
        }
    }
    ESP_LOGI(TAG, "Scan done: %d device(s)", found);
    return found ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_i2c_add_device(uint8_t addr, uint32_t speed_hz, i2c_master_dev_handle_t *dev_handle)
{
    if (!s_bus) return ESP_ERR_INVALID_STATE;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = addr,
        .scl_speed_hz    = speed_hz,
    };
    esp_err_t ret = i2c_master_bus_add_device(s_bus, &dev_cfg, dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device 0x%02X: %s", addr, esp_err_to_name(ret));
    }
    return ret;
}

void bsp_i2c_lock(void)   { if (s_lock) xSemaphoreTakeRecursive(s_lock, portMAX_DELAY); }
void bsp_i2c_unlock(void) { if (s_lock) xSemaphoreGiveRecursive(s_lock); }

void bsp_i2c_recover(void)
{
    if (s_bus) i2c_master_bus_reset(s_bus);
}

void bsp_i2c_watchdog_arm(uint32_t timeout_ms)
{
    if (!s_wdog) return;
    esp_timer_stop(s_wdog);
    esp_timer_start_once(s_wdog, (uint64_t)timeout_ms * 1000);
}

void bsp_i2c_watchdog_disarm(void)
{
    if (s_wdog) esp_timer_stop(s_wdog);
}
