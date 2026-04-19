#include "config_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "CFG";

#define NVS_NAMESPACE "app_cfg"
#define NVS_KEY_JSON  "config_json"

/* ─────────────────────────────────────────────────────────────────────────
 * Module A: NVS 初始化
 *
 * 约束：
 *   - 若 nvs_flash_init() 返回 ESP_ERR_NVS_NO_FREE_PAGES 或
 *     ESP_ERR_NVS_NEW_VERSION_FOUND，先调用 nvs_flash_erase() 再重新初始化
 * ───────────────────────────────────────────────────────────────────────── */
esp_err_t config_manager_init(void)
{
    // TODO: 实现 NVS flash 初始化
    //
    //   esp_err_t ret = nvs_flash_init();
    //   if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //       ESP_LOGW(TAG, "NVS partition truncated/changed, erasing...");
    //       nvs_flash_erase();
    //       ret = nvs_flash_init();
    //   }
    //   return ret;

    ESP_LOGE(TAG, "config_manager_init NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Module B: 从 NVS 读取配置
 *
 * 流程：
 *   1. nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle)
 *   2. nvs_get_str(handle, NVS_KEY_JSON, ...) 读取 JSON 字符串
 *      - 先用 NULL buffer 查询长度，再 malloc + 再读
 *   3. config_manager_from_json() 解析
 *   4. 若 ESP_ERR_NVS_NOT_FOUND：填充 CFG_DEFAULT_* 默认值
 *   5. nvs_close(handle)
 * ───────────────────────────────────────────────────────────────────────── */
esp_err_t config_manager_load(app_config_t *cfg)
{
    if (!cfg) return ESP_ERR_INVALID_ARG;

    // TODO: 实现 NVS 读取逻辑
    //
    // 提示：
    //   size_t len = 0;
    //   nvs_get_str(handle, NVS_KEY_JSON, NULL, &len);  /* 查询长度 */
    //   char *buf = malloc(len);
    //   nvs_get_str(handle, NVS_KEY_JSON, buf, &len);   /* 实际读取 */
    //   config_manager_from_json(buf, cfg);
    //   free(buf);

    ESP_LOGE(TAG, "config_manager_load NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Module C: 将配置写入 NVS
 *
 * 流程：
 *   1. config_manager_to_json(cfg) 生成 JSON 字符串
 *   2. nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle)
 *   3. nvs_set_str(handle, NVS_KEY_JSON, json_str)
 *   4. nvs_commit(handle)
 *   5. nvs_close(handle)，free(json_str)
 * ───────────────────────────────────────────────────────────────────────── */
esp_err_t config_manager_save(const app_config_t *cfg)
{
    if (!cfg) return ESP_ERR_INVALID_ARG;

    // TODO: 实现 NVS 写入逻辑

    ESP_LOGE(TAG, "config_manager_save NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}

/* 清除 NVS 中的配置键，下次 load 返回默认值 */
esp_err_t config_manager_reset(void)
{
    // TODO: nvs_open → nvs_erase_key → nvs_commit → nvs_close
    ESP_LOGE(TAG, "config_manager_reset NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Module D: JSON 字符串 → app_config_t
 *
 * 约束：
 *   - 字段缺失时保留 cfg 原值（部分更新语义）
 *   - 字符串字段用 strncpy，防止溢出
 *   - 数值字段：整数用 ->valueint，浮点用 ->valuedouble
 *
 * 示例输入：{"sensor_interval_ms":1000,"accel_alert_threshold":1.5}
 * ───────────────────────────────────────────────────────────────────────── */
esp_err_t config_manager_from_json(const char *json_str, app_config_t *cfg)
{
    if (!json_str || !cfg) return ESP_ERR_INVALID_ARG;

    // TODO: 实现 cJSON 解析
    //
    //   cJSON *root = cJSON_Parse(json_str);
    //   if (!root) { ESP_LOGE(TAG, "JSON parse error"); return ESP_FAIL; }
    //
    //   cJSON *item;
    //   if ((item = cJSON_GetObjectItem(root, "wifi_ssid")) && cJSON_IsString(item))
    //       strncpy(cfg->wifi_ssid, item->valuestring, sizeof(cfg->wifi_ssid) - 1);
    //
    //   if ((item = cJSON_GetObjectItem(root, "sensor_interval_ms")) && cJSON_IsNumber(item))
    //       cfg->sensor_interval_ms = (uint32_t)item->valueint;
    //
    //   if ((item = cJSON_GetObjectItem(root, "accel_alert_threshold")) && cJSON_IsNumber(item))
    //       cfg->accel_alert_threshold = (float)item->valuedouble;
    //
    //   // ... 其余字段类似 ...
    //   cJSON_Delete(root);
    //   return ESP_OK;

    ESP_LOGE(TAG, "config_manager_from_json NOT IMPLEMENTED");
    return ESP_ERR_NOT_SUPPORTED;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Module E: app_config_t → JSON 字符串（调用者须 free()）
 *
 * 约束：
 *   - 使用 cJSON_PrintUnformatted() 生成紧凑 JSON
 *   - cJSON 对象本身用 cJSON_Delete() 释放
 *   - 返回的字符串指针由调用者 free()
 * ───────────────────────────────────────────────────────────────────────── */
char *config_manager_to_json(const app_config_t *cfg)
{
    if (!cfg) return NULL;

    // TODO: 实现 cJSON 序列化
    //
    //   cJSON *root = cJSON_CreateObject();
    //   cJSON_AddStringToObject(root, "wifi_ssid",            cfg->wifi_ssid);
    //   cJSON_AddStringToObject(root, "wifi_password",        cfg->wifi_password);
    //   cJSON_AddStringToObject(root, "mqtt_uri",             cfg->mqtt_uri);
    //   cJSON_AddStringToObject(root, "device_id",            cfg->device_id);
    //   cJSON_AddNumberToObject(root, "sensor_interval_ms",   cfg->sensor_interval_ms);
    //   cJSON_AddNumberToObject(root, "accel_alert_threshold",cfg->accel_alert_threshold);
    //   char *out = cJSON_PrintUnformatted(root);
    //   cJSON_Delete(root);
    //   return out;   /* 调用者 free() */

    ESP_LOGE(TAG, "config_manager_to_json NOT IMPLEMENTED");
    return NULL;
}
