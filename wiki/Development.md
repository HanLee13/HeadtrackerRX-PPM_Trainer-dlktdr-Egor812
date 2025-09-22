# 开发指南

## 项目结构

```
├── src/                 # 源代码目录
│   ├── bt.c             # 蓝牙相关功能
│   ├── bt_client.c      # 蓝牙客户端实现
│   ├── bt_server.c      # 蓝牙服务器实现
│   ├── frskybt.c        # FRSky 蓝牙协议处理
│   ├── ppm.c            # PPM 信号生成
│   ├── main.c           # 主程序入口
│   └── ...              # 其他源文件
├── include/             # 头文件目录
├── lib/                 # 库文件目录
├── doc/                 # 文档和图片
├── platformio.ini       # PlatformIO 配置文件
└── README.md            # 项目说明文件
```

## 开发环境设置

### 安装 PlatformIO

1. 安装 Python 3.7+ (推荐 3.9+)
2. 安装 PlatformIO Core：
   ```bash
   pip install --upgrade platformio
   ```
   或者安装 PlatformIO IDE（VSCode 插件）

### 项目配置

项目使用 PlatformIO 进行构建，基于 ESP-IDF 框架。

#### platformio.ini 配置

主要配置项：
- 平台：espressif32
- 框架：espidf
- 开发板：esp32-c3-devkitm-1 (C3Mini)

构建标志：
- RTOS_FREERTOS=y
- PCB_C3MINI=y
- CPU_ESP32=y
- BLUETOOTH=y
- BLUETOOTH5=y

## 核心功能模块

### PPM 信号生成 (ppm.c)

该模块使用 ESP32 的 RMT（远程控制外设）来生成精确的 PPM 信号：
- 使用 RMT 通道和编码器生成精确的脉冲
- 支持 8 个通道的 PPM 信号输出
- 帧周期为 22.5ms

关键函数：
- `setupRMTChannel()` - 初始化 RMT 通道
- `setupRMTEncoder()` - 设置 RMT 编码器
- `generatePPM()` - 生成 PPM 信号
- `ppmTask()` - PPM 任务主循环

### 蓝牙通信 (bt_client.c, bt_server.c)

蓝牙模块实现了 GATT 客户端功能：
- 扫描并连接到指定的蓝牙设备
- 订阅特征值通知以接收通道数据
- 支持 FRSky Para 协议的数据解析

关键组件：
- ESP32 Bluetooth LE GATT 客户端
- GAP（通用访问配置文件）回调处理
- GATT（通用属性配置文件）事件处理

### FRSky 协议处理 (frskybt.c)

处理 FRSky Para 协议的数据帧：
- 解析接收到的蓝牙数据包
- 将数据转换为 PPM 通道值
- 支持数据帧的编码和解码

关键函数：
- `processFrame()` - 处理接收到的数据帧
- `setTrainer()` - 构建训练器数据帧
- `processTrainerFrame()` - 处理训练器数据帧

### 主程序 (main.c)

主程序初始化系统组件并创建任务：
- 初始化 NVS（非易失性存储）
- 创建 PPM 任务（高优先级）
- 创建 UART 任务
- 加载设置

## 代码规范

### 编码标准

- 遵循 ESP-IDF 编码规范
- 使用 FreeRTOS 任务管理
- 实现线程安全的数据访问（使用互斥锁）

### 命名约定

- 函数名使用驼峰命名法
- 全局变量使用小写字母和下划线
- 常量使用大写字母和下划线

## 构建和调试

### 构建项目

```bash
# 构建 C3Mini 环境
pio run -e C3Mini
```

### 烧录固件

```bash
# 烧录固件
pio run -e C3Mini -t upload
```

### 监视串口输出

```bash
# 监视串口输出
pio run -e C3Mini -t monitor
```

### 调试技巧

1. 使用 ESP-IDF 的日志系统输出调试信息
2. 利用串口监视器查看运行状态
3. 使用 LED 指示灯观察程序运行状态