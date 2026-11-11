"""
Troll-vs-Troll Project
Differential Controller Module

This module implements the control algorithm for the electronic differential
system based on the rollover risk predictions from the ML model. It adjusts
wheel speeds to prevent rollover during turns and sudden movements.

Version: 1.3.2
"""

import time
from ..ml.rollover_prediction import RolloverPredictor
from ..sensors.data_processor import SensorDataProcessor


class DifferentialController:
    """
    Controls the electronic differential system to prevent rollover.
    Uses ML predictions and sensor data to adjust wheel speeds in real-time.
    """
    
    def __init__(self, bag_length=0.5, bag_width=0.3, bag_height=0.7, center_of_gravity_height=0.4, 
                 wheel_radius=0.1, wheel_mass=0.5, bag_total_mass=5.0):
        """
        Initialize the differential controller with physical parameters.
        
        Args:
            bag_length (float): Length of the bag (m)
            bag_width (float): Width of the bag (m) 
            bag_height (float): Height of the bag (m)
            center_of_gravity_height (float): Height of center of gravity from ground (m)
            wheel_radius (float): Radius of wheels (m)
            wheel_mass (float): Mass of each wheel (kg)
            bag_total_mass (float): Total mass of bag with contents (kg)
        """
        # TODO: Implement differential control algorithm - HIGH - Developer
        # TODO: Integrate with ML rollover prediction model - HIGH - Developer
        # TODO: Implement real-time wheel speed adjustment - MEDIUM - Developer
        
        self.rollover_predictor = RolloverPredictor(
            bag_length=bag_length,
            bag_width=bag_width,
            bag_height=bag_height,
            center_of_gravity_height=center_of_gravity_height,
            wheel_radius=wheel_radius,
            wheel_mass=wheel_mass,
            bag_total_mass=bag_total_mass
        )
        self.sensor_processor = SensorDataProcessor()
        
        # Physical parameters
        self.bag_length = bag_length
        self.bag_width = bag_width
        self.bag_height = bag_height
        self.center_of_gravity_height = center_of_gravity_height
        self.wheel_radius = wheel_radius
        self.wheel_mass = wheel_mass
        self.bag_total_mass = bag_total_mass
        
        # Control parameters
        # Adjust max wheel diff based on physical parameters
        self.max_wheel_diff = min(0.5, (self.bag_width / 2) / self.wheel_radius * 0.1)  # Based on geometry
        self.control_threshold = 0.3  # Risk threshold to activate control
        self.last_control_time = time.time()
        self.control_interval = 0.1  # Control update interval in seconds
        
        # Wheel control states
        self.left_wheel_speed = 0.0
        self.right_wheel_speed = 0.0
        self.control_active = False
        
        print(f"DifferentialController initialized with bag dimensions: {bag_length}m x {bag_width}m x {bag_height}m, CoG height: {center_of_gravity_height}m")

    def update_control(self, accel_data, gyro_data=None):
        """
        Update the differential control based on sensor data.
        
        Args:
            accel_data (tuple): (x, y, z) acceleration values
            gyro_data (tuple, optional): (x, y, z) gyroscope values
            
        Returns:
            dict: Control outputs for wheel speeds
        """
        current_time = time.time()
        
        # Limit control update frequency
        if current_time - self.last_control_time < self.control_interval:
            return {
                'left_wheel_speed': self.left_wheel_speed,
                'right_wheel_speed': self.right_wheel_speed,
                'control_active': self.control_active
            }
        
        self.last_control_time = current_time
        
        # Process sensor data
        self.sensor_processor.add_accel_data(accel_data)
        if gyro_data:
            self.sensor_processor.add_gyro_data(gyro_data)
        
        # Get processed features
        features = self.sensor_processor.get_processed_features()
        if not features:
            return {
                'left_wheel_speed': self.left_wheel_speed,
                'right_wheel_speed': self.right_wheel_speed,
                'control_active': False
            }
        
        # Predict rollover risk
        risk_assessment = self.rollover_predictor.predict_rollover_risk(
            accel_data, gyro_data
        )
        
        # Apply differential control if risk is detected
        if risk_assessment['needs_control']:
            self.control_active = True
            
            # Calculate differential based on roll angle
            roll_angle = abs(features['orientation']['roll'])
            
            # Adjust wheel speeds based on risk level
            base_speed = 1.0  # Base speed for normal operation
            risk_factor = risk_assessment['risk_score']
            
            # Calculate differential: more differential for higher risk
            differential = min(self.max_wheel_diff, risk_factor * self.max_wheel_diff * 2)
            
            # Apply differential based on turn direction (sign of roll)
            if features['orientation']['roll'] > 0:
                # Turning right - slow down right wheel
                self.left_wheel_speed = base_speed
                self.right_wheel_speed = max(0.1, base_speed - differential)
            else:
                # Turning left - slow down left wheel
                self.left_wheel_speed = max(0.1, base_speed - differential)
                self.right_wheel_speed = base_speed
        else:
            # Normal operation - equal wheel speeds
            self.control_active = False
            base_speed = 1.0
            self.left_wheel_speed = base_speed
            self.right_wheel_speed = base_speed
        
        return {
            'left_wheel_speed': self.left_wheel_speed,
            'right_wheel_speed': self.right_wheel_speed,
            'control_active': self.control_active,
            'risk_assessment': risk_assessment
        }

    def get_wheel_speeds(self):
        """
        Get the current wheel speeds.
        
        Returns:
            tuple: (left_wheel_speed, right_wheel_speed)
        """
        return (self.left_wheel_speed, self.right_wheel_speed)

    def reset_control(self):
        """
        Reset the control system to default state.
        """
        self.left_wheel_speed = 0.0
        self.right_wheel_speed = 0.0
        self.control_active = False
        self.rollover_predictor = RolloverPredictor()
        self.sensor_processor = SensorDataProcessor()


def main():
    """
    Main function for testing the differential controller.
    """
    print("Testing Differential Controller...")
    
    controller = DifferentialController()
    
    # Simulate normal operation
    normal_accel = (0.1, 0.05, 9.81)
    result = controller.update_control(normal_accel)
    print(f"Normal operation: {result}")
    
    # Simulate high risk situation
    risky_accel = (1.5, 3.0, 8.5)  # High roll angle
    result = controller.update_control(risky_accel)
    print(f"High risk operation: {result}")
    
    print("Differential controller test completed.")


if __name__ == "__main__":
    main()