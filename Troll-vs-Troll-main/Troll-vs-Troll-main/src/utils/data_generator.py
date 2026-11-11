"""
Troll-vs-Troll Project
Sensor Data Generator Module

This module generates realistic sensor data for training and testing
the machine learning models. The data simulates real-world scenarios
for pull-handle carriers including normal movement, turns, and rollover risks.

Version: 1.3.2
"""

import time
import math
import random
import numpy as np
from datetime import datetime, timedelta


class SensorDataGenerator:
    """
    Generates realistic sensor data for pull-handle carriers.
    Simulates accelerometer and gyroscope data under various conditions.
    """
    
    def __init__(self, seed=None):
        """
        Initialize the data generator.
        
        Args:
            seed (int, optional): Random seed for reproducible data
        """
        # TODO: Implement realistic sensor data generation - HIGH - Developer
        # TODO: Simulate various movement scenarios (normal, turning, risky) - HIGH - Developer
        # TODO: Add noise and drift to make data more realistic - MEDIUM - Developer
        
        if seed:
            random.seed(seed)
            np.random.seed(seed)
        
        self.start_time = time.time()
        self.scenario = "normal"  # normal, turning, risky, rollover_imminent
        self.time_in_scenario = 0
        self.scenario_duration = 10.0  # seconds
        self.accel_bias = [0.0, 0.0, 0.0]  # Bias in accelerometer readings
        self.gyro_bias = [0.0, 0.0, 0.0]  # Bias in gyroscope readings
        
        print("SensorDataGenerator initialized")

    def set_scenario(self, scenario):
        """
        Set the current movement scenario.
        
        Args:
            scenario (str): One of "normal", "turning", "risky", "rollover_imminent"
        """
        if scenario in ["normal", "turning", "risky", "rollover_imminent"]:
            self.scenario = scenario
            self.time_in_scenario = 0
        else:
            raise ValueError(f"Invalid scenario: {scenario}")

    def generate_accel_data(self, timestamp=None):
        """
        Generate realistic accelerometer data based on current scenario.
        
        Args:
            timestamp (float, optional): Timestamp for the data point
            
        Returns:
            tuple: (x, y, z) acceleration values in m/s^2
        """
        if timestamp is None:
            timestamp = time.time()
        
        # Base acceleration (gravity + movement)
        base_x, base_y, base_z = self._get_base_acceleration()
        
        # Add scenario-specific effects
        scenario_x, scenario_y, scenario_z = self._get_scenario_acceleration()
        
        # Add noise to make it more realistic
        noise_x = np.random.normal(0, 0.05)  # 50 mg noise
        noise_y = np.random.normal(0, 0.05)
        noise_z = np.random.normal(0, 0.02)  # Less noise on Z (gravity axis)
        
        # Add bias (sensor imperfection)
        total_x = base_x + scenario_x + noise_x + self.accel_bias[0]
        total_y = base_y + scenario_y + noise_y + self.accel_bias[1]
        total_z = base_z + scenario_z + noise_z + self.accel_bias[2]
        
        # Update scenario time
        self.time_in_scenario += 0.01  # Assuming 100Hz sampling rate
        
        return (total_x, total_y, total_z)

    def generate_gyro_data(self, timestamp=None):
        """
        Generate realistic gyroscope data based on current scenario.
        
        Args:
            timestamp (float, optional): Timestamp for the data point
            
        Returns:
            tuple: (x, y, z) angular velocity values in rad/s
        """
        if timestamp is None:
            timestamp = time.time()
        
        # Base angular velocity (usually close to 0 in normal conditions)
        base_x, base_y, base_z = self._get_base_angular_velocity()
        
        # Add scenario-specific effects
        scenario_x, scenario_y, scenario_z = self._get_scenario_angular_velocity()
        
        # Add noise
        noise_x = np.random.normal(0, 0.005)
        noise_y = np.random.normal(0, 0.005)
        noise_z = np.random.normal(0, 0.005)
        
        # Add bias
        total_x = base_x + scenario_x + noise_x + self.gyro_bias[0]
        total_y = base_y + scenario_y + noise_y + self.gyro_bias[1]
        total_z = base_z + scenario_z + noise_z + self.gyro_bias[2]
        
        return (total_x, total_y, total_z)

    def _get_base_acceleration(self):
        """
        Get base acceleration values (gravity + normal movement).
        
        Returns:
            tuple: Base (x, y, z) acceleration values
        """
        # Base acceleration is mostly gravity (9.81 m/s^2) on Z axis
        # With small variations due to normal movement
        base_x = np.random.normal(0, 0.1)  # Small variations in x
        base_y = np.random.normal(0, 0.1)  # Small variations in y
        base_z = 9.8 + np.random.normal(0, 0.05)  # Gravity with small variations
        
        return (base_x, base_y, base_z)

    def _get_scenario_acceleration(self):
        """
        Get acceleration values specific to the current scenario.
        
        Returns:
            tuple: Scenario-specific (x, y, z) acceleration values
        """
        if self.scenario == "normal":
            # Normal walking/rolling - minimal extra acceleration
            x = np.random.normal(0, 0.2)
            y = np.random.normal(0, 0.2)
            z = np.random.normal(0, 0.1)
        elif self.scenario == "turning":
            # Turning - more lateral acceleration
            x = np.random.normal(0, 0.5)  # More variation in x for turns
            y = np.random.normal(0, 0.8)  # More variation in y for turns
            z = np.random.normal(0, 0.2)  # Slight variation in z
        elif self.scenario == "risky":
            # Risky movement - higher accelerations
            x = np.random.normal(0, 1.0)  # Higher variation
            y = np.random.normal(0, 1.0)
            z = np.random.normal(0, 0.5)
        elif self.scenario == "rollover_imminent":
            # About to rollover - high accelerations and tilting
            x = np.random.normal(0, 2.0)  # Very high variation
            y = np.random.normal(0, 2.0)
            z = np.random.normal(0, 1.0)  # Large variations in z due to tilting
        else:
            x = y = z = 0.0
        
        return (x, y, z)

    def _get_base_angular_velocity(self):
        """
        Get base angular velocity values.
        
        Returns:
            tuple: Base (x, y, z) angular velocity values
        """
        # Base angular velocity is usually close to 0
        return (0.0, 0.0, 0.0)

    def _get_scenario_angular_velocity(self):
        """
        Get angular velocity values specific to the current scenario.
        
        Returns:
            tuple: Scenario-specific (x, y, z) angular velocity values
        """
        if self.scenario == "normal":
            # Normal - very small angular velocities
            x = np.random.normal(0, 0.01)
            y = np.random.normal(0, 0.01)
            z = np.random.normal(0, 0.01)
        elif self.scenario == "turning":
            # Turning - higher angular velocity around Z axis
            x = np.random.normal(0, 0.05)
            y = np.random.normal(0, 0.05)
            z = np.random.normal(0, 0.2)  # Turning around vertical axis
        elif self.scenario == "risky":
            # Risky - more rotation
            x = np.random.normal(0, 0.1)
            y = np.random.normal(0, 0.1)
            z = np.random.normal(0, 0.3)
        elif self.scenario == "rollover_imminent":
            # About to rollover - high rotation rates
            x = np.random.normal(0, 0.3)  # High rotation around x (roll)
            y = np.random.normal(0, 0.3)  # High rotation around y (pitch)
            z = np.random.normal(0, 0.2)  # Some rotation around z
        else:
            x = y = z = 0.0
        
        return (x, y, z)

    def generate_data_sequence(self, duration, scenario="normal", sample_rate=100):
        """
        Generate a sequence of sensor data over a specified duration.
        
        Args:
            duration (float): Duration in seconds
            scenario (str): Movement scenario
            sample_rate (int): Samples per second
            
        Returns:
            list: List of (timestamp, accel_data, gyro_data) tuples
        """
        data_sequence = []
        self.set_scenario(scenario)
        
        num_samples = int(duration * sample_rate)
        
        for i in range(num_samples):
            timestamp = time.time() + i / sample_rate
            accel_data = self.generate_accel_data(timestamp)
            gyro_data = self.generate_gyro_data(timestamp)
            
            data_sequence.append((timestamp, accel_data, gyro_data))
            
            # Update scenario time
            self.time_in_scenario += 1.0 / sample_rate
        
        return data_sequence

    def generate_training_dataset(self, num_samples=1000):
        """
        Generate a balanced training dataset with different scenarios.
        
        Args:
            num_samples (int): Total number of samples to generate
            
        Returns:
            list: List of (accel_data, gyro_data, label) tuples
                  where label is 0=normal, 1=turning, 2=risky, 3=rollover_imminent
        """
        dataset = []
        
        # Define distribution of scenarios in training data
        scenario_distribution = {
            "normal": 0.4,      # 40% normal
            "turning": 0.3,     # 30% turning
            "risky": 0.2,       # 20% risky
            "rollover_imminent": 0.1  # 10% rollover imminent
        }
        
        samples_per_scenario = {
            scenario: int(num_samples * ratio)
            for scenario, ratio in scenario_distribution.items()
        }
        
        # Adjust for rounding errors
        total_allocated = sum(samples_per_scenario.values())
        if total_allocated < num_samples:
            samples_per_scenario["normal"] += num_samples - total_allocated
        
        for scenario, count in samples_per_scenario.items():
            self.set_scenario(scenario)
            label = ["normal", "turning", "risky", "rollover_imminent"].index(scenario)
            
            for _ in range(count):
                accel_data = self.generate_accel_data()
                gyro_data = self.generate_gyro_data()
                dataset.append((accel_data, gyro_data, label))
                
                # Slightly change scenario time for variation
                self.time_in_scenario += 0.01
        
        # Shuffle the dataset
        random.shuffle(dataset)
        
        return dataset


