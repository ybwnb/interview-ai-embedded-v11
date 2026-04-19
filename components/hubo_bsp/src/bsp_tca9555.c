#include "bsp_tca9555.h"
#include "bsp_i2c.h"
#include "esp_log.h"

static const char *TAG = "TCA9555";

#define TCA9555_ADDR   0x20
#define REG_INPUT_P0   0x00
#define REG_INPUT_P1   0x01
#define REG_CONFIG_P0  0x06
#define REG_CONFIG_P1  0x07

#define PIN_JOY_UP     (1 << 0)
#define PIN_JOY_LEFT   (1 << 1)
#define PIN_JOY_DOWN   (1 << 2)
#define PIN_JOY_RIGHT  (1 << 3)
#define PIN_JOY_CENTER (1 << 4)
#define PIN_BTN_PREV   (1 << 5)
#define PIN_BTN_NEXT   (1 << 6)

static i2c_master_dev_handle_t s_dev = NULL;
static bool s_initialized = false;

static esp_err_t write_reg(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    bsp_i2c_lock();
    bsp_i2c_watchdog_arm(100);
    esp_err_t ret = i2c_master_transmit(s_dev, buf, 2, 100);
    bsp_i2c_watchdog_disarm();
    bsp_i2c_unlock();
    return ret;
}

static esp_err_t read_reg(uint8_t reg, uint8_t *data)
{
    bsp_i2c_lock();
    bsp_i2c_watchdog_arm(100);
    esp_err_t ret = i2c_master_transmit_receive(s_dev, &reg, 1, data, 1, 100);
    bsp_i2c_watchdog_disarm();
    bsp_i2c_unlock();
    return ret;
}

esp_err_t bsp_tca9555_init(void)
{
    esp_err_t ret = bsp_i2c_add_device(TCA9555_ADDR, 400000, &s_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Add device failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = write_reg(REG_CONFIG_P0, 0xFF);  /* P0 全部输入 */
    if (ret != ESP_OK) return ret;
    ret = write_reg(REG_CONFIG_P1, 0xFF);  /* P1 全部输入 */
    if (ret != ESP_OK) return ret;

    s_initialized = true;
    ESP_LOGI(TAG, "TCA9555 @ 0x%02X ready", TCA9555_ADDR);
    ESP_LOGI(TAG, "  joy: up/left/down/right/center = P0.0-4");
    ESP_LOGI(TAG, "  btn: prev=P0.5  next=P0.6");
    return ESP_OK;
}

esp_err_t bsp_tca9555_read_buttons(tca9555_button_state_t *state)
{
    if (!s_initialized || !state) return ESP_ERR_INVALID_STATE;
    uint8_t p0 = 0xFF;
    esp_err_t ret = read_reg(REG_INPUT_P0, &p0);
    if (ret != ESP_OK) return ret;

    state->raw_p0     = p0;
    state->joy_up     = !(p0 & PIN_JOY_UP);
    state->joy_left   = !(p0 & PIN_JOY_LEFT);
    state->joy_down   = !(p0 & PIN_JOY_DOWN);
    state->joy_right  = !(p0 & PIN_JOY_RIGHT);
    state->joy_center = !(p0 & PIN_JOY_CENTER);
    state->btn_prev   = !(p0 & PIN_BTN_PREV);
    state->btn_next   = !(p0 & PIN_BTN_NEXT);
    return ESP_OK;
}

bool bsp_tca9555_is_initialized(void) { return s_initialized; }
