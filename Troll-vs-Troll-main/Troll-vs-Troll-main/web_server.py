"""
Web Server for Troll-vs-Troll Project
Provides a web interface to simulate sensor inputs for testing the rollover prediction and differential control system.

This server allows users to drag a 3D cube to simulate accelerometer and gyroscope data,
which is then sent to the local control system for processing and display.

## 版本日志
- v1.0.0 2025-12-28: 初始版本 - 成功
- v1.1.0 2026-02-17: 添加机器学习训练接口 - 测试中
- v1.3.2 2026-02-18: 集成反馈控制系统和配置驱动频率管理 - 测试中

Version: 1.3.2
"""

from bottle import Bottle, route, static_file, run, request, response
import json
import threading
import time
from collections import deque
from src.ml.rollover_prediction import RolloverPredictor
from src.sensors.data_processor import SensorDataProcessor
from src.control.differential_controller import DifferentialController
from src.control.feedback_controller import FeedbackController
from src.ml.control_rl import ControlRLAgent

app = Bottle()

# Load system configuration
try:
    with open('system_config.json', 'r', encoding='utf-8') as f:
        system_config = json.load(f)
    ui_settings = system_config.get('ui_settings', {})
    polling_intervals = ui_settings.get('data_polling_intervals', {})
except Exception as e:
    print(f"Warning: Could not load system config: {e}")
    # Default values
    polling_intervals = {
        'sensor_data_update': 50,
        'visualization_refresh': 500,
        'learning_mode_polling': 200,
        'performance_report_update': 1000,
        'control_effectiveness_eval': 100
    }

# Global instances of our control systems with default physical parameters
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
feedback_controller = FeedbackController(**bag_params)
rl_agent = ControlRLAgent()

# Current sensor values
current_sensor_data = {
    'accel_x': 0.0,
    'accel_y': 0.0,
    'accel_z': 9.8,
    'gyro_x': 0.0,
    'gyro_y': 0.0,
    'gyro_z': 0.0
}

# Store recent data for visualization
recent_results = deque(maxlen=50)  # Store last 50 results for charts

def load_template(template_name):
    """Load HTML template from the templates directory."""
    with open(f'templates/{template_name}', 'r', encoding='utf-8') as f:
        return f.read()

@app.route('/')
def input_page():
    """Serve the sensor input page."""
    return load_template('index.html')

@app.route('/params')
def params_page():
    """Serve the physical parameters configuration page."""
    return load_template('params.html')

@app.route('/visualization')
def visualization_page():
    """Serve the visualization page with charts."""
    return load_template('visualization.html')

