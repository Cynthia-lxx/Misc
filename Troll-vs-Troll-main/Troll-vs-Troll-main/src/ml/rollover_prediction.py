"""
Troll-vs-Troll Project
Rollover Risk Prediction Module

This module implements machine learning algorithms to predict rollover 
risk based on sensor data (accelerometer, gyroscope, etc.). Uses 
real-time data to determine when differential control is needed.

Version: 1.3.2
"""

import time
import numpy as np
from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import StandardScaler


class RolloverPredictor:
    """
    Machine learning model to predict rollover risk based on sensor data.
    Uses a combination of anomaly detection and physics-based algorithms
    to determine when the pull-handle carrier is at risk of rollover.
    """
    
    def __init__(self, bag_length=0.5, bag_width=0.3, bag_height=0.7, center_of_gravity_height=0.4, 
                 wheel_radius=0.1, wheel_mass=0.5, bag_total_mass=5.0):
        """
        Initialize the rollover prediction model with physical parameters.
        
        Args:
            bag_length (float): Length of the bag (m)
            bag_width (float): Width of the bag (m) 
            bag_height (float): Height of the bag (m)
            center_of_gravity_height (float): Height of center of gravity from ground (m)
            wheel_radius (float): Radius of wheels (m)
            wheel_mass (float): Mass of each wheel (kg)
            bag_total_mass (float): Total mass of bag with contents (kg)
        """
        # TODO: Implement machine learning model for rollover prediction - HIGH - Developer
        # TODO: Use accelerometer and other sensor data to predict rollover risk - HIGH - Developer
        # TODO: Implement real-time prediction algorithm - MEDIUM - Developer
        
        # Initialize ML components
        self.scaler = StandardScaler()
        self.anomaly_detector = IsolationForest(contamination=0.1, random_state=42)
        
        # Physical parameters of the bag
        self.bag_length = bag_length  # m (length along the handle pulling direction)
        self.bag_width = bag_width   # m (width of the bag, perpendicular to turning direction)
        self.bag_height = bag_height # m (height of the bag)
        self.center_of_gravity_height = center_of_gravity_height  # m (CoG height)
        self.wheel_radius = wheel_radius  # m
        self.wheel_mass = wheel_mass      # kg
        self.bag_total_mass = bag_total_mass  # kg
        
        # Derived parameters
        self.total_system_mass = bag_total_mass + 2 * wheel_mass  # Including both wheels
        
        # Physics-based rollover thresholds
        # Critical angle based on geometry: tan(θ) = width/(2*height_of_CoG)
        self.geometric_critical_angle = np.arctan(self.bag_width / (2 * self.center_of_gravity_height)) * 180 / np.pi
        
        # Moment of inertia approximations
        self.moment_of_inertia_yaw = (1/12) * self.bag_total_mass * (self.bag_length**2 + self.bag_height**2)
        self.moment_of_inertia_roll = (1/12) * self.bag_total_mass * (self.bag_width**2 + self.bag_height**2)
        
        # Thresholds based on physics calculations
        self.rollover_angle_threshold = self.geometric_critical_angle  # degrees
        self.wheel_slip_threshold = 0.1  # ratio
        
        # Store historical data for prediction
        self.historical_data = []
        self.max_history = 100
        
        # Model parameters
        self.is_trained = False
        self.normal_behavior_model = None
        
        print(f"RolloverPredictor initialized with bag dimensions: {bag_length}m x {bag_width}m x {bag_height}m, CoG height: {center_of_gravity_height}m")

    def preprocess_sensor_data(self, accel_data, gyro_data=None, time_stamp=None):
        """
        Preprocess sensor data for ML model.
        
        Args:
            accel_data (tuple): (x, y, z) acceleration values
            gyro_data (tuple, optional): (x, y, z) gyroscope values
            time_stamp (float, optional): Timestamp of the reading
            
        Returns:
            np.array: Processed feature vector
        """
        # Extract features from raw sensor data
        ax, ay, az = accel_data
        
        # Calculate magnitude of acceleration
        accel_magnitude = (ax**2 + ay**2 + az**2)**0.5
        
        # Calculate tilt angles (simplified)
        pitch = np.arctan2(ax, np.sqrt(ay**2 + az**2)) * 180 / np.pi
        roll = np.arctan2(ay, az) * 180 / np.pi
        
        # Create feature vector
        features = [ax, ay, az, accel_magnitude, pitch, roll]
        
        if gyro_data:
            gx, gy, gz = gyro_data
            features.extend([gx, gy, gz])
        
        if time_stamp:
            features.append(time_stamp)
            
        return np.array(features).reshape(1, -1)

    def predict_rollover_risk(self, accel_data, gyro_data=None):
        """
        Predict the rollover risk based on sensor data and physical parameters.
        
        Args:
            accel_data (tuple): (x, y, z) acceleration values
            gyro_data (tuple, optional): (x, y, z) gyroscope values
            
        Returns:
            dict: Risk assessment with probability and confidence
        """
        # Preprocess the input data
        features = self.preprocess_sensor_data(accel_data, gyro_data, time.time())
        
        ax, ay, az = accel_data
        
        # Calculate magnitude of acceleration
        accel_magnitude = (ax**2 + ay**2 + az**2)**0.5
        
        # Calculate tilt angles
        pitch = np.arctan2(ax, np.sqrt(ay**2 + az**2)) * 180 / np.pi
        roll = np.arctan2(ay, az) * 180 / np.pi
        
        # PHYSICS-BASED ROLLOVER CALCULATION
        # Calculate lateral force causing rollover (side acceleration)
        lateral_force = self.total_system_mass * abs(ay)  # Force in y-direction
        
        # Calculate restoring moment from weight
        restoring_moment = self.total_system_mass * 9.81 * (self.bag_width / 2)  # Weight * moment arm
        
        # Calculate overturning moment from lateral acceleration
        overturning_moment = self.total_system_mass * abs(ay) * self.center_of_gravity_height
        
        # Rollover factor (ratio of overturning to restoring moment)
        rollover_factor = overturning_moment / restoring_moment if restoring_moment != 0 else 0
        
        # Use gyroscope data if available to calculate angular velocity effects
        if gyro_data:
            gx, gy, gz = gyro_data
            
            # Calculate angular acceleration effects
            # Angular velocity in roll direction affects stability
            roll_rate_effect = abs(gy) * self.center_of_gravity_height
            
            # Factor in angular momentum effects
            angular_momentum_effect = (self.moment_of_inertia_roll * abs(gy)) / (self.bag_width / 2)
        else:
            roll_rate_effect = 0
            angular_momentum_effect = 0
        
        # Calculate risk based on physics model
        # The rollover occurs when overturning moment > restoring moment
        theoretical_stability_limit = restoring_moment / (self.total_system_mass * self.center_of_gravity_height)
        current_lateral_acceleration_ratio = abs(ay) / theoretical_stability_limit if theoretical_stability_limit != 0 else 0
        
        # Calculate risk score based on multiple factors
        geometric_risk = min(1.0, rollover_factor)  # Based on static stability
        dynamic_risk = min(1.0, abs(ay) / 9.81)  # Based on lateral g-force
        angular_risk = min(1.0, roll_rate_effect / 5.0)  # Based on angular rates
        
        # Combine all risk factors with weights
        risk_score = 0.6 * geometric_risk + 0.3 * dynamic_risk + 0.1 * angular_risk
        
        # Determine risk level
        if risk_score > 0.8:
            risk_level = "HIGH"
        elif risk_score > 0.4:
            risk_level = "MEDIUM"
        else:
            risk_level = "LOW"
            
        return {
            "risk_score": float(risk_score),
            "risk_level": risk_level,
            "tilt_angle": max(abs(pitch), abs(roll)),
            "acceleration": float(accel_magnitude),
            "lateral_force": float(lateral_force),
            "restoring_moment": float(restoring_moment),
            "overturning_moment": float(overturning_moment),
            "rollover_factor": float(rollover_factor),
            "geometric_critical_angle": float(self.geometric_critical_angle),
            "needs_control": risk_score > 0.3,
            "bag_length": self.bag_length,
            "bag_width": self.bag_width,
            "bag_height": self.bag_height,
            "center_of_gravity_height": self.center_of_gravity_height,
            "wheel_radius": self.wheel_radius,
            "wheel_mass": self.wheel_mass,
            "bag_total_mass": self.bag_total_mass,
            "total_system_mass": self.total_system_mass
        }

    def update_model(self, new_data_point):
        """
        Update the model with new data (online learning).
        
        Args:
            new_data_point: New sensor data point to learn from
        """
        # Add to historical data
        self.historical_data.append(new_data_point)
        
        # Keep only recent data
        if len(self.historical_data) > self.max_history:
            self.historical_data.pop(0)
            
        # Retrain if enough data is available
        if len(self.historical_data) > 10 and not self.is_trained:
            self._train_model()
            self.is_trained = True

    def _train_model(self):
        """
        Internal method to train the ML model on historical data.
        """
        if len(self.historical_data) == 0:
            return
            
        # Convert historical data to feature matrix
        feature_matrix = np.array(self.historical_data)
        
        # Normalize the features
        normalized_features = self.scaler.fit_transform(feature_matrix)
        
        # Train anomaly detector
        self.anomaly_detector.fit(normalized_features)
        
        self.is_trained = True
        print(f"Model trained on {len(self.historical_data)} data points")


def main():
    """
    Main function for testing the rollover prediction module.
    """
    print("Testing Rollover Prediction Module...")
    
    predictor = RolloverPredictor()
    
    # Simulate some test data
    test_accel_data = (0.5, 0.2, 9.8)  # Normal state
    result = predictor.predict_rollover_risk(test_accel_data)
    print(f"Normal state: {result}")
    
    test_accel_data = (2.0, 4.0, 8.0)  # High risk state
    result = predictor.predict_rollover_risk(test_accel_data)
    print(f"High risk state: {result}")


if __name__ == "__main__":
    main()