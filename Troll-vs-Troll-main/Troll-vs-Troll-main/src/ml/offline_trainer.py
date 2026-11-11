"""
Offline Machine Learning Trainer for Troll-vs-Troll System
离线机器学习训练器

Version: 1.3.2
"""

import numpy as np
import json
import os
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, classification_report
from sklearn.preprocessing import StandardScaler
import joblib
from datetime import datetime

class OfflineTrainer:
    """离线训练器 - 处理用户收集的训练数据"""
    
    def __init__(self, data_file='training_data.json', model_file='trained_model.pkl'):
        self.data_file = data_file
        self.model_file = model_file
        self.scaler_file = 'feature_scaler.pkl'
        self.config_file = 'model_config.json'
        
        # 特征工程相关
        self.feature_names = [
            'mean_accel_x', 'mean_accel_y', 'mean_accel_z',
            'std_accel_x', 'std_accel_y', 'std_accel_z',
            'mean_gyro_x', 'mean_gyro_y', 'mean_gyro_z',
            'std_gyro_x', 'std_gyro_y', 'std_gyro_z',
            'max_roll', 'max_pitch', 'duration',
            'accel_magnitude_var', 'gyro_magnitude_mean'
        ]
        
        # 训练参数
        self.model = None
        self.scaler = StandardScaler()
        self.training_history = []
        
    def load_training_data(self):
        """加载训练数据"""
        if not os.path.exists(self.data_file):
            raise FileNotFoundError(f"训练数据文件 {self.data_file} 不存在")
            
        with open(self.data_file, 'r', encoding='utf-8') as f:
            raw_data = json.load(f)
            
        return raw_data
    
    def extract_features(self, segment_data):
        """从数据片段中提取特征"""
        if not segment_data['data']:
            return None
            
        data_points = segment_data['data']
        
        # 提取数值数据
        accel_x = [point['accel_x'] for point in data_points]
        accel_y = [point['accel_y'] for point in data_points]
        accel_z = [point['accel_z'] for point in data_points]
        gyro_x = [point['gyro_x'] for point in data_points]
        gyro_y = [point['gyro_y'] for point in data_points]
        gyro_z = [point['gyro_z'] for point in data_points]
        roll_angles = [point['roll'] for point in data_points]
        pitch_angles = [point['pitch'] for point in data_points]
        
        # 计算统计特征
        features = [
            np.mean(accel_x), np.mean(accel_y), np.mean(accel_z),
            np.std(accel_x), np.std(accel_y), np.std(accel_z),
            np.mean(gyro_x), np.mean(gyro_y), np.mean(gyro_z),
            np.std(gyro_x), np.std(gyro_y), np.std(gyro_z),
            np.max(np.abs(roll_angles)), np.max(np.abs(pitch_angles)),
            segment_data['duration'],
            np.var(np.sqrt(np.array(accel_x)**2 + np.array(accel_y)**2 + np.array(accel_z)**2)),
            np.mean(np.sqrt(np.array(gyro_x)**2 + np.array(gyro_y)**2 + np.array(gyro_z)**2))
        ]
        
        return features
    
    def prepare_training_dataset(self):
        """准备训练数据集"""
        raw_data = self.load_training_data()
        
        X = []  # 特征
        y = []  # 标签
        
        for segment in raw_data:
            features = self.extract_features(segment)
            if features is not None:
                X.append(features)
                y.append(segment['label'])
        
        if len(X) == 0:
            raise ValueError("没有有效的训练数据")
            
        return np.array(X), np.array(y)
    
    def train_model(self, X, y):
        """训练随机森林分类器"""
        # 分割训练和测试集
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.2, random_state=42, stratify=y
        )
        
        # 特征标准化
        X_train_scaled = self.scaler.fit_transform(X_train)
        X_test_scaled = self.scaler.transform(X_test)
        
        # 训练随机森林
        self.model = RandomForestClassifier(
            n_estimators=100,
            max_depth=10,
            min_samples_split=5,
            min_samples_leaf=2,
            random_state=42
        )
        
        self.model.fit(X_train_scaled, y_train)
        
        # 评估模型
        y_pred = self.model.predict(X_test_scaled)
        accuracy = accuracy_score(y_test, y_pred)
        
        # 详细报告
        report = classification_report(y_test, y_pred, output_dict=True)
        
        return {
            'accuracy': accuracy,
            'classification_report': report,
            'feature_importance': dict(zip(self.feature_names, self.model.feature_importances_)),
            'train_samples': len(X_train),
            'test_samples': len(X_test)
        }
    
    def save_model(self, training_result):
        """保存训练好的模型和配置"""
        # 保存模型
        joblib.dump(self.model, self.model_file)
        joblib.dump(self.scaler, self.scaler_file)
        
        # 保存配置文件
        config = {
            'model_type': 'RandomForestClassifier',
            'feature_names': self.feature_names,
            'training_timestamp': datetime.now().isoformat(),
            'training_result': training_result,
            'model_parameters': {
                'n_estimators': 100,
                'max_depth': 10,
                'min_samples_split': 5,
                'min_samples_leaf': 2
            }
        }
        
        with open(self.config_file, 'w', encoding='utf-8') as f:
            json.dump(config, f, indent=2, ensure_ascii=False)
        
        # 保存训练历史
        self.training_history.append({
            'timestamp': datetime.now().isoformat(),
            'result': training_result
        })
        
        return config
    
    def train_from_collected_data(self):
        """完整的训练流程"""
        try:
            print("开始训练流程...")
            
            # 1. 准备数据
            print("1. 准备训练数据...")
            X, y = self.prepare_training_dataset()
            print(f"   成功加载 {len(X)} 个样本，其中正样本 {sum(y)} 个")
            
            # 2. 训练模型
            print("2. 训练模型...")
            training_result = self.train_model(X, y)
            print(f"   训练完成，准确率: {training_result['accuracy']:.3f}")
            
            # 3. 保存模型
            print("3. 保存模型和配置...")
            config = self.save_model(training_result)
            print("   模型保存成功")
            
            return {
                'status': 'success',
                'training_result': training_result,
                'config_saved': True,
                'samples_processed': len(X)
            }
            
        except Exception as e:
            return {
                'status': 'error',
                'message': str(e)
            }
    
    def load_trained_model(self):
        """加载已训练的模型"""
        try:
            if not os.path.exists(self.model_file):
                raise FileNotFoundError("未找到训练好的模型文件")
                
            self.model = joblib.load(self.model_file)
            self.scaler = joblib.load(self.scaler_file)
            
            # 加载配置
            if os.path.exists(self.config_file):
                with open(self.config_file, 'r', encoding='utf-8') as f:
                    config = json.load(f)
                return config
            else:
                return {'status': 'loaded', 'message': '模型加载成功'}
                
        except Exception as e:
            raise Exception(f"模型加载失败: {str(e)}")
    
    def predict_risk(self, sensor_data_segment):
        """使用训练好的模型预测风险"""
        if self.model is None:
            raise Exception("模型未加载")
            
        # 提取特征
        features = self.extract_features({'data': sensor_data_segment, 'duration': len(sensor_data_segment)/10})
        
        if features is None:
            return 0.5  # 不确定情况返回中等风险
            
        # 标准化并预测
        features_scaled = self.scaler.transform([features])
        probability = self.model.predict_proba(features_scaled)[0][1]  # 预测为正类的概率
        
        return float(probability)
    
    def get_training_statistics(self):
        """获取训练统计数据"""
        try:
            if os.path.exists(self.data_file):
                with open(self.data_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                
                positive_count = sum(1 for segment in data if segment['label'] == 1)
                negative_count = len(data) - positive_count
                
                return {
                    'total_segments': len(data),
                    'positive_samples': positive_count,
                    'negative_samples': negative_count,
                    'last_training': self._get_last_training_time()
                }
            else:
                return {
                    'total_segments': 0,
                    'positive_samples': 0,
                    'negative_samples': 0,
                    'last_training': '从未训练'
                }
        except Exception as e:
            return {'error': str(e)}
    
    def _get_last_training_time(self):
        """获取最后训练时间"""
        if os.path.exists(self.config_file):
            mtime = os.path.getmtime(self.config_file)
            return datetime.fromtimestamp(mtime).strftime('%Y-%m-%d %H:%M:%S')
        return '从未训练'

# 测试代码
if __name__ == "__main__":
    trainer = OfflineTrainer()
    
    # 如果有训练数据，进行训练
    try:
        result = trainer.train_from_collected_data()
        print("训练结果:", result)
    except FileNotFoundError:
        print("未找到训练数据文件，请先在学习模式中收集数据")
    except Exception as e:
        print(f"训练出错: {e}")