def main():
    """
    Main function for testing the data generator.
    """
    print("Testing Sensor Data Generator...")
    
    generator = SensorDataGenerator(seed=42)
    
    # Generate single data points for different scenarios
    scenarios = ["normal", "turning", "risky", "rollover_imminent"]
    
    for scenario in scenarios:
        generator.set_scenario(scenario)
        accel_data = generator.generate_accel_data()
        gyro_data = generator.generate_gyro_data()
        
        print(f"Scenario: {scenario}")
        print(f"  Acceleration: ({accel_data[0]:.3f}, {accel_data[1]:.3f}, {accel_data[2]:.3f}) m/s²")
        print(f"  Gyroscope: ({gyro_data[0]:.3f}, {gyro_data[1]:.3f}, {gyro_data[2]:.3f}) rad/s")
        print()
    
    # Generate a short sequence
    print("Generating 2-second sequence for 'risky' scenario:")
    sequence = generator.generate_data_sequence(duration=2.0, scenario="risky", sample_rate=10)
    print(f"Generated {len(sequence)} data points")
    
    # Show first few points
    for i, (timestamp, accel, gyro) in enumerate(sequence[:5]):
        print(f"  {i+1}: Accel=({accel[0]:.3f}, {accel[1]:.3f}, {accel[2]:.3f}), "
              f"Gyro=({gyro[0]:.3f}, {gyro[1]:.3f}, {gyro[2]:.3f})")
    
    print("\nData generation test completed.")


if __name__ == "__main__":
    main()