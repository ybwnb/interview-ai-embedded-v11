# interview-ai-embedded-v1

AI 嵌软工程师面试实测骨架仓库 V2。

## 候选人须知

请阅读 [TASK.md](./TASK.md) 了解考核要求、模块说明和验收步骤。

## 硬件

面试官提供 **Hubo-S3-Lite V5.1** 开发板（ESP32-S3R16N8：16MB Octal PSRAM + 8MB Flash，USB-C 接电脑）。

## Claude Code CLI 配置

```powershell
# 1. 安装（仅首次）
npm install -g @anthropic-ai/claude-code@2.1.111

# 2. 配置（粘贴到 PowerShell，token 从面试官处扫码获取）
$env:ANTHROPIC_BASE_URL = "https://cc.hubosmart.com/api"
$env:ANTHROPIC_AUTH_TOKEN = "<扫码获取>"
$env:ENABLE_TOOL_SEARCH = "true"
$env:CLAUDE_CODE_ATTRIBUTION_HEADER = "0"
$env:CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS = "1"
$env:CLAUDE_CODE_DISABLE_NONESSENTIAL_TRAFFIC = "1"
$env:CLAUDE_CODE_DISABLE_1M_CONTEXT = "1"
$env:CLAUDE_CODE_AUTO_COMPACT_WINDOW = "200000"

# 3. 在项目根目录启动
claude
```

## 快速开始

```bash
# Fork 本仓库到自己账号，然后 clone
git clone git@github.com:<你的账号>/interview-ai-embedded-v1.git
cd interview-ai-embedded-v1

# 设置目标芯片（仅首次）
idf.py set-target esp32s3

# 编译 + 烧录 + 监控（替换 COMx）
idf.py -p COMx flash monitor
```

> **注意：** `main/CMakeLists.txt` 的 REQUIRES 已声明 `ina3221`，但 `components/ina3221/` 缺 `CMakeLists.txt`。  
> 首次 `idf.py build` 会报 `Could not find component 'ina3221'`——这是进入 Module A 的入口信号。  
> Module A 完成（`ina3221/CMakeLists.txt` 从零编写）后，`config_manager_init()` 仍返回 `ESP_ERR_NOT_SUPPORTED`，须完成 Module B 程序才能正常启动。

## 技术栈

ESP-IDF v5.4+ · ESP32-S3 · FreeRTOS · cJSON · NVS · I2C · WS2812B (RMT) · TCA9555 · QMI8658 · INA3221

## 目录结构

```
.
├── components/
│   ├── hubo_bsp/           ← BSP 驱动（已实现，禁止修改）
│   │   ├── include/        ← bsp_i2c.h / bsp_ws2812.h / bsp_tca9555.h / bsp_qmi8658.h
│   │   └── src/            ← 对应实现
│   ├── ina3221/            ← 候选人实现（Module A 移植任务）
│   │   ├── include/ina3221.h ← API 已提供
│   │   ├── src/ina3221.c     ← TODO 骨架（需填写）
│   │   └── CMakeLists.txt    ← ⚠️ 须自行从零编写（不提供模板）
│   └── config_manager/     ← 候选人实现（Module B）
│       ├── include/config_manager.h ← API 已提供
│       └── src/config_manager.c    ← TODO 骨架（需填写）
├── main/
│   └── main.c              ← 候选人修改（Module C/D）
├── reference/
│   └── ina3221_arduino.ino ← Module A 移植参考代码（仅供阅读，不得复制）
├── partitions.csv
├── sdkconfig.defaults
├── TASK.md                 ← 考核要求（必读）
└── SELF-REVIEW.template.md
```

## 交付提醒

完成后执行 `cp SELF-REVIEW.template.md SELF-REVIEW.md`，填写四题并 commit，再推送仓库 URL。详见 [TASK.md](./TASK.md)。
