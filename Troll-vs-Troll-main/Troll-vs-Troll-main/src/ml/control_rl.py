"""
Reinforcement Learning Module for Troll-vs-Troll Control Optimization

Implements Q-learning and policy gradient methods to optimize 
the differential control strategy based on control effectiveness feedback.

## 版本日志
- v1.0.0 2026-02-18: 初始版本 - 测试中

Version: 1.0.0
"""

import numpy as np
import random
from collections import defaultdict
import json
import time
from datetime import datetime

class ControlRLAgent:
    """
    Reinforcement Learning agent for optimizing control policies.
    Learns optimal control actions based on state observations and rewards.
    """
    
    def __init__(self, state_dim=8, action_dim=3, learning_rate=0.1, discount_factor=0.95, epsilon=0.1):
        """
        Initialize RL agent.
        
        Args:
            state_dim (int): Dimension of state space
            action_dim (int): Number of discrete actions
            learning_rate (float): Q-learning update rate
            discount_factor (float): Future reward discount
            epsilon (float): Exploration rate
        """
        self.state_dim = state_dim
        self.action_dim = action_dim
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.epsilon = epsilon
        
        # Q-table: state-action value function
        self.q_table = defaultdict(lambda: np.zeros(action_dim))
        
        # Experience replay buffer
        self.replay_buffer = []
        self.buffer_size = 10000
        
        # Training statistics
        self.training_stats = {
            'episodes': 0,
            'total_rewards': 0,
            'exploration_rate': epsilon,
            'convergence_metric': 0.0
        }
        
        print(f"ControlRLAgent initialized with {state_dim} state dims, {action_dim} actions")
    
    def discretize_state(self, continuous_state):
        """
        Convert continuous state to discrete representation for Q-learning.
        
        Args:
            continuous_state (dict): Raw sensor data
            
        Returns:
            tuple: Discretized state representation
        """
        # Extract key features
        features = [
            continuous_state.get('risk_score', 0),
            continuous_state.get('roll_angle', 0) / 45.0,  # Normalize to [-1, 1]
            continuous_state.get('pitch_angle', 0) / 30.0,
            continuous_state.get('accel_magnitude', 9.8) / 19.6,  # Relative to gravity
            continuous_state.get('gyro_magnitude', 0) / 10.0,
            1.0 if continuous_state.get('control_active', False) else 0.0,
            continuous_state.get('wheel_speed_diff', 0) / 2.0,
            time.time() % 60 / 60.0  # Time of day feature
        ]
        
        # Discretize to reduce state space
        discretized = tuple(int(f * 10) for f in features)  # 10 bins per feature
        return discretized
    
    def select_action(self, state, training=True):
        """
        Select action using epsilon-greedy policy.
        
        Args:
            state (dict): Current state
            training (bool): Whether in training mode
            
        Returns:
            int: Selected action index
        """
        discrete_state = self.discretize_state(state)
        
        if training and random.random() < self.epsilon:
            # Exploration
            return random.randint(0, self.action_dim - 1)
        else:
            # Exploitation
            return np.argmax(self.q_table[discrete_state])
    
    def get_action_details(self, action_index):
        """
        Convert discrete action index to actual control parameters.
        
        Args:
            action_index (int): Discrete action
            
        Returns:
            dict: Control action parameters
        """
        actions = [
            {'type': 'conservative', 'diff_factor': 0.5, 'base_speed': 1.0},
            {'type': 'moderate', 'diff_factor': 1.0, 'base_speed': 1.0},
            {'type': 'aggressive', 'diff_factor': 1.5, 'base_speed': 1.0}
        ]
        
        return actions[min(action_index, len(actions) - 1)]
    
    def update_q_value(self, state, action, reward, next_state, done):
        """
        Update Q-value using Q-learning update rule.
        
        Args:
            state (dict): Previous state
            action (int): Action taken
            reward (float): Reward received
            next_state (dict): Resulting state
            done (bool): Episode termination flag
        """
        current_state = self.discretize_state(state)
        next_discrete_state = self.discretize_state(next_state)
        
        # Q-learning update
        current_q = self.q_table[current_state][action]
        
        if done:
            target_q = reward
        else:
            target_q = reward + self.discount_factor * np.max(self.q_table[next_discrete_state])
        
        # Update Q-value
        self.q_table[current_state][action] += self.learning_rate * (target_q - current_q)
        
        # Store experience for replay
        experience = (state, action, reward, next_state, done)
        self.store_experience(experience)
    
    def store_experience(self, experience):
        """Store experience in replay buffer."""
        self.replay_buffer.append(experience)
        if len(self.replay_buffer) > self.buffer_size:
            self.replay_buffer.pop(0)  # Remove oldest experience
    
    def compute_reward(self, state, action, next_state, control_effectiveness):
        """
        Compute reward based on control outcome.
        
        Args:
            state (dict): Previous state
            action (int): Action taken
            next_state (dict): Resulting state
            control_effectiveness (float): Effectiveness score [0,1]
            
        Returns:
            float: Computed reward
        """
        # Base reward from effectiveness
        reward = control_effectiveness * 2.0 - 1.0  # Scale to [-1, 1]
        
        # Penalize excessive control activity
        if next_state.get('control_active', False):
            reward -= 0.1
            
        # Reward stability maintenance
        if (state.get('risk_score', 0.5) < 0.3 and 
            next_state.get('risk_score', 0.5) < 0.3):
            reward += 0.2
            
        # Penalize high risk states
        if next_state.get('risk_score', 0) > 0.8:
            reward -= 0.5
            
        # Encourage appropriate action selection
        risk_level = state.get('risk_score', 0)
        if risk_level > 0.7 and action == 2:  # Aggressive action for high risk
            reward += 0.3
        elif risk_level < 0.3 and action == 0:  # Conservative action for low risk
            reward += 0.2
            
        return max(-1.0, min(1.0, reward))  # Clamp reward
    
    def train_from_episodes(self, episodes_data):
        """
        Train agent from collected episode data.
        
        Args:
            episodes_data (list): List of episode dictionaries
        """
        print(f"Training RL agent on {len(episodes_data)} episodes...")
        
        total_reward = 0
        updates = 0
        
        for episode in episodes_data:
            if 'sensor_readings' not in episode or 'control_actions' not in episode:
                continue
                
            # Process each step in episode
            for i in range(len(episode['control_actions']) - 1):
                # Get state transition
                current_state = episode['sensor_readings'][i]
                next_state = episode['sensor_readings'][i + 1]
                action_taken = episode['control_actions'][i]
                outcome = episode['outcomes'][i] if i < len(episode['outcomes']) else {}
                
                # Compute reward
                effectiveness = outcome.get('effectiveness', 0.5)
                reward = self.compute_reward(
                    current_state, 
                    action_taken.get('action_index', 1), 
                    next_state, 
                    effectiveness
                )
                
                # Update Q-value
                self.update_q_value(
                    current_state, 
                    action_taken.get('action_index', 1),
                    reward, 
                    next_state, 
                    i == len(episode['control_actions']) - 2  # Done flag
                )
                
                total_reward += reward
                updates += 1
        
        # Update training statistics
        self.training_stats['episodes'] += len(episodes_data)
        self.training_stats['total_rewards'] += total_reward
        
        # Decay exploration rate
        self.epsilon = max(0.01, self.epsilon * 0.995)
        self.training_stats['exploration_rate'] = self.epsilon
        
        avg_reward = total_reward / max(1, updates)
        print(f"Training completed. Avg reward: {avg_reward:.3f}, "
              f"Epsilon: {self.epsilon:.3f}")
        
        return avg_reward
    
    def get_policy(self, state):
        """
        Get recommended action for given state.
        
        Args:
            state (dict): Current state
            
        Returns:
            dict: Recommended action with confidence
        """
        discrete_state = self.discretize_state(state)
        action_values = self.q_table[discrete_state]
        best_action = np.argmax(action_values)
        confidence = float(np.max(action_values))
        
        return {
            'action_index': int(best_action),
            'action_details': self.get_action_details(best_action),
            'confidence': confidence,
            'action_values': action_values.tolist()
        }
    
    def save_model(self, filepath):
        """Save trained model to file."""
        model_data = {
            'q_table': {str(k): v.tolist() for k, v in self.q_table.items()},
            'hyperparameters': {
                'state_dim': self.state_dim,
                'action_dim': self.action_dim,
                'learning_rate': self.learning_rate,
                'discount_factor': self.discount_factor,
                'epsilon': self.epsilon
            },
            'training_stats': self.training_stats,
            'timestamp': datetime.now().isoformat()
        }
        
        try:
            with open(filepath, 'w') as f:
                json.dump(model_data, f, indent=2)
            print(f"Model saved to {filepath}")
            return True
        except Exception as e:
            print(f"Error saving model: {e}")
            return False
    
    def load_model(self, filepath):
        """Load trained model from file."""
        try:
            with open(filepath, 'r') as f:
                model_data = json.load(f)
            
            # Restore Q-table
            self.q_table = defaultdict(lambda: np.zeros(self.action_dim))
            for state_str, values in model_data['q_table'].items():
                state_tuple = tuple(eval(state_str))  # Convert string back to tuple
                self.q_table[state_tuple] = np.array(values)
            
            # Restore hyperparameters
            params = model_data['hyperparameters']
            self.learning_rate = params['learning_rate']
            self.discount_factor = params['discount_factor']
            self.epsilon = params['epsilon']
            
            # Restore training stats
            self.training_stats = model_data['training_stats']
            
            print(f"Model loaded from {filepath}")
            return True
        except Exception as e:
            print(f"Error loading model: {e}")
            return False

