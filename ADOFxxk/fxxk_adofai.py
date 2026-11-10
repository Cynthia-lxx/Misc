import json
import random
import math

class ADOFAIFxxk:
    def __init__(self, file_path):
        self.file_path = file_path
        self.data = None
        self.original_content = None
        
    def read_file(self):
        """读取ADOFAI文件内容"""
        with open(self.file_path, 'r', encoding='utf-8-sig') as file:
            self.original_content = file.read()
        return self.original_content
    
    def parse(self):
        """解析ADOFAI文件"""
        if self.original_content is None:
            self.read_file()
            
        # 清理内容以符合JSON格式
        cleaned_content = self._clean_content(self.original_content)
        
        try:
            # 尝试使用标准JSON解析
            self.data = json.loads(cleaned_content)
        except json.JSONDecodeError:
            raise ValueError("无法解析文件内容")
            
        return self.data
    
    def _clean_content(self, content):
        """清理内容以符合JSON格式"""
        # 移除尾随逗号
        content = content.replace(',]', ']').replace(',}', '}')
        return content
    
    def calculate_max_deviation(self, bpm, max_ms_deviation=20):
        """
        计算最大角度偏差
        Args:
            bpm: 每分钟节拍数
            max_ms_deviation: 最大毫秒偏差，默认20ms
        Returns:
            最大角度偏差
        """
        # 1拍 = 360度 = 60000ms/BPM
        # 1ms = 360 * BPM / 60000 度
        degrees_per_ms = 360 * bpm / 60000
        return degrees_per_ms * max_ms_deviation
    
    def fxxk_level(self, output_path, max_ms_deviation=20, seed=None):
        """
        扭曲关卡，使轨道产生随机偏移但保持在指定范围内
        Args:
            output_path: 输出文件路径
            max_ms_deviation: 最大毫秒偏差，默认20ms
            seed: 随机种子，用于可重现的结果
        """
        if seed is not None:
            random.seed(seed)
            
        if self.data is None:
            self.parse()
            
        # 获取原始angleData
        original_angle_data = self.data.get("angleData", [])
        
        if not original_angle_data:
            raise ValueError("文件中没有找到angleData")
            
        # 获取BPM
        bpm = self.data["settings"].get("bpm", 100)
        
        # 计算最大角度偏差
        max_angle_deviation = self.calculate_max_deviation(bpm, max_ms_deviation)
        
        # 创建新的angleData，累积偏差不能超过限制
        new_angle_data = []
        cumulative_deviation = 0
        
        for i, angle in enumerate(original_angle_data):
            # 生成随机偏差，但要考虑累积偏差
            max_deviation_allowed = max_angle_deviation - abs(cumulative_deviation)
            if max_deviation_allowed <= 0:
                # 如果累积偏差已经达到极限，则只允许反向调整
                if cumulative_deviation > 0:
                    deviation = random.uniform(-max_angle_deviation, 0)
                else:
                    deviation = random.uniform(0, max_angle_deviation)
            else:
                # 在允许范围内生成随机偏差
                min_deviation = max(-max_angle_deviation, -max_deviation_allowed)
                max_deviation = min(max_angle_deviation, max_deviation_allowed)
                deviation = random.uniform(min_deviation, max_deviation)
            
            # 应用偏差
            new_angle = angle + deviation
            new_angle_data.append(round(new_angle, 6))  # 保留6位小数
            
            # 更新累积偏差
            cumulative_deviation += deviation
            
            # 确保累积偏差不超过限制
            if abs(cumulative_deviation) > max_angle_deviation:
                cumulative_deviation = max(-max_angle_deviation, min(max_angle_deviation, cumulative_deviation))
        
        # 更新数据
        self.data["angleData"] = new_angle_data
        
        # 保存到新文件
        self.save_to_file(output_path)
        
    def save_to_file(self, output_path):
        """
        保存修改后的数据到文件
        Args:
            output_path: 输出文件路径
        """
        if self.data is None:
            raise ValueError("没有数据可保存")
            
        # 使用原始文件格式保存
        with open(output_path, 'w', encoding='utf-8-sig') as file:
            json.dump(self.data, file, ensure_ascii=False, separators=(',', ': '))
            
        print(f"文件已保存到: {output_path}")

def main():
    # 使用示例
    parser = ADOFAIFxxk("level.adofai")
    parser.parse()
    
    # 扭曲关卡，最大偏差20ms
    parser.fxxk_level("level_fxxked.adofai", max_ms_deviation=20)
    
    print("关卡已成功扭曲！")

if __name__ == "__main__":
    main()