@app.route('/api/send_sensor_data', method='POST')
def send_sensor_data():
    """Receive sensor data from the web interface and process it with feedback control."""
    global current_sensor_data
    
    # Get JSON data from request
    data = request.json
    
    # Update current sensor data
    current_sensor_data = {
        'accel_x': data['accel_x'],
        'accel_y': data['accel_y'],
        'accel_z': data['accel_z'],
        'gyro_x': data['gyro_x'],
        'gyro_y': data['gyro_y'],
        'gyro_z': data['gyro_z']
    }
    
    # Prepare accelerometer and gyroscope data
    accel_data = (data['accel_x'], data['accel_y'], data['accel_z'])
    gyro_data = (data['gyro_x'], data['gyro_y'], data['gyro_z'])
    
    # Process sensor data
    sensor_processor.add_accel_data(accel_data)
    sensor_processor.add_gyro_data(gyro_data)
    
    # Get processed features
    processed_features = sensor_processor.get_processed_features()
    
    # Predict rollover risk
    risk_assessment = rollover_predictor.predict_rollover_risk(accel_data, gyro_data)
    
    # Execute feedback control
    control_context = {
        'accel_data': accel_data,
        'gyro_data': gyro_data,
        'risk_score': risk_assessment['risk_score'],
        'roll_angle': processed_features['orientation']['roll'] if processed_features else 0,
        'pitch_angle': processed_features['orientation']['pitch'] if processed_features else 0,
        'accel_magnitude': (accel_data[0]**2 + accel_data[1]**2 + accel_data[2]**2)**0.5
    }
    
    control_result = feedback_controller.execute_control_action(accel_data, gyro_data)
    
    # Evaluate control effectiveness
    post_control_state = {
        'risk_score': risk_assessment['risk_score'],
        'roll_angle': processed_features['orientation']['roll'] if processed_features else 0,
        'pitch_angle': processed_features['orientation']['pitch'] if processed_features else 0,
        'accel_magnitude': (accel_data[0]**2 + accel_data[1]**2 + accel_data[2]**2)**0.5
    }
    
    effectiveness = feedback_controller.evaluate_control_effectiveness(post_control_state)
    
    # Record learning sample
    feedback_controller.record_learning_sample(control_context, effectiveness)
    
    # Get RL recommendation
    rl_policy = rl_agent.get_policy(control_context)
    
    # Helper function to convert numpy types to native Python types
    def convert_numpy_types(obj):
        import numpy as np
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        elif isinstance(obj, np.floating):
            return float(obj)
        elif isinstance(obj, np.integer):
            return int(obj)
        elif isinstance(obj, np.bool_):
            return bool(obj)
        elif isinstance(obj, (list, tuple)):
            return [convert_numpy_types(item) for item in obj]
        elif isinstance(obj, dict):
            return {key: convert_numpy_types(value) for key, value in obj.items()}
        else:
            return obj
    
    # Combine all results
    result = {
        'risk_assessment': risk_assessment,
        'left_wheel_speed': control_result['left_wheel_speed'],
        'right_wheel_speed': control_result['right_wheel_speed'],
        'control_active': control_result['control_active'],
        'processed_features': processed_features,
        'control_effectiveness': effectiveness,
        'rl_recommendation': rl_policy,
        'performance_report': feedback_controller.get_performance_report()
    }
    
    # Convert numpy types to native Python types for JSON serialization
    result = convert_numpy_types(result)
    
    # Add result to recent results for visualization
    recent_results.append(result)
    
    response.content_type = 'application/json'
    return json.dumps(result, indent=2)

@app.route('/api/update_parameters', method='POST')
def update_parameters():
    """Update the physical parameters of the system."""
    global rollover_predictor, differential_controller
    
    params = request.json
    
    # Update the global parameters
    for key in bag_params.keys():
        if key in params:
            bag_params[key] = params[key]
    
    # Reinitialize the controllers with new parameters
    rollover_predictor = RolloverPredictor(**bag_params)
    differential_controller = DifferentialController(**bag_params)
    
    response.content_type = 'application/json'
    return json.dumps({'status': 'success', 'message': 'Parameters updated successfully', 'params': bag_params})

@app.route('/api/get_current_data')
def get_current_data():
    """Get current sensor data."""
    response.content_type = 'application/json'
    return json.dumps(current_sensor_data)

@app.route('/api/get_realtime_sensor_data')
def get_realtime_sensor_data():
    """Get real-time sensor data for learning mode."""
    global current_sensor_data
    
    # Get processed features for orientation data
    processed_features = sensor_processor.get_processed_features()
    
    response_data = {
        'sensor_data': current_sensor_data,
        'processed_features': processed_features,
        'timestamp': time.time()
    }
    
    response.content_type = 'application/json'
    return json.dumps(response_data)

@app.route('/api/get_latest_results')
def get_latest_results():
    """Get the latest results for visualization."""
    response.content_type = 'application/json'
    return json.dumps({'results': list(recent_results)})

@app.route('/learning')
def learning_mode():
    """Serve the learning mode page."""
    return load_template('learning_mode.html')