# Test function
def main():
    """Test the RL agent."""
    print("Testing Control RL Agent...")
    
    agent = ControlRLAgent()
    
    # Simulate some training data
    test_episodes = []
    for episode_id in range(5):
        episode = {
            'sensor_readings': [],
            'control_actions': [],
            'outcomes': []
        }
        
        # Generate episode data
        for step in range(10):
            state = {
                'risk_score': random.random(),
                'roll_angle': random.uniform(-20, 20),
                'pitch_angle': random.uniform(-15, 15),
                'accel_magnitude': random.uniform(8, 12),
                'control_active': step > 5
            }
            
            action_idx = agent.select_action(state)
            action = agent.get_action_details(action_idx)
            
            episode['sensor_readings'].append(state)
            episode['control_actions'].append({
                'action_index': action_idx,
                'details': action
            })
            
            # Simulate outcome
            episode['outcomes'].append({
                'effectiveness': random.random()
            })
        
        test_episodes.append(episode)
    
    # Train agent
    agent.train_from_episodes(test_episodes)
    
    # Test policy
    test_state = {
        'risk_score': 0.8,
        'roll_angle': 15,
        'pitch_angle': 5,
        'accel_magnitude': 15.0,
        'control_active': False
    }
    
    policy = agent.get_policy(test_state)
    print(f"Recommended action for high-risk state: {policy}")

if __name__ == "__main__":
    main()