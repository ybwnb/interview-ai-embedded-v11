#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 应用配置结构体
 *
 * 所有字段均可通过 JSON 序列化后持久化到 NVS。
 * 候选人任务：在 config_manager.c 中实现各 API，使 main.c 能从 NVS 读取此结构体。
 */
typedef struct {
    char     wifi_ssid[64];
    char     wifi_password[64];
    char     mqtt_uri[128];
    char     device_id[32];
    uint32_t sensor_interval_ms;
    float    accel_alert_threshold;
} app_config_t;

/* 默认值（首次启动 / NVS 无数据时使用） */
#define CFG_DEFAULT_WIFI_SSID          "hubo_interview"
#define CFG_DEFAULT_WIFI_PASSWORD      "hubo2026"
#define CFG_DEFAULT_MQTT_URI           "mqtt://192.168.1.100:1883"
#define CFG_DEFAULT_DEVICE_ID          "HZ-S3-INTERVIEW"
#define CFG_DEFAULT_SENSOR_INTERVAL_MS 2000
#define CFG_DEFAULT_ACCEL_THRESHOLD    2.0f

/**
 * @brief 初始化 NVS flash（app_main 最先调用）
 *
 * 若 NVS 分区无可用页或版本不匹配，自动擦除后重新初始化。
 */
esp_err_t config_manager_init(void);

/**
 * @brief 从 NVS 读取配置到 cfg
 *
 * 若 NVS 中无记录，cfg 填充默认值并返回 ESP_OK。
 */
esp_err_t config_manager_load(app_config_t *cfg);

/**
 * @brief 将 cfg 序列化为 JSON 并写入 NVS
 */
esp_err_t config_manager_save(const app_config_t *cfg);

/**
 * @brief 删除 NVS 中的配置（下次 load 将返回默认值）
 */
esp_err_t config_manager_reset(void);

/**
 * @brief 将 JSON 字符串解析并合并到 cfg（缺失字段保留原值）
 *
 * @param json_str 合法的 JSON 字符串
 * @param cfg      [in/out] 目标结构体
 */
esp_err_t config_manager_from_json(const char *json_str, app_config_t *cfg);

/**
 * @brief 将 cfg 序列化为 JSON 字符串（调用者须 free()）
 *
 * @return 动态分配的 JSON 字符串，失败返回 NULL
 */
char *config_manager_to_json(const app_config_t *cfg);

#ifdef __cplusplus
}
#endif
