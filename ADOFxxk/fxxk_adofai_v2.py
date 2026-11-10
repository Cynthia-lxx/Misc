import json
import random
import re

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
        content = re.sub(r',(\s*[}\]])', r'\1', content)
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
    
    def get_bpm_changes(self):
        """
        获取所有变速事件
        Returns:
            列表，包含所有变速事件的信息
        """
        if self.data is None:
            self.parse()
            
        actions = self.data.get("actions", [])
        bpm_changes = []
        
        # 初始BPM
        current_bpm = self.data["settings"].get("bpm", 100)
        
        # 查找所有SetSpeed事件
        for action in actions:
            if action.get("eventType") == "SetSpeed":
                floor = action.get("floor", 0)
                speed_type = action.get("speedType")
                
                if speed_type == "Bpm":
                    # 直接设置BPM
                    new_bpm = action.get("beatsPerMinute", current_bpm)
                    current_bpm = new_bpm
                    bpm_changes.append({
                        "floor": floor,
                        "bpm": new_bpm,
                        "type": "Bpm"
                    })
                elif speed_type == "Multiplier":
                    # 通过倍数设置BPM
                    multiplier = action.get("bpmMultiplier", 1.0)
                    new_bpm = current_bpm * multiplier
                    current_bpm = new_bpm
                    bpm_changes.append({
                        "floor": floor,
                        "bpm": new_bpm,
                        "type": "Multiplier",
                        "multiplier": multiplier
                    })
                    
        return bpm_changes
    
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
            
        # 获取初始BPM
        initial_bpm = self.data["settings"].get("bpm", 100)
        
        # 获取所有变速事件
        bpm_changes = self.get_bpm_changes()
        
        # 创建新的angleData，累积偏差不能超过限制
        new_angle_data = []
        cumulative_deviation = 0
        current_bpm = initial_bpm
        bpm_change_index = 0
        
        for i, angle in enumerate(original_angle_data):
            # 检查是否有在这一格的变速事件
            while (bpm_change_index < len(bpm_changes) and 
                   bpm_changes[bpm_change_index]["floor"] == i):
                current_bpm = bpm_changes[bpm_change_index]["bpm"]
                bpm_change_index += 1
                
            # 计算当前BPM下的最大角度偏差
            max_angle_deviation = self.calculate_max_deviation(current_bpm, max_ms_deviation)
            
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
            
            # 确保累积偏差不超过当前BPM下的限制
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
            json.dump(self.data, file, ensure_ascii=False, separators=(',', ':'))
            
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