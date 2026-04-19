/**
 * INA3221 三通道电流/电压监测 — Arduino 参考实现
 *
 * 芯片：Texas Instruments INA3221
 * 通信：I2C，默认地址 0x40（A0/A1 接 GND）
 * 分流电阻：0.1 Ω（Hubo-S3-Lite V5.1 板载）
 *
 * 面试说明：
 *   本文件仅作移植参考，不得直接复制。
 *   你的任务是读懂这里的寄存器时序和换算逻辑，
 *   将其移植为 ESP-IDF 组件 components/ina3221/。
 */

#include <Wire.h>

// ── I2C 地址 ──────────────────────────────────────────────
#define INA3221_ADDR  0x40

// ── 寄存器地址 ────────────────────────────────────────────
#define REG_CONFIG    0x00   // 配置寄存器
#define REG_CH1_SHUNT 0x01   // 通道 1 分流电压
#define REG_CH1_BUS   0x02   // 通道 1 母线电压
#define REG_CH2_SHUNT 0x03   // 通道 2 分流电压
#define REG_CH2_BUS   0x04   // 通道 2 母线电压
#define REG_CH3_SHUNT 0x05   // 通道 3 分流电压
#define REG_CH3_BUS   0x06   // 通道 3 母线电压
#define REG_MFID      0xFE   // 厂商 ID，期望值 0x5449 ('TI')
#define REG_DIEID     0xFF   // Die ID，期望值 0x3220

// ── 物理常量 ──────────────────────────────────────────────
#define SHUNT_RESISTOR 0.1f    // 分流电阻 (Ω)
#define SHUNT_LSB_UV   40.0f   // 分流电压寄存器 LSB = 40 μV
#define BUS_LSB_MV     8.0f    // 母线电压寄存器 LSB = 8 mV

// ── 底层 I2C 读写 ─────────────────────────────────────────

static uint16_t readReg16(uint8_t reg) {
    Wire.beginTransmission(INA3221_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);       // repeated start，不释放总线
    Wire.requestFrom(INA3221_ADDR, 2);
    uint8_t hi = Wire.read();
    uint8_t lo = Wire.read();
    return (uint16_t)(hi << 8) | lo;  // big-endian
}

static void writeReg16(uint8_t reg, uint16_t val) {
    Wire.beginTransmission(INA3221_ADDR);
    Wire.write(reg);
    Wire.write((uint8_t)(val >> 8));
    Wire.write((uint8_t)(val & 0xFF));
    Wire.endTransmission();
}

// ── 初始化 ────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // 验证厂商 ID
    uint16_t mfid = readReg16(REG_MFID);
    Serial.print("Manufacturer ID: 0x");
    Serial.println(mfid, HEX);   // 应打印 5449

    if (mfid != 0x5449) {
        Serial.println("INA3221 NOT FOUND");
        while (1) delay(1000);
    }

    // 配置寄存器 0x7427：
    //   [15]    = 0     RST=不复位
    //   [14:12] = 111   CH1/CH2/CH3 全部使能
    //   [11:9]  = 000   AVG=1次（无平均）
    //   [8:6]   = 100   VBUS_CT=2.116ms
    //   [5:3]   = 100   VSHUNT_CT=2.116ms
    //   [2:0]   = 111   MODE=连续采样（shunt+bus）
    writeReg16(REG_CONFIG, 0x7427);
    delay(10);

    Serial.println("INA3221 ready.");
}

// ── 业务读取函数 ──────────────────────────────────────────

/**
 * 读取母线电压（单位 V）
 * @param channel 通道编号 1~3
 */
float getBusVoltage_V(uint8_t channel) {
    uint8_t reg = REG_CH1_BUS + (channel - 1) * 2;
    int16_t raw = (int16_t)readReg16(reg);
    raw >>= 3;                          // bits[15:3] 有效，低 3 位固定为 0
    return raw * (BUS_LSB_MV / 1000.0f);
}

/**
 * 读取通道电流（单位 mA）
 * @param channel 通道编号 1~3
 */
float getCurrent_mA(uint8_t channel) {
    uint8_t reg = REG_CH1_SHUNT + (channel - 1) * 2;
    int16_t raw = (int16_t)readReg16(reg);  // 有符号，支持反向电流
    raw >>= 3;
    float shunt_v = raw * (SHUNT_LSB_UV * 1e-6f);   // 转换为 V
    return (shunt_v / SHUNT_RESISTOR) * 1000.0f;    // I = V/R，转换为 mA
}

// ── 主循环 ────────────────────────────────────────────────

void loop() {
    for (uint8_t ch = 1; ch <= 3; ch++) {
        float v = getBusVoltage_V(ch);
        float i = getCurrent_mA(ch);
        Serial.print("CH"); Serial.print(ch);
        Serial.print(": ");
        Serial.print(v, 3); Serial.print(" V  ");
        Serial.print(i, 2); Serial.println(" mA");
    }
    Serial.println("---");
    delay(1000);
}