@app.route('/api/save_training_data', method='POST')
def save_training_data():
    """Save training data from learning mode."""
    data = request.json
    
    # Save to training data file
    training_data_file = 'training_data.json'
    try:
        if os.path.exists(training_data_file):
            with open(training_data_file, 'r', encoding='utf-8') as f:
                existing_data = json.load(f)
        else:
            existing_data = []
        
        existing_data.append(data)
        
        with open(training_data_file, 'w', encoding='utf-8') as f:
            json.dump(existing_data, f, indent=2, ensure_ascii=False)
        
        response.content_type = 'application/json'
        return json.dumps({'status': 'success', 'message': '数据保存成功'})
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'status': 'error', 'message': str(e)})

@app.route('/api/clear_training_data', method='POST')
def clear_training_data():
    """Clear all training data."""
    try:
        if os.path.exists('training_data.json'):
            os.remove('training_data.json')
        
        response.content_type = 'application/json'
        return json.dumps({'status': 'success', 'message': '数据已清空'})
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'status': 'error', 'message': str(e)})

@app.route('/api/train_rl_model', method='POST')
def train_rl_model():
    """Train the reinforcement learning model with collected data."""
    try:
        # Export training data from feedback controller
        sample_count = feedback_controller.export_training_data('rl_training_data.json')
        
        # Load and train RL agent
        # Note: In practice, you'd load the exported data and train the agent
        # This is a simplified version
        
        response.content_type = 'application/json'
        return json.dumps({
            'status': 'success', 
            'message': f'Training data exported ({sample_count} samples)',
            'training_required': sample_count > 100
        })
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'status': 'error', 'message': str(e)})

@app.route('/api/get_feedback_report')
def get_feedback_report():
    """Get detailed feedback control performance report."""
    try:
        report = feedback_controller.get_performance_report()
        response.content_type = 'application/json'
        return json.dumps(report, indent=2)
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'error': str(e)})

@app.route('/api/reset_feedback_system', method='POST')
def reset_feedback_system():
    """Reset the feedback control system."""
    try:
        global feedback_controller, rl_agent
        feedback_controller = FeedbackController(**bag_params)
        rl_agent = ControlRLAgent()
        
        response.content_type = 'application/json'
        return json.dumps({'status': 'success', 'message': 'Feedback system reset'})
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'status': 'error', 'message': str(e)})

@app.route('/api/get_config')
def get_config():
    """Get system configuration."""
    try:
        with open('system_config.json', 'r', encoding='utf-8') as f:
            config = json.load(f)
        response.content_type = 'application/json'
        return json.dumps(config)
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'error': str(e)})

@app.route('/api/get_polling_intervals')
def get_polling_intervals():
    """Get polling intervals configuration."""
    try:
        response.content_type = 'application/json'
        return json.dumps(polling_intervals)
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'error': str(e)})

@app.route('/api/update_config', method='POST')
def update_config():
    """Update system configuration."""
    try:
        new_config = request.json
        
        # 读取现有配置
        with open('system_config.json', 'r', encoding='utf-8') as f:
            current_config = json.load(f)
        
        # 深度合并配置
        def deep_merge(old, new):
            for key, value in new.items():
                if key in old and isinstance(old[key], dict) and isinstance(value, dict):
                    deep_merge(old[key], value)
                else:
                    old[key] = value
            return old
        
        updated_config = deep_merge(current_config, new_config)
        
        # 保存更新后的配置
        with open('system_config.json', 'w', encoding='utf-8') as f:
            json.dump(updated_config, f, indent=2, ensure_ascii=False)
        
        response.content_type = 'application/json'
        return json.dumps({'status': 'success', 'message': '配置已更新'})
    except Exception as e:
        response.status = 500
        response.content_type = 'application/json'
        return json.dumps({'status': 'error', 'message': str(e)})

def run_server():
    """Run the web server in a separate thread."""
    run(app, host='localhost', port=8080, debug=False)

if __name__ == '__main__':
    print("启动Troll-vs-Troll Web服务器...")
    print("请访问 http://localhost:8080 查看传感器输入页面")
    run_server()