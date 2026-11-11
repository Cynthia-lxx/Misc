"""
Local Demo for Troll-vs-Troll Project
A complete system that integrates learning, detection, and differential control
with a web-based sensor simulator for testing purposes.

This script runs a local demonstration of the entire system without
requiring the UNIHIKER M10 hardware.
"""

import threading
import time
import sys
import os
from src.ml.rollover_prediction import RolloverPredictor
from src.sensors.data_processor import SensorDataProcessor
from src.control.differential_controller import DifferentialController
from src.utils.data_generator import SensorDataGenerator

def run_web_server():
    """Import and run the web server in a separate thread."""
    from web_server import run_server
    run_server()

def run_local_system():
    """Run the complete local system with simulated data."""
    print("启动Troll-vs-Troll本地演示系统...")
    
    # Initialize all components with default physical parameters
    bag_params = {
        'bag_length': 0.5,  # meters
        'bag_width': 0.3,   # meters
        'bag_height': 0.7,  # meters
        'center_of_gravity_height': 0.4,  # meters
        'wheel_radius': 0.1,  # meters
        'wheel_mass': 0.5,    # kg
        'bag_total_mass': 5.0  # kg
    }
    
    rollover_predictor = RolloverPredictor(**bag_params)
    sensor_processor = SensorDataProcessor()
    differential_controller = DifferentialController(**bag_params)
    data_generator = SensorDataGenerator(seed=int(time.time()))
    
    print("系统初始化完成！")
    print("组件状态：")
    print(f"- 侧翻预测器: {'就绪' if rollover_predictor else '错误'}")
    print(f"- 传感器处理器: {'就绪' if sensor_processor else '错误'}")
    print(f"- 差速控制器: {'就绪' if differential_controller else '错误'}")
    print(f"- 数据生成器: {'就绪' if data_generator else '错误'}")
    
    print("\n使用说明：")
    print("- 运行Web服务器以通过网页界面模拟传感器输入")
    print("- 或使用随机数据生成器进行自动测试")
    
    # Ask user for input method
    print("\n请选择输入模式：")
    print("1. Web界面（通过浏览器拖动滑块）")
    print("2. 随机数据生成器（自动测试）")
    
    try:
        choice = input("请输入选择 (1 或 2): ").strip()
        
        if choice == "1":
            # Start web server in a separate thread
            server_thread = threading.Thread(target=run_web_server, daemon=True)
            server_thread.start()
            print("\nWeb服务器已在后台启动...")
            print("请访问 http://localhost:8080 查看传感器模拟器")
            print("按 Ctrl+C 退出程序\n")
            
            try:
                while True:
                    time.sleep(1)
            except KeyboardInterrupt:
                print("\n正在关闭系统...")
                return
                
        elif choice == "2":
            print("\n使用随机数据生成器进行自动测试...")
            print("按 Ctrl+C 退出程序\n")
            
            try:
                scenario_idx = 0
                scenarios = ["normal", "turning", "risky", "rollover_imminent"]
                
                while True:
                    # Switch scenarios periodically
                    if int(time.time()) % 10 == 0:  # Change scenario every 10 seconds
                        scenario_idx = (scenario_idx + 1) % len(scenarios)
                        data_generator.set_scenario(scenarios[scenario_idx])
                    
                    # Generate sensor data
                    accel_data = data_generator.generate_accel_data()
                    gyro_data = data_generator.generate_gyro_data()
                    
                    # Process sensor data
                    sensor_processor.add_accel_data(accel_data)
                    sensor_processor.add_gyro_data(gyro_data)
                    
                    # Get processed features
                    processed_features = sensor_processor.get_processed_features()
                    
                    # Predict rollover risk
                    risk_assessment = rollover_predictor.predict_rollover_risk(accel_data, gyro_data)
                    
                    # Update controller
                    control_result = differential_controller.update_control(accel_data, gyro_data)
                    
                    # Display results
                    print(f"\n[{time.strftime('%H:%M:%S')}] 当前状态:")
                    print(f"  场景: {data_generator.scenario}")
                    print(f"  加速度: X={accel_data[0]:.2f}, Y={accel_data[1]:.2f}, Z={accel_data[2]:.2f}")
                    print(f"  陀螺仪: X={gyro_data[0]:.2f}, Y={gyro_data[1]:.2f}, Z={gyro_data[2]:.2f}")
                    print(f"  风险等级: {risk_assessment['risk_level']} ({risk_assessment['risk_score']*100:.1f}%)")
                    print(f"  控制激活: {'是' if control_result['control_active'] else '否'}")
                    print(f"  轮速: 左={control_result['left_wheel_speed']:.3f}, 右={control_result['right_wheel_speed']:.3f}")
                    
                    # Update model with new data (simulating online learning)
                    if processed_features:
                        feature_vector = [
                            accel_data[0], accel_data[1], accel_data[2],  # accel
                            gyro_data[0], gyro_data[1], gyro_data[2],    # gyro
                            processed_features['orientation']['pitch'],   # pitch
                            processed_features['orientation']['roll']     # roll
                        ]
                        rollover_predictor.update_model(feature_vector)
                    
                    time.sleep(1)  # Update every second
                    
            except KeyboardInterrupt:
                print("\n正在关闭系统...")
                return
                
        else:
            print("无效选择，退出...")
            return
            
    except KeyboardInterrupt:
        print("\n正在关闭系统...")
        return

def main():
    """Main function to run the local demo."""
    print("=" * 60)
    print("Troll-vs-Troll 本地演示系统")
    print("面向拉杆载具的电子差速防侧翻安全系统")
    print("=" * 60)
    
    run_local_system()

if __name__ == "__main__":
    main()