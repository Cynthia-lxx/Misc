import json
import re
import os

class ADOFAIParser:
    def __init__(self, file_path):
        """
        初始化ADOFAI解析器
        
        Args:
            file_path (str): ADOFAI文件路径
        """
        self.file_path = file_path
        self.data = None
        self.original_content = None
        
    def read_file(self):
        """
        读取ADOFAI文件内容，不修改原始文件
        
        Returns:
            str: 文件内容
        """
        with open(self.file_path, 'r', encoding='utf-8-sig') as file:
            content = file.read()
        self.original_content = content
        return content
    
    def parse(self):
        """
        解析ADOFAI文件内容为Python对象
        
        Returns:
            dict: 解析后的数据
        """
        if self.original_content is None:
            self.read_file()
            
        # 清理内容以符合JSON格式
        cleaned_content = self._clean_content(self.original_content)
        
        try:
            # 尝试使用标准JSON解析
            self.data = json.loads(cleaned_content)
        except json.JSONDecodeError:
            # 如果标准JSON解析失败，使用更宽松的方法
            self.data = self._parse_relaxed_json(cleaned_content)
            
        return self.data
    
    def _clean_content(self, content):
        """
        清理内容以符合JSON格式
        
        Args:
            content (str): 原始内容
            
        Returns:
            str: 清理后的内容
        """
        # 移除尾随逗号
        content = re.sub(r',(\s*[}\]])', r'\1', content)
        return content
    
    def _parse_relaxed_json(self, content):
        """
        使用更宽松的方法解析JSON
        
        Args:
            content (str): 要解析的内容
            
        Returns:
            dict: 解析后的数据
        """
        # 这里可以使用第三方库如json5或自己实现宽松解析
        # 为了简单起见，我们先尝试修复常见的问题
        try:
            import json5
            return json5.loads(content)
        except ImportError:
            # 如果没有json5库，手动修复一些常见问题
            # 移除注释（如果有的话）
            content = re.sub(r'//.*', '', content)
            # 移除尾随逗号
            content = re.sub(r',(\s*[}\]])', r'\1', content)
            return json.loads(content)
    
    def modify_data(self, modifications):
        """
        修改数据
        
        Args:
            modifications (dict): 要修改的数据，使用点号表示法或嵌套字典
        """
        if self.data is None:
            self.parse()
            
        for key, value in modifications.items():
            keys = key.split('.')
            obj = self.data
            
            # 遍历到倒数第二个键
            for k in keys[:-1]:
                if k.isdigit():
                    k = int(k)
                obj = obj[k]
                
            # 设置最后一个键的值
            last_key = keys[-1]
            if last_key.isdigit():
                last_key = int(last_key)
            obj[last_key] = value
    
    def save_to_file(self, output_path):
        """
        将数据保存为ADOFAI文件，保持原始格式
        
        Args:
            output_path (str): 输出文件路径
        """
        if self.data is None:
            raise ValueError("没有数据可保存，请先解析文件")
            
        # 将数据转换为JSON字符串
        json_str = json.dumps(self.data, ensure_ascii=False, indent=1)
        
        # 调整缩进以匹配ADOFAI格式
        lines = json_str.split('\n')
        formatted_lines = []
        
        for line in lines:
            # 处理缩进
            indent_level = 0
            stripped_line = line.lstrip()
            if stripped_line:
                indent_level = (len(line) - len(stripped_line)) // 2
            
            # ADOFAI使用制表符缩进
            formatted_line = '\t' * indent_level + stripped_line
            formatted_lines.append(formatted_line)
        
        # 重新组合内容
        formatted_content = '\n'.join(formatted_lines)
        
        # 添加尾随逗号以匹配原始格式
        formatted_content = self._add_trailing_commas(formatted_content)
        
        # 写入文件
        with open(output_path, 'w', encoding='utf-8') as file:
            # 添加BOM以匹配原始文件
            file.write('\ufeff')
            file.write(formatted_content)
    
    def _add_trailing_commas(self, content):
        """
        添加尾随逗号以匹配ADOFAI格式
        
        Args:
            content (str): 格式化的内容
            
        Returns:
            str: 添加尾随逗号后的内容
        """
        lines = content.split('\n')
        result_lines = []
        
        for i, line in enumerate(lines):
            # 如果行以 } 或 ] 结尾，并且下一行存在并且缩进更少，则添加逗号
            if (line.strip().endswith('}') or line.strip().endswith(']')) and i < len(lines) - 1:
                next_line = lines[i + 1]
                current_indent = len(line) - len(line.lstrip())
                next_indent = len(next_line) - len(next_line.lstrip())
                
                if next_indent < current_indent:
                    # 在当前行末尾添加逗号
                    if not line.endswith(','):
                        line = line.rstrip() + ','
                        
            result_lines.append(line)
            
        return '\n'.join(result_lines)

def main():
    """
    主函数，演示如何使用ADOFAIParser
    """
    # 创建解析器实例
    parser = ADOFAIParser('We Want To Nerf.adofai')
    
    # 解析文件
    data = parser.parse()
    print("文件解析成功!")
    print(f"关卡名称: {data['settings']['song']}")
    print(f"作者: {data['settings']['author']}")
    print(f"动作数量: {len(data['actions'])}")
    
    # 示例修改：更改关卡作者
    parser.modify_data({
        'settings.author': 'Modified by Python Script'
    })
    
    # 保存修改后的文件
    parser.save_to_file('We Want To Nerf_modified.adofai')
    print("修改后的文件已保存为 'We Want To Nerf_modified.adofai'")

if __name__ == "__main__":
    main()