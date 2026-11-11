"""
Troll-vs-Troll Project
Feedback Control System with Adaptive Learning

This module implements a closed-loop control system that:
1. Executes control actions based on sensor data
2. Analyzes control effectiveness 
3. Records control outcomes for machine learning
4. Adapts control strategies based on learned patterns

## 版本日志
- v1.0.0 2026-02-18: 初始版本 - 测试中

Version: 1.0.0
"""

import time
import json
import numpy as np
from collections import deque
from datetime import datetime
from .differential_controller import DifferentialController
from ..ml.rollover_prediction import RolloverPredictor
from ..sensors.data_processor import SensorDataProcessor

class FeedbackController:
    """
    Closed-loop controller with adaptive learning capabilities.
    Implements the complete cycle: Sense → Analyze → Control → Evaluate → Learn
    """
    
    def __init__(self, bag_length=0.5, bag_width=0.3, bag_height=0.7, center_of_gravity_height=0.4, 
                 wheel_radius=0.1, wheel_mass=0.5, bag_total_mass=5.0):
        """
        Initialize the feedback control system.
        
        Args:
            Physical parameters for the bag system
        """
        # Core components
        self.differential_controller = DifferentialController(
            bag_length, bag_width, bag_height, center_of_gravity_height,
            wheel_radius, wheel_mass, bag_total_mass
        )
        self.rollover_predictor = RolloverPredictor(
            bag_length, bag_width, bag_height, center_of_gravity_height,
            wheel_radius, wheel_mass, bag_total_mass
        )
        self.sensor_processor = SensorDataProcessor()
        
        # Control history and learning
        self.control_history = deque(maxlen=1000)  # Store recent control episodes
        self.learning_buffer = []  # Buffer for training data
        self.episode_start_time = None
        self.current_episode = None
        
        # Performance metrics
        self.performance_metrics = {
            'total_episodes': 0,
            'successful_controls': 0,
            'false_positives': 0,
            'false_negatives': 0,
            'average_response_time': 0.0,
            'control_accuracy': 0.0
        }
        
        # Adaptive parameters
        self.adaptation_enabled = True
        self.learning_rate = 0.01
        self.exploration_rate = 0.1  # For reinforcement learning
        
        print("FeedbackController initialized with adaptive learning capabilities")
    
    def start_control_episode(self, initial_state):
        """
        Start a new control episode for tracking and analysis.
        
        Args:
            initial_state (dict): Initial sensor state
        """
        self.episode_start_time = time.time()
        self.current_episode = {
            'start_time': self.episode_start_time,
            'initial_state': initial_state,
            'control_actions': [],
            'sensor_readings': [],
            'outcomes': [],
            'episode_id': len(self.control_history)
        }
        
    def execute_control_action(self, accel_data, gyro_data=None):
        """
        Execute control action and record the process.
        
        Args:
            accel_data (tuple): (x, y, z) acceleration values
            gyro_data (tuple, optional): (x, y, z) gyroscope values
            
        Returns:
            dict: Control result with effectiveness metrics
        """
        if self.current_episode is None:
            self.start_control_episode({'accel': accel_data, 'gyro': gyro_data})
        
        # Record sensor input
        sensor_reading = {
            'timestamp': time.time(),
            'accel_data': accel_data,
            'gyro_data': gyro_data,
            'processed_features': self.sensor_processor.get_processed_features()
        }
        self.current_episode['sensor_readings'].append(sensor_reading)
        
        # Execute control
        control_result = self.differential_controller.update_control(accel_data, gyro_data)
        
        # Record control action
        action_record = {
            'timestamp': time.time(),
            'left_wheel_speed': control_result['left_wheel_speed'],
            'right_wheel_speed': control_result['right_wheel_speed'],
            'control_active': control_result['control_active'],
            'risk_assessment': control_result.get('risk_assessment', {})
        }
        self.current_episode['control_actions'].append(action_record)
        
        return control_result
    
    def evaluate_control_effectiveness(self, post_control_state):
        """
        Evaluate how effective the recent control actions were.
        
        Args:
            post_control_state (dict): State after control execution
            
        Returns:
            dict: Effectiveness metrics
        """
        if not self.current_episode or len(self.current_episode['control_actions']) == 0:
            return {'effectiveness': 0.0, 'confidence': 0.0}
        
        # Calculate control effectiveness metrics
        latest_action = self.current_episode['control_actions'][-1]
        initial_state = self.current_episode['initial_state']
        
        # Risk reduction metric
        initial_risk = initial_state.get('risk_score', 0.5)
        current_risk = post_control_state.get('risk_score', initial_risk)
        risk_reduction = max(0, initial_risk - current_risk)
        
        # Stability improvement
        initial_stability = self._calculate_stability(initial_state)
        current_stability = self._calculate_stability(post_control_state)
        stability_improvement = current_stability - initial_stability
        
        # Response time
        response_time = time.time() - self.current_episode['start_time']
        
        # Overall effectiveness score (0-1)
        effectiveness = (
            0.4 * risk_reduction +           # 40% weight on risk reduction
            0.3 * max(0, stability_improvement) +  # 30% weight on stability
            0.2 * max(0, 1 - response_time/2.0) +  # 20% weight on response time
            0.1 * (1 - abs(initial_risk - 0.5))    # 10% weight on appropriate activation
        )
        
        evaluation = {
            'effectiveness': float(effectiveness),
            'risk_reduction': float(risk_reduction),
            'stability_improvement': float(stability_improvement),
            'response_time': float(response_time),
            'control_active': latest_action['control_active'],
            'timestamp': time.time()
        }
        
        self.current_episode['outcomes'].append(evaluation)
        return evaluation
    
    def _calculate_stability(self, state):
        """
        Calculate stability metric from sensor state.
        
        Args:
            state (dict): Sensor state data
            
        Returns:
            float: Stability score (higher is more stable)
        """
        # Extract relevant features
        accel_magnitude = state.get('accel_magnitude', 9.8)
        roll_angle = abs(state.get('roll_angle', 0))
        pitch_angle = abs(state.get('pitch_angle', 0))
        
        # Stability decreases with larger angles and acceleration variations
        stability = 1.0 - (roll_angle/45.0 + pitch_angle/30.0 + abs(accel_magnitude - 9.8)/19.6)
        return max(0, stability)  # Ensure non-negative
    
    def record_learning_sample(self, control_context, outcome):
        """
        Record a sample for machine learning training.
        
        Args:
            control_context (dict): Context when control was applied
            outcome (dict): Control outcome evaluation
        """
        sample = {
            'context': control_context,
            'outcome': outcome,
            'timestamp': time.time(),
            'episode_id': self.current_episode['episode_id'] if self.current_episode else -1
        }
        
        self.learning_buffer.append(sample)
        
        # Periodically save to persistent storage
        if len(self.learning_buffer) >= 50:
            self._save_learning_data()
    
    def _save_learning_data(self):
        """Save accumulated learning data to file."""
        try:
            filename = f"control_learning_data_{datetime.now().strftime('%Y%m%d')}.json"
            with open(filename, 'a', encoding='utf-8') as f:
                for sample in self.learning_buffer:
                    json.dump(sample, f, ensure_ascii=False)
                    f.write('\n')
            
            self.learning_buffer.clear()
            print(f"Saved {len(self.learning_buffer)} learning samples")
            
        except Exception as e:
            print(f"Error saving learning data: {e}")
    
    def adapt_control_strategy(self):
        """
        Adapt control strategy based on learning history.
        Implements simple reinforcement learning.
        """
        if not self.adaptation_enabled or len(self.control_history) < 10:
            return
        
        # Analyze recent performance
        recent_episodes = list(self.control_history)[-20:]  # Last 20 episodes
        avg_effectiveness = np.mean([ep.get('avg_effectiveness', 0) 
                                   for ep in recent_episodes if 'avg_effectiveness' in ep])
        
        # Adjust control parameters based on performance
        if avg_effectiveness < 0.6:  # Poor performance
            # Increase sensitivity
            self.differential_controller.control_threshold *= 0.95
            self.exploration_rate = min(0.3, self.exploration_rate * 1.1)
        elif avg_effectiveness > 0.8:  # Good performance
            # Reduce sensitivity to avoid over-control
            self.differential_controller.control_threshold *= 1.05
            self.exploration_rate = max(0.05, self.exploration_rate * 0.9)
        
        print(f"Adapted control strategy - Threshold: {self.differential_controller.control_threshold:.3f}, "
              f"Exploration: {self.exploration_rate:.3f}")
    
    def end_control_episode(self, final_state):
        """
        End current control episode and store results.
        
        Args:
            final_state (dict): Final state of the episode
        """
        if not self.current_episode:
            return
        
        # Calculate episode summary
        avg_effectiveness = np.mean([outcome['effectiveness'] 
                                   for outcome in self.current_episode['outcomes']]) if self.current_episode['outcomes'] else 0
        
        self.current_episode['final_state'] = final_state
        self.current_episode['avg_effectiveness'] = float(avg_effectiveness)
        self.current_episode['duration'] = time.time() - self.episode_start_time
        
        # Update performance metrics
        self.performance_metrics['total_episodes'] += 1
        if avg_effectiveness > 0.7:
            self.performance_metrics['successful_controls'] += 1
        
        # Store episode
        self.control_history.append(self.current_episode)
        
        # Trigger adaptation
        self.adapt_control_strategy()
        
        # Reset for next episode
        self.current_episode = None
        self.episode_start_time = None
    
    def get_performance_report(self):
        """
        Get current performance report.
        
        Returns:
            dict: Performance metrics and statistics
        """
        if self.performance_metrics['total_episodes'] > 0:
            self.performance_metrics['control_accuracy'] = (
                self.performance_metrics['successful_controls'] / 
                self.performance_metrics['total_episodes']
            )
        
        return {
            'metrics': self.performance_metrics,
            'recent_episodes': len(self.control_history),
            'learning_samples': len(self.learning_buffer),
            'current_status': 'Active' if self.current_episode else 'Idle'
        }
    
    def export_training_data(self, filename='feedback_training_data.json'):
        """
        Export all collected training data for external ML training.
        
        Args:
            filename (str): Output filename
        """
        training_data = []
        
        # Include control history
        for episode in self.control_history:
            if episode.get('outcomes'):
                for i, outcome in enumerate(episode['outcomes']):
                    if i < len(episode['control_actions']):
                        sample = {
                            'features': episode['control_actions'][i],
                            'label': 1 if outcome['effectiveness'] > 0.7 else 0,
                            'effectiveness': outcome['effectiveness'],
                            'timestamp': outcome['timestamp']
                        }
                        training_data.append(sample)
        
        # Include buffered samples
        training_data.extend(self.learning_buffer)
        
        try:
            with open(filename, 'w', encoding='utf-8') as f:
                json.dump(training_data, f, indent=2, ensure_ascii=False)
            print(f"Exported {len(training_data)} training samples to {filename}")
            return len(training_data)
        except Exception as e:
            print(f"Error exporting training data: {e}")
            return 0

