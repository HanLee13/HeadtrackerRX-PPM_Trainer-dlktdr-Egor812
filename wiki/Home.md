# BT-PPM Module Wiki

欢迎来到 BT-PPM Module 项目的 Wiki 页面！

## 项目简介

BT-PPM Module 是一个基于 ESP32-C3 的蓝牙接收器项目，用于无线头部追踪器与 FRSky Para 协议的通信。该项目基于 dlktdr 的 BTWifiModule 和 Egor812 的 HeadtrackerRX 进行开发。

## 功能特性

- 通过蓝牙接收来自头部追踪器的数据（使用 FRSky Para 协议）
- 将接收到的数据通过 PPM（脉冲位置调制）信号发送到遥控器的教练端口
- 支持与 ysoldak 的 HeadTracker 固件配合使用（运行在 Seed Studio nRF52840 Sense 上）
- 适用于不想在设备中焊接蓝牙接收器的用户

## 目录

- [安装指南](Installation.md) - 硬件连接和固件烧录说明
- [使用说明](Usage.md) - 蓝牙配对和日常使用指南
- [开发指南](Development.md) - 项目结构和开发环境设置
- [故障排除](Troubleshooting.md) - 常见问题和解决方案