/**
 * @file main.c
 * @brief Hubo-S3-Lite 面试骨架 — 候选人入口文件
 *
 * 当前状态：关键参数全部硬编码在本文件的 #define 区块中。
 *
 * 候选人需完成的工作（按模块顺序）：
 *
 *   Module A（INA3221 移植）
 *     - 阅读 reference/ina3221_arduino.ino，理解寄存器时序
 *     - 创建 components/ina3221/ 组件，自行编写 CMakeLists.txt
 *     - 实现 ina3221_init() 和 ina3221_read_channel()
 *     - 在 app_main 末尾验证：UART 打印通道 1 母线电压和电流
 *
 *   Module B（实现 config_manager）
 *     - 填写 components/config_manager/src/config_manager.c 中的全部 TODO
 *     - 单元验证：在 app_main 末尾调用 config_manager_to_json(&cfg) 打印结果
 *
 *   Module C（main.c 解耦）
 *     - 删除下方所有 #define 硬编码
 *     - 用 config_manager_load(&cfg) 替换手动赋值
 *     - 实现 sensor_task：读 QMI8658 → 比较阈值 → 驱动 WS2812
 *
 *   Module D（运行时配置更新）
 *     - 实现 uart_cmd_task：UART 接收 JSON → config_manager_save → 提示重启
 *     - 重启后 config_manager_load 验证配置已持久化
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_i2c.h"
#include "bsp_ws2812.h"
#include "bsp_tca9555.h"
#include "bsp_qmi8658.h"
#include "config_manager.h"
/* TODO Module A: 完成 components/ina3221/ 后取消注释 */
/* #include "ina3221.h" */

static const char *TAG = "MAIN";

/* =========================================================================
 * 硬编码参数区（Module C 完成后全部删除，改由 config_manager_load 提供）
 * ========================================================================= */
#define WIFI_SSID             "hubo_interview"
#define WIFI_PASSWORD         "hubo2026"
#define MQTT_BROKER_URI       "mqtt://192.168.1.100:1883"
#define DEVICE_ID             "HZ-S3-INTERVIEW"
#define SENSOR_INTERVAL_MS    2000
#define ACCEL_ALERT_THRESHOLD 2.0f
/* =========================================================================
 * 硬编码参数区结束
 * ========================================================================= */

/* -------------------------------------------------------------------------
 * TODO Module C: 实现 sensor_task
 *
 * 每隔 cfg->sensor_interval_ms 毫秒执行一次：
 *   1. bsp_qmi8658_read_accel(&x, &y, &z) 读取原始值
 *   2. 换算 g 值：accel_g = raw / 8192.0f（量程 4g，满量程 ±32768）
 *   3. 计算合加速度：mag = sqrtf(ax² + ay² + az²)
 *   4. mag > cfg->accel_alert_threshold → bsp_ws2812_set_all(ws2812_rgb(50, 0, 0))   // 红色报警
 *      mag <= threshold              → bsp_ws2812_set_all(ws2812_rgb(0, 50, 0))    // 绿色正常
 *   5. 通过 ESP_LOGI 打印原始值和合加速度
 *
 * 注意：cfg 通过 task arg 传入（指向 app_main 中的 app_config_t cfg）
 * 须在 main/CMakeLists.txt REQUIRES 中添加 ina3221（Module A 完成后）
 * ------------------------------------------------------------------------- */
static void sensor_task(void *arg)
{
    // TODO: implement
    (void)arg;
    ESP_LOGW(TAG, "sensor_task: NOT IMPLEMENTED");
    vTaskDelete(NULL);
}

/* -------------------------------------------------------------------------
 * TODO Module D: 实现 uart_cmd_task
 *
 * 循环读取 UART0 输入（每行一条 JSON 命令），示例：
 *   {"sensor_interval_ms":1000,"accel_alert_threshold":1.5}
 *
 * 处理流程：
 *   1. fgets(line, sizeof(line), stdin) 读取一行
 *   2. config_manager_load(&cfg) 读取当前配置
 *   3. config_manager_from_json(line, &cfg) 合并新值
 *   4. config_manager_save(&cfg) 写入 NVS
 *   5. 打印 "Config saved. Send 'r' + Enter to reboot."
 *   6. 若下一行输入为 "r"，调用 esp_restart()
 *
 * 提示：UART0 已由 IDF console 初始化，可直接用 fgets/scanf
 * ------------------------------------------------------------------------- */
static void uart_cmd_task(void *arg)
{
    // TODO: implement
    (void)arg;
    ESP_LOGW(TAG, "uart_cmd_task: NOT IMPLEMENTED");
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "==============================");
    ESP_LOGI(TAG, " Hubo-S3-Lite Interview Skeleton");
    ESP_LOGI(TAG, " Device: %s", DEVICE_ID);  /* TODO Module C: 改为 cfg.device_id */
    ESP_LOGI(TAG, "==============================");

    /* ── BSP 初始化 ── */
    ESP_ERROR_CHECK(bsp_i2c_init());
    bsp_i2c_scan();                  /* 期望日志看到 0x20、0x40、0x6B */
    ESP_ERROR_CHECK(bsp_ws2812_init());
    ESP_ERROR_CHECK(bsp_tca9555_init());
    ESP_ERROR_CHECK(bsp_qmi8658_init());

    /* ── Module A: INA3221 初始化（完成组件后取消注释）── */
    /* ESP_ERROR_CHECK(ina3221_init()); */

    /* ── Module A 验证（完成后取消注释，确认 UART 打印电压电流）── */
    /* float v, i;
       ina3221_read_channel(1, &v, &i);
       ESP_LOGI(TAG, "INA3221 CH1: %.3f V  %.2f mA", v, i); */

    /* ── Module B: config_manager 初始化 ── */
    ESP_ERROR_CHECK(config_manager_init());

    /* ── Module C: 从 NVS 读取配置（当前为手动填充，需替换）── */
    app_config_t cfg = {0};

    /* TODO Module C: 替换以下硬编码赋值为 config_manager_load(&cfg) */
    cfg.sensor_interval_ms    = SENSOR_INTERVAL_MS;
    cfg.accel_alert_threshold = ACCEL_ALERT_THRESHOLD;
    strncpy(cfg.device_id, DEVICE_ID, sizeof(cfg.device_id) - 1);
    /* TODO Module C end */

    ESP_LOGI(TAG, "Config: interval=%ldms  threshold=%.1fg  device=%s",
             cfg.sensor_interval_ms, cfg.accel_alert_threshold, cfg.device_id);

    /* ── 启动 FreeRTOS 任务 ── */
    xTaskCreate(sensor_task,   "sensor",   4096, &cfg, 5, NULL);
    xTaskCreate(uart_cmd_task, "uart_cmd", 4096, NULL, 3, NULL);

    /* WS2812 蓝色：系统就绪指示 */
    bsp_ws2812_set_all(ws2812_rgb(0, 0, 50));

    ESP_LOGI(TAG, "System ready. UART → JSON command to update config.");
}
