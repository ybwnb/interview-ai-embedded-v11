# AI 嵌软工程师面试实测任务书 V2

**考核时长：** 120 分钟 | **版本：** V2.0 | **骨架：** interview-ai-embedded-v1

## 硬件

面试官提供 **Hubo-S3-Lite V5.1** 开发板（ESP32-S3R16N8：16MB Octal PSRAM + 8MB Flash）。  
USB-C 连接电脑，确认设备管理器中出现 COM 口。

## 考核主题

在生产级 ESP-IDF 骨架上，完成以下端到端链路：

**INA3221 驱动移植 → 实现 config_manager（cJSON + NVS）→ main.c 解耦 → 运行时配置更新**

## 模块一览（共 100 分）

| 模块 | 分支 | 分值 | 核心要求 |
|------|------|------|---------|
| A INA3221 移植 | `feat/ina3221` | 20 | 阅读 `reference/ina3221_arduino.ino`，移植为 ESP-IDF 组件；**自行从零编写** `CMakeLists.txt`；UART 打印通道 1 电压和电流验证 |
| B config_manager 核心 | `feat/config-manager` | 30 | 实现全部 5 个函数：init / load / save / from_json / to_json |
| C main.c 解耦 | `feat/refactor-main` | 20 | 删除所有 #define 硬编码；调用 config_manager_load；sensor_task 通过 WS2812 驱动报警 |
| D 运行时配置更新 | `feat/uart-cmd` | 20 | 实现 uart_cmd_task；UART 收到 JSON → NVS 持久化；重启验证配置生效 |
| 工程过程 | — | 10 | `.claude/` 目录包含本次会话记录并 commit；SELF-REVIEW.md 四题必答 |

## 快速开始

```bash
# 1. Fork 并 clone 到本地
git clone git@github.com:<你的账号>/interview-ai-embedded-v1.git
cd interview-ai-embedded-v1

# 2. 设置目标芯片（仅首次）
idf.py set-target esp32s3

# 3. 编译 + 烧录 + 监控（替换 COMx 为实际端口）
idf.py -p COMx flash monitor
```

> `main/CMakeLists.txt` 的 REQUIRES 已声明 `ina3221`，但 `components/ina3221/` 无 `CMakeLists.txt`——首次 `idf.py build` 会报 **Could not find component 'ina3221'**，这是进入 Module A 的入口信号。  
> Module A 的 `CMakeLists.txt` 写好后编译通过，此时 `config_manager_init` 返回 `ESP_ERR_NOT_SUPPORTED`；实现 Module B 后 `ESP_ERROR_CHECK(config_manager_init())` 才能正常启动。

## Module A 详细说明

**参考文件：** `reference/ina3221_arduino.ino`

**任务：**
1. 阅读参考代码，理解 INA3221 的 I2C 通信时序、寄存器结构和换算公式
2. 在 `components/ina3221/` 目录下**自行从零编写 `CMakeLists.txt`**
3. 实现 `components/ina3221/src/ina3221.c` 中的两个 TODO 函数

**验收方式：** 取消 `main.c` 中的注释，上电后 UART 日志打印类似：
```
INA3221 CH1: 3.296 V  12.40 mA
```

**约束：**
- `CMakeLists.txt` 中 `REQUIRES` 须声明 `hubo_bsp`（使用其 I2C 接口）
- 厂商 ID 验证失败（非 0x5449）时须返回 `ESP_FAIL` 并打印错误日志，不得继续执行
- 禁止直接复制参考代码——须将 `Wire.h` 操作替换为 `bsp_i2c.h` 提供的 ESP-IDF I2C API

## 关键技术约束

- cJSON 须满足以下三点：
  1. **声明依赖**：`config_manager/CMakeLists.txt` 的 REQUIRES 必须包含 `json`（IDF 内置），禁止手动 include 路径或引入第三方库
  2. **错误处理**：`cJSON_Parse()` 返回 NULL 时须打印错误日志并返回 `ESP_FAIL`；cJSON 对象必须用 `cJSON_Delete()` 释放，不得内存泄漏
  3. **验证方式**：Module B 完成后在 `app_main` 末尾调用 `config_manager_to_json(&cfg)` 打印 JSON 到 UART，肉眼对照六个字段值确认正确后再进入 Module C
- NVS namespace 固定为 `app_cfg`，key 固定为 `config_json`
- `config_manager_from_json()` 必须是**部分更新**语义：JSON 中没有的字段保留 cfg 原值
- `config_manager_to_json()` 使用 `cJSON_PrintUnformatted()`，返回字符串由**调用者** `free()`
- 禁止修改 `components/hubo_bsp/` 中的任何文件
- 至少 4 个 feature 分支，语义化 commit

## 演示验收

```
idf.py -p COMx monitor
```

1. **上电**：I2C scan 日志显示 0x20（TCA9555）、0x40（INA3221）、0x6B（QMI8658）
2. **Module A 验证**：UART 打印 INA3221 通道 1 电压（≈实际轨电压）和电流
3. **晃动硬件**：合加速度超过 `accel_alert_threshold` → WS2812 红色；静止 → 绿色
4. **UART 输入**（idf.py monitor 窗口直接输入）：
   ```json
   {"sensor_interval_ms":500,"accel_alert_threshold":1.5}
   ```
   回显 `Config saved. Send 'r' + Enter to reboot.`
5. **输入 r 重启**：重启后日志显示新的 interval=500ms threshold=1.5g

## SELF-REVIEW（10 分，必交）

```bash
cp SELF-REVIEW.template.md SELF-REVIEW.md
```

打开 `SELF-REVIEW.md`，按模板填写四题并 commit。

> 这 10 分不考代码质量，考你怎么思考、怎么用 AI、遇到问题怎么解。

## 交付

1. 合并所有 feature 分支到 main
2. 将本次 Claude Code 会话记录（`.jsonl`）放入 `.claude/` 目录并 commit
3. commit SELF-REVIEW.md
4. 推送到自己 GitHub 仓库，当场交 URL
