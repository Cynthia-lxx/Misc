# Troll-vs-Troll

## Version Log
- v1.0.0 2025-12-28: Initial version - Success
- v1.0.1 2025-12-18至2026-01: 差速处理内核开发 - 测试未通过（M10 API不熟悉）
- v1.1.0 2025-12-28: Added UNIHIKER M10 hardware foundation information - Success
- v1.2.0 2026-02-17: 完成本地开发环境和Web界面 - Success
- v1.3.0 2026-02-17: 新增机器学习增强功能 - 测试中
- v1.3.1 2026-02-17: 系统架构重大升级 - 待测试
- v1.3.2 2026-02-18: **新增智能闭环控制系统和配置驱动的频率管理 - 测试中**

## Project Version: 1.3.2 - 智能闭环控制版 (测试中)

*For SES student project*: Trolley-Anti-Troll is an electronic differential system for pull-handle carriers (e.g., suitcases) to prevent rollover. It replaces mechanical structures with lightweight electronic control, using real-time wheel slip monitoring to enhance stability during turns. Low-cost, energy-efficient, and easy to deploy.

This project will be developed around and based on the UNIHIKER M10 Model flight control board as the hardware foundation.

**重要说明**: 本项目自2025年12月28日起文档未更新，请注意时效性。

**项目时效性提醒**: 如您在README及其他自述性文件中看到此提醒，请注意这些文档可能已过时。项目实际进展情况请参考Update_Log.md文件。

**🚨 行空板开发重要提示**: 所有涉及UNIHIKER M10行空板的代码开发都**必须严格参照/ref目录下的官方文档**，包括但不限于：
- [行空板官方文档-unihiker库](./ref/UNIHIKER-M10-Documentation/行空板官方文档-unihiker库.html) - 屏幕显示和GUI控制
- [行空板官方文档 - pinpong库](./ref/UNIHIKER-M10-Documentation/行空板官方文档 - pinpong库.html) - 硬件接口和传感器控制

严禁在未查阅官方文档的情况下进行行空板相关开发！

**✨ 最新功能**: 智能闭环控制系统
- 实现完整的反馈控制循环：传感器输入→数据分析→控制执行→效果评估→机器学习优化
- 集成强化学习代理进行策略优化
- 所有数据传输频率参数统一配置化管理
- 新增API端点支持实时性能监控和模型训练
- 统一配置管理系统，所有参数集中管理
- API驱动的动态参数加载和更新
- 一体化用户界面，整合运动控制与传感器输入
- 实时数据流统一，支持差速控制、机器学习、物理仿真协同工作
- 去中心化参数设计，提高系统可维护性和扩展性

**⚠️ 重要技术警告**: 差速控制内核存在严重设计缺陷：
1. 会阻止所有正常转向操作
2. 错误的姿态角计算方法导致日常使用中频繁误判

详情请查看Update_Log.md中的问题记录。

项目当前状态：智能闭环控制系统开发完成，正在进行集成测试验证。

## Project Features and Usage

Trolley-Anti-Troll is designed to prevent rollover in pull-handle carriers (e.g., suitcases) by implementing an electronic differential system. The system monitors wheel slip in real-time and adjusts wheel speeds during turns to maintain stability.

For detailed version history and updates, please see [Update_Log.md](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/Update_Log.md).

## New Feature: UNIHIKER M10 Benchmark Demo

The project now includes a comprehensive benchmark demo ([benchmark.py](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/src/main/benchmark.py)) that tests all onboard sensors, display components, and computational performance of the UNIHIKER M10 board. The demo features a page-based UI to navigate through different sensor readings and performance metrics.

## Machine Learning Component

The project now includes a machine learning module for predicting rollover risk based on sensor data. The [rollover_prediction.py](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/src/ml/rollover_prediction.py) module implements algorithms to predict when the pull-handle carrier is at risk of rollover using accelerometer and gyroscope data.

## Sensor Data Processing

The [data_processor.py](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/src/sensors/data_processor.py) module processes raw sensor data to extract meaningful features for the machine learning model. It includes filtering, feature extraction, and anomaly detection capabilities.

## Differential Control System

The [differential_controller.py](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/src/control/differential_controller.py) module implements the electronic differential control algorithm. It adjusts wheel speeds based on the machine learning model's rollover risk predictions to prevent side tipping during turns and sudden movements.

## Sensor Data Generation

The [data_generator.py](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/src/utils/data_generator.py) module generates realistic sensor data for training and testing the machine learning models. The data simulates real-world scenarios for pull-handle carriers including normal movement, turns, and rollover risks.

## 项目进展历史

### 前期开发 (2025/12/18 - 2026年1月)
- 开发了main.py主程序及相关差速处理内核
- **测试结果：未通过**
- **失败原因**：未完全了解M10硬件的API，ref中的文档可能不够完备
- 差速处理内核仍在测试中

### 当前阶段 (2026/2/17)
完成了如您所见的其余内容：
- 本地（脱离M10硬件）的完整测试模板
- 相应的控制服务器(web_server.py)
- 静态内容和Web界面
- 3D传感器数据模拟器
- 本地演示系统

## Important Notice

If you are a developer or an AI tool assisting in writing project code, you **must** thoroughly read and strictly follow all guidelines in [Developer_Guidelines.md](file:///E:/Comp/特需/Troll-vs-Troll-main/Troll-vs-Troll-main/Developer_Guidelines.md). If changes are made that violate these guidelines, it would be better not to make them at all, and such changes should be reverted.