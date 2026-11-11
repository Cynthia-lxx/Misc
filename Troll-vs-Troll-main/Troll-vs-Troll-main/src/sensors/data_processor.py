"""
Troll-vs-Troll Project
Sensor Data Processor Module

This module processes raw sensor data from accelerometers, gyroscopes,
and other sensors to extract meaningful features for the machine learning
model to predict rollover risk.

Version: 1.3.2
"""

import time
import numpy as np
from collections import deque
from statistics import mean


class SensorDataProcessor:
    """
    Processes raw sensor data to extract features for ML model.
    Implements filtering, feature extraction, and anomaly detection.
    """
    
    def __init__(self, window_size=10):
        """
        Initialize the sensor data processor.
        
        Args:
            window_size (int): Size of the sliding window for data processing
        """
        # TODO: Implement sensor data processing pipeline - HIGH - Developer
        # TODO: Add filtering for sensor noise reduction - MEDIUM - Developer
        # TODO: Extract features for ML model input - HIGH - Developer
        
        self.window_size = window_size
        self.accel_data_buffer = deque(maxlen=window_size)
        self.gyro_data_buffer = deque(maxlen=window_size)
        
        # Statistical measures
        self.mean_buffer = deque(maxlen=window_size)
        self.std_buffer = deque(maxlen=window_size)
        
        print("SensorDataProcessor initialized")

    def add_accel_data(self, accel_data):
        """
        Add accelerometer data to the processing buffer.
        
        Args:
            accel_data (tuple): (x, y, z) acceleration values in m/s^2
        """
        if len(accel_data) != 3:
            raise ValueError("Acceleration data must be a tuple of 3 values (x, y, z)")
        
        self.accel_data_buffer.append(accel_data)
        
        # Calculate derived values
        magnitude = (accel_data[0]**2 + accel_data[1]**2 + accel_data[2]**2)**0.5
        self.mean_buffer.append(magnitude)
        
        # Calculate standard deviation if we have enough data
        if len(self.mean_buffer) > 1:
            self.std_buffer.append(np.std(list(self.mean_buffer)))

    def add_gyro_data(self, gyro_data):
        """
        Add gyroscope data to the processing buffer.
        
        Args:
            gyro_data (tuple): (x, y, z) gyroscope values in rad/s
        """
        if len(gyro_data) != 3:
            raise ValueError("Gyroscope data must be a tuple of 3 values (x, y, z)")
        
        self.gyro_data_buffer.append(gyro_data)

    def get_processed_features(self):
        """
        Extract processed features from the sensor data buffers.
        
        Returns:
            dict: Processed features for ML model
        """
        if len(self.accel_data_buffer) == 0:
            return None
            
        # Get the latest data point
        latest_accel = self.accel_data_buffer[-1]
        latest_gyro = self.gyro_data_buffer[-1] if len(self.gyro_data_buffer) > 0 else (0, 0, 0)
        
        # Calculate features
        ax, ay, az = latest_accel
        gx, gy, gz = latest_gyro
        
        # Acceleration magnitude
        accel_magnitude = (ax**2 + ay**2 + az**2)**0.5
        
        # Tilt angles (pitch and roll)
        pitch = np.arctan2(ax, np.sqrt(ay**2 + az**2)) * 180 / np.pi
        roll = np.arctan2(ay, az) * 180 / np.pi
        
        # Calculate derivatives (rate of change)
        if len(self.accel_data_buffer) > 1:
            prev_ax, prev_ay, prev_az = self.accel_data_buffer[-2]
            accel_change_rate = abs(accel_magnitude - (prev_ax**2 + prev_ay**2 + prev_az**2)**0.5)
        else:
            accel_change_rate = 0
            
        # Calculate statistical features from the window
        if len(self.accel_data_buffer) > 1:
            ax_values = [data[0] for data in self.accel_data_buffer]
            ay_values = [data[1] for data in self.accel_data_buffer]
            az_values = [data[2] for data in self.accel_data_buffer]
            
            ax_mean = mean(ax_values)
            ay_mean = mean(ay_values)
            az_mean = mean(az_values)
            
            ax_std = np.std(ax_values)
            ay_std = np.std(ay_values)
            az_std = np.std(az_values)
        else:
            ax_mean = ax_std = ay_mean = ay_std = az_mean = az_std = 0
            
        # Pack features into a dictionary
        features = {
            'acceleration': {
                'x': ax,
                'y': ay, 
                'z': az,
                'magnitude': accel_magnitude,
                'change_rate': accel_change_rate,
                'mean': {'x': ax_mean, 'y': ay_mean, 'z': az_mean},
                'std': {'x': ax_std, 'y': ay_std, 'z': az_std}
            },
            'gyroscope': {
                'x': gx,
                'y': gy,
                'z': gz
            },
            'orientation': {
                'pitch': pitch,
                'roll': roll
            },
            'timestamp': time.time()
        }
        
        return features

    def detect_anomalies(self):
        """
        Detect anomalies in the sensor data that might indicate rollover risk.
        
        Returns:
            dict: Anomaly detection results
        """
        if len(self.accel_data_buffer) < 3:
            return {'anomaly_detected': False, 'confidence': 0.0}
            
        # Calculate moving average and detect sudden changes
        ax_values = [data[0] for data in self.accel_data_buffer]
        ay_values = [data[1] for data in self.accel_data_buffer]
        az_values = [data[2] for data in self.accel_data_buffer]
        
        # Check for sudden changes in acceleration
        recent_ax = ax_values[-2:]  # Last 2 values
        recent_ay = ay_values[-2:]
        recent_az = az_values[-2:]
        
        # Calculate differences
        if len(recent_ax) == 2:
            ax_change = abs(recent_ax[1] - recent_ax[0])
            ay_change = abs(recent_ay[1] - recent_ay[0])
            az_change = abs(recent_az[1] - recent_az[0])
            
            # Define thresholds for anomaly detection
            threshold = 3.0  # m/s^2
            
            anomaly_detected = (ax_change > threshold or 
                              ay_change > threshold or 
                              az_change > threshold)
            
            max_change = max(ax_change, ay_change, az_change)
            confidence = min(1.0, max_change / threshold)
            
            return {
                'anomaly_detected': anomaly_detected,
                'confidence': confidence,
                'max_change': max_change,
                'threshold': threshold
            }
        else:
            return {'anomaly_detected': False, 'confidence': 0.0}


def main():
    """
    Main function for testing the sensor data processor.
    """
    print("Testing Sensor Data Processor...")
    
    processor = SensorDataProcessor()
    
    # Simulate some sensor data
    sample_accel_data = [
        (0.1, 0.05, 9.81),
        (0.2, 0.1, 9.75),
        (0.3, 0.15, 9.70),
        (1.5, 2.0, 8.5),  # Sudden change indicating possible risk
        (1.6, 2.1, 8.4)
    ]
    
    for data in sample_accel_data:
        processor.add_accel_data(data)
        features = processor.get_processed_features()
        anomalies = processor.detect_anomalies()
        
        print(f"Data: {data}, Features pitch: {features['orientation']['pitch']:.2f}, "
              f"Anomaly: {anomalies['anomaly_detected']}, Confidence: {anomalies['confidence']:.2f}")
    
    print("Sensor data processing test completed.")


if __name__ == "__main__":
    main()