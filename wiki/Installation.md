# 安装指南

## 硬件要求

### 所需硬件

- ESP32-C3 开发板（如 Waveshare ESP32-C3-Zero 或 Seed Studio XIAO-ESP32C3）
- 3.5mm TRRS 插孔或 3.5mm 单声道 TS 插孔
- 支持 FRSky Para 协议的头部追踪器（如运行 ysoldak 固件的 Seed Studio nRF52840 Sense）
- USB 数据线（用于固件烧录和配置）

### 连接方式

#### 方式一：使用 3.5mm TRRS 插孔

```
插孔 3.5mm 4针 TRRS
|= = = >
 A B C D
A - 未使用
B 和 C - GND
D - PPM - D0 (GPIO2)
```

#### 方式二：使用 3.5mm 单声道 TS 插孔

```
插孔 3.5mm 单声道 TS
|=== >
  A  D
A - GND
D - D0 (GPIO2)
```

### 硬件设置

1. 在遥控器上设置：
   - Model-Setup-Trainer mode: 设置为 Master/Jack
   - Sys-Trainer: 配置通道和微调

2. ESP32C3 与 Radiomaster 电台的接口：
   - 使用焊接到地面和 D0 的教练电缆
   - 通过 Radiomaster 辅助端口的电源和地线供电

## 固件烧录

### 下载预编译固件

1. 访问项目的 GitHub Actions 页面
2. 选择一个最近的工作流运行
3. 在 ARTIFACTS 部分下载固件文件
4. 解压后会得到以下文件：
   - bootloader.bin
   - partitions.bin
   - firmware.bin

### 使用 Web 烧录工具

推荐使用 Spacehuhn 的 Web 烧录工具：https://esp.huhn.me/

烧录步骤：
1. 按住开发板的 BOOT 按钮，然后按 RESET 按钮
2. 点击 ERASE 等待擦除完成
3. 选择以下文件进行烧录：
   ```
   0x0 bootloader.bin
   0x8000 partitions.bin
   0x10000 firmware.bin
   ```
4. 点击 PROGRAM 等待烧录完成
5. 烧录完成后按 RESET 按钮重启设备

### 使用 PlatformIO 本地编译烧录

#### 安装依赖

1. 安装 Python
2. 安装 PlatformIO：
   ```bash
   pip install --upgrade platformio
   ```

#### 构建和烧录项目

```bash
# 构建 C3Mini 环境
pio run -e C3Mini

# 烧录固件
pio run -e C3Mini -t upload

# 监视串口输出
pio run -e C3Mini -t monitor
```