# Test function
def main():
    """Test the feedback controller."""
    print("Testing Feedback Controller...")
    
    controller = FeedbackController()
    
    # Simulate a control episode
    test_data = [
        ((0.1, 0.05, 9.81), None),  # Normal state
        ((0.5, 1.0, 9.2), None),    # Mild risk
        ((1.0, 2.5, 8.0), None),    # High risk
        ((0.2, 0.1, 9.7), None)     # Recovery
    ]
    
    controller.start_control_episode({'accel': test_data[0][0], 'risk_score': 0.1})
    
    for accel_data, gyro_data in test_data:
        result = controller.execute_control_action(accel_data, gyro_data)
        print(f"Control result: Active={result['control_active']}, "
              f"L={result['left_wheel_speed']:.3f}, R={result['right_wheel_speed']:.3f}")
        
        # Simulate outcome evaluation
        outcome = controller.evaluate_control_effectiveness({
            'risk_score': 0.3,
            'accel_magnitude': np.linalg.norm(accel_data),
            'roll_angle': 5.0
        })
        print(f"Effectiveness: {outcome['effectiveness']:.3f}")
    
    controller.end_control_episode({'final_risk': 0.1})
    
    report = controller.get_performance_report()
    print(f"Performance Report: {report}")

if __name__ == "__main__":
    main()