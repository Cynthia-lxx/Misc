import sys
import os
from PIL import Image, ImageDraw, ImageFont, ImageColor
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QPushButton, QLabel, QFileDialog, 
                             QGroupBox, QSpinBox, QDoubleSpinBox, QComboBox,
                             QCheckBox, QMessageBox, QTextEdit, QSlider,
                             QLineEdit, QTabWidget, QColorDialog, QProgressBar)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QRect
from PyQt5.QtGui import QFont, QPixmap, QColor, QFontDatabase, QImage
import numpy as np

class TextToImageGenerator(QThread):
    """文本转图片生成线程"""
    progress = pyqtSignal(int)
    status = pyqtSignal(str)
    finished = pyqtSignal(object)  # 发送生成的图片路径
    
    def __init__(self, text, output_path, settings):
        super().__init__()
        self.text = text
        self.output_path = output_path
        self.settings = settings
        
    def run(self):
        try:
            self.status.emit("正在生成图片...")
            self.progress.emit(10)
            
            # 生成图片
            image = self.generate_image()
            
            self.progress.emit(80)
            
            # 保存图片
            image.save(self.output_path)
            
            self.progress.emit(100)
            self.status.emit("生成完成！")
            self.finished.emit(self.output_path)
            
        except Exception as e:
            self.status.emit(f"错误: {str(e)}")
            self.finished.emit(None)
    
    def generate_image(self):
        """生成文本图片"""
        # 解析文本（支持换行）
        lines = self.text.split('\n')
        
        # 获取字体
        try:
            font = ImageFont.truetype(self.settings['font_path'], self.settings['font_size'])
        except:
            # 使用默认字体
            font = ImageFont.load_default()
        
        # 计算每行文本的尺寸
        temp_img = Image.new('RGBA', (1, 1))
        temp_draw = ImageDraw.Draw(temp_img)
        
        max_width = 0
        total_height = 0
        line_heights = []
        line_widths = []
        
        for line in lines:
            if line.strip():  # 非空行
                bbox = temp_draw.textbbox((0, 0), line, font=font)
                line_width = bbox[2] - bbox[0]
                line_height = bbox[3] - bbox[1]
            else:
                # 空行使用字体高度
                bbox = temp_draw.textbbox((0, 0), " ", font=font)
                line_width = 0
                line_height = bbox[3] - bbox[1]
            
            line_widths.append(line_width)
            line_heights.append(line_height)
            max_width = max(max_width, line_width)
            total_height += line_height
        
        # 添加行间距
        line_spacing = self.settings['line_spacing']
        total_height += line_spacing * (len(lines) - 1)
        
        # 添加内边距
        padding = self.settings['padding']
        
        # 计算最终图片尺寸
        if self.settings['auto_size']:
            # 自动模式：根据文本内容确定尺寸
            final_width = max_width + padding * 2
            final_height = total_height + padding * 2
        else:
            # 手动模式：使用指定的分辨率基数
            width_base = self.settings['width_base']
            height_base = self.settings['height_base']
            
            # 根据文本行数和最长行计算实际需要的尺寸
            required_width = max_width + padding * 2
            required_height = total_height + padding * 2
            
            # 使用基数，但确保不小于所需尺寸
            final_width = max(width_base, required_width)
            final_height = max(height_base, required_height)
        
        # 创建图片
        if self.settings['background_transparent']:
            image = Image.new('RGBA', (final_width, final_height), (0, 0, 0, 0))
        else:
            bg_color = self.settings['bg_color']
            image = Image.new('RGBA', (final_width, final_height), bg_color + (255,))
        
        draw = ImageDraw.Draw(image)
        
        # 绘制文本
        current_y = padding
        
        # 文本对齐
        for i, line in enumerate(lines):
            if not line.strip() and self.settings['preserve_empty_lines']:
                # 保留空行
                current_y += line_heights[i] + line_spacing
                continue
            elif not line.strip():
                continue
            
            line_width = line_widths[i]
            
            # 根据对齐方式计算x坐标
            if self.settings['alignment'] == 'left':
                x = padding
            elif self.settings['alignment'] == 'center':
                x = (final_width - line_width) // 2
            else:  # right
                x = final_width - padding - line_width
            
            # 文本颜色
            text_color = self.settings['text_color']
            
            # 绘制文本
            draw.text((x, current_y), line, font=font, fill=text_color + (self.settings['text_opacity'],))
            
            current_y += line_heights[i] + line_spacing
        
        return image

class TextToImageApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.current_output_path = None
        self.preview_image = None
        self.font_paths = self.get_system_fonts()
        self.init_ui()
        
    def get_system_fonts(self):
        """获取系统可用字体"""
        fonts = []
        # 常见字体路径
        common_fonts = {
            'Consolas': 'C:/Windows/Fonts/consola.ttf',
            'Arial': 'C:/Windows/Fonts/arial.ttf',
            'SimHei': 'C:/Windows/Fonts/simhei.ttf',
            'SimSun': 'C:/Windows/Fonts/simsun.ttc',
            'Microsoft YaHei': 'C:/Windows/Fonts/msyh.ttc',
            'KaiTi': 'C:/Windows/Fonts/simkai.ttf',
            'FangSong': 'C:/Windows/Fonts/simfang.ttf'
        }
        
        for name, path in common_fonts.items():
            if os.path.exists(path):
                fonts.append((name, path))
        
        # 如果没有找到，添加默认字体
        if not fonts:
            fonts.append(('Default', None))
        
        return fonts
    
    def init_ui(self):
        self.setWindowTitle("文本转图片工具")
        self.setGeometry(100, 100, 1200, 800)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)
        
        # 左侧：输入区域
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        
        # 文本输入
        text_group = QGroupBox("文本输入")
        text_layout = QVBoxLayout()
        self.text_edit = QTextEdit()
        self.text_edit.setPlaceholderText("在这里输入要转换的文本...\n支持多行文本")
        text_layout.addWidget(self.text_edit)
        text_group.setLayout(text_layout)
        left_layout.addWidget(text_group)
        
        # 右侧：设置和预览
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)
        
        # 设置选项卡
        tabs = QTabWidget()
        
        # 基础设置
        basic_tab = QWidget()
        basic_layout = QVBoxLayout(basic_tab)
        
        # 字体设置
        font_group = QGroupBox("字体设置")
        font_layout = QVBoxLayout()
        
        font_select_layout = QHBoxLayout()
        font_select_layout.addWidget(QLabel("字体:"))
        self.font_combo = QComboBox()
        for font_name, _ in self.font_paths:
            self.font_combo.addItem(font_name)
        font_select_layout.addWidget(self.font_combo)
        font_layout.addLayout(font_select_layout)
        
        font_size_layout = QHBoxLayout()
        font_size_layout.addWidget(QLabel("字体大小:"))
        self.font_size_spin = QSpinBox()
        self.font_size_spin.setRange(8, 500)
        self.font_size_spin.setValue(48)
        font_size_layout.addWidget(self.font_size_spin)
        font_layout.addLayout(font_size_layout)
        
        font_group.setLayout(font_layout)
        basic_layout.addWidget(font_group)
        
        # 颜色设置
        color_group = QGroupBox("颜色设置")
        color_layout = QVBoxLayout()
        
        text_color_layout = QHBoxLayout()
        text_color_layout.addWidget(QLabel("文字颜色:"))
        self.text_color_btn = QPushButton()
        self.text_color_btn.setFixedSize(50, 30)
        self.text_color_btn.setStyleSheet("background-color: rgb(0, 0, 0);")
        self.text_color_btn.clicked.connect(lambda: self.choose_color('text'))
        text_color_layout.addWidget(self.text_color_btn)
        self.text_color_label = QLabel("RGB(0, 0, 0)")
        text_color_layout.addWidget(self.text_color_label)
        text_color_layout.addStretch()
        color_layout.addLayout(text_color_layout)
        
        text_opacity_layout = QHBoxLayout()
        text_opacity_layout.addWidget(QLabel("文字透明度:"))
        self.text_opacity_slider = QSlider(Qt.Horizontal)
        self.text_opacity_slider.setRange(0, 255)
        self.text_opacity_slider.setValue(255)
        self.text_opacity_slider.valueChanged.connect(self.update_preview)
        text_opacity_layout.addWidget(self.text_opacity_slider)
        self.text_opacity_label = QLabel("255")
        text_opacity_layout.addWidget(self.text_opacity_label)
        color_layout.addLayout(text_opacity_layout)
        
        bg_color_layout = QHBoxLayout()
        bg_color_layout.addWidget(QLabel("背景颜色:"))
        self.bg_color_btn = QPushButton()
        self.bg_color_btn.setFixedSize(50, 30)
        self.bg_color_btn.setStyleSheet("background-color: rgb(255, 255, 255);")
        self.bg_color_btn.clicked.connect(lambda: self.choose_color('bg'))
        bg_color_layout.addWidget(self.bg_color_btn)
        self.bg_color_label = QLabel("RGB(255, 255, 255)")
        bg_color_layout.addWidget(self.bg_color_label)
        bg_color_layout.addStretch()
        color_layout.addLayout(bg_color_layout)
        
        self.bg_transparent_check = QCheckBox("透明背景")
        self.bg_transparent_check.toggled.connect(self.on_transparent_toggle)
        color_layout.addWidget(self.bg_transparent_check)
        
        color_group.setLayout(color_layout)
        basic_layout.addWidget(color_group)
        
        # 布局设置
        layout_group = QGroupBox("布局设置")
        layout_layout = QVBoxLayout()
        
        alignment_layout = QHBoxLayout()
        alignment_layout.addWidget(QLabel("文本对齐:"))
        self.alignment_combo = QComboBox()
        self.alignment_combo.addItems(["左对齐", "居中对齐", "右对齐"])
        alignment_layout.addWidget(self.alignment_combo)
        layout_layout.addLayout(alignment_layout)
        
        spacing_layout = QHBoxLayout()
        spacing_layout.addWidget(QLabel("行间距:"))
        self.line_spacing_spin = QSpinBox()
        self.line_spacing_spin.setRange(0, 100)
        self.line_spacing_spin.setValue(10)
        spacing_layout.addWidget(self.line_spacing_spin)
        layout_layout.addLayout(spacing_layout)
        
        padding_layout = QHBoxLayout()
        padding_layout.addWidget(QLabel("内边距:"))
        self.padding_spin = QSpinBox()
        self.padding_spin.setRange(0, 200)
        self.padding_spin.setValue(20)
        padding_layout.addWidget(self.padding_spin)
        layout_layout.addLayout(padding_layout)
        
        self.preserve_empty_check = QCheckBox("保留空行")
        self.preserve_empty_check.setChecked(True)
        layout_layout.addWidget(self.preserve_empty_check)
        
        layout_group.setLayout(layout_layout)
        basic_layout.addWidget(layout_group)
        
        # 尺寸设置
        size_group = QGroupBox("尺寸设置")
        size_layout = QVBoxLayout()
        
        self.auto_size_check = QCheckBox("自动调整尺寸")
        self.auto_size_check.setChecked(True)
        self.auto_size_check.toggled.connect(self.on_auto_size_toggle)
        size_layout.addWidget(self.auto_size_check)
        
        manual_size_layout = QHBoxLayout()
        manual_size_layout.addWidget(QLabel("宽度基数:"))
        self.width_base_spin = QSpinBox()
        self.width_base_spin.setRange(100, 5000)
        self.width_base_spin.setValue(800)
        self.width_base_spin.setEnabled(False)
        manual_size_layout.addWidget(self.width_base_spin)
        
        manual_size_layout.addWidget(QLabel("高度基数:"))
        self.height_base_spin = QSpinBox()
        self.height_base_spin.setRange(100, 5000)
        self.height_base_spin.setValue(600)
        self.height_base_spin.setEnabled(False)
        manual_size_layout.addWidget(self.height_base_spin)
        size_layout.addLayout(manual_size_layout)
        
        size_group.setLayout(size_layout)
        basic_layout.addWidget(size_group)
        
        basic_layout.addStretch()
        tabs.addTab(basic_tab, "基本设置")
        
        right_layout.addWidget(tabs)
        
        # 预览区域
        preview_group = QGroupBox("预览")
        preview_layout = QVBoxLayout()
        self.preview_label = QLabel()
        self.preview_label.setAlignment(Qt.AlignCenter)
        self.preview_label.setMinimumHeight(300)
        self.preview_label.setStyleSheet("border: 1px solid #ccc; background-color: #f5f5f5;")
        preview_layout.addWidget(self.preview_label)
        
        preview_controls = QHBoxLayout()
        self.refresh_preview_btn = QPushButton("刷新预览")
        self.refresh_preview_btn.clicked.connect(self.update_preview)
        preview_controls.addWidget(self.refresh_preview_btn)
        
        preview_layout.addLayout(preview_controls)
        preview_group.setLayout(preview_layout)
        right_layout.addWidget(preview_group)
        
        # 输出控制
        output_group = QGroupBox("输出")
        output_layout = QVBoxLayout()
        
        output_path_layout = QHBoxLayout()
        self.output_path_edit = QLineEdit()
        self.output_path_edit.setPlaceholderText("输出文件路径（留空则保存到桌面）")
        output_path_layout.addWidget(self.output_path_edit)
        browse_btn = QPushButton("浏览")
        browse_btn.clicked.connect(self.select_output_path)
        output_path_layout.addWidget(browse_btn)
        output_layout.addLayout(output_path_layout)
        
        self.progress_bar = QProgressBar()
        output_layout.addWidget(self.progress_bar)
        
        self.status_label = QLabel("就绪")
        output_layout.addWidget(self.status_label)
        
        generate_btn = QPushButton("生成图片")
        generate_btn.setStyleSheet("background-color: #4CAF50; color: white; padding: 10px; font-size: 14px;")
        generate_btn.clicked.connect(self.generate_image)
        output_layout.addWidget(generate_btn)
        
        output_group.setLayout(output_layout)
        right_layout.addWidget(output_group)
        
        # 添加到主布局
        main_layout.addWidget(left_widget, 1)
        main_layout.addWidget(right_widget, 1)
        
        # 连接信号
        self.text_edit.textChanged.connect(self.auto_update_preview)
        self.font_combo.currentIndexChanged.connect(self.update_preview)
        self.font_size_spin.valueChanged.connect(self.update_preview)
        self.alignment_combo.currentIndexChanged.connect(self.update_preview)
        self.line_spacing_spin.valueChanged.connect(self.update_preview)
        self.padding_spin.valueChanged.connect(self.update_preview)
        self.preserve_empty_check.toggled.connect(self.update_preview)
        self.auto_size_check.toggled.connect(self.update_preview)
        self.width_base_spin.valueChanged.connect(self.update_preview)
        self.height_base_spin.valueChanged.connect(self.update_preview)
        
        self.text_color = (0, 0, 0)
        self.bg_color = (255, 255, 255)
        
        # 初始化预览
        self.update_preview()
    
    def choose_color(self, color_type):
        color = QColorDialog.getColor()
        if color.isValid():
            rgb = (color.red(), color.green(), color.blue())
            if color_type == 'text':
                self.text_color = rgb
                self.text_color_btn.setStyleSheet(f"background-color: rgb({rgb[0]}, {rgb[1]}, {rgb[2]});")
                self.text_color_label.setText(f"RGB({rgb[0]}, {rgb[1]}, {rgb[2]})")
            else:
                self.bg_color = rgb
                self.bg_color_btn.setStyleSheet(f"background-color: rgb({rgb[0]}, {rgb[1]}, {rgb[2]});")
                self.bg_color_label.setText(f"RGB({rgb[0]}, {rgb[1]}, {rgb[2]})")
            self.update_preview()
    
    def on_transparent_toggle(self, checked):
        self.bg_color_btn.setEnabled(not checked)
        self.bg_color_label.setEnabled(not checked)
        self.update_preview()
    
    def on_auto_size_toggle(self, checked):
        self.width_base_spin.setEnabled(not checked)
        self.height_base_spin.setEnabled(not checked)
        self.update_preview()
    
    def auto_update_preview(self):
        # 延迟更新预览，避免频繁刷新
        from PyQt5.QtCore import QTimer
        if hasattr(self, '_preview_timer'):
            self._preview_timer.stop()
        self._preview_timer = QTimer()
        self._preview_timer.setSingleShot(True)
        self._preview_timer.timeout.connect(self.update_preview)
        self._preview_timer.start(500)
    
    def update_preview(self):
        if not self.text_edit.toPlainText().strip():
            self.preview_label.setText("请输入文本以预览")
            return
        
        # 创建临时生成器用于预览
        temp_path = os.path.join(os.path.expanduser("~"), "temp_preview.png")
        
        settings = self.get_current_settings()
        
        # 异步生成预览
        self.preview_generator = TextToImageGenerator(
            self.text_edit.toPlainText(),
            temp_path,
            settings
        )
        self.preview_generator.finished.connect(self.show_preview)
        self.preview_generator.start()
    
    def show_preview(self, path):
        if path and os.path.exists(path):
            pixmap = QPixmap(path)
            # 缩放预览以适应窗口
            scaled_pixmap = pixmap.scaled(400, 300, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            self.preview_label.setPixmap(scaled_pixmap)
    
    def get_current_settings(self):
        """获取当前设置"""
        font_path = None
        if self.font_combo.currentIndex() >= 0:
            font_name = self.font_combo.currentText()
            for name, path in self.font_paths:
                if name == font_name:
                    font_path = path
                    break
        
        return {
            'font_path': font_path,
            'font_size': self.font_size_spin.value(),
            'text_color': self.text_color,
            'text_opacity': self.text_opacity_slider.value(),
            'bg_color': self.bg_color,
            'background_transparent': self.bg_transparent_check.isChecked(),
            'alignment': ['left', 'center', 'right'][self.alignment_combo.currentIndex()],
            'line_spacing': self.line_spacing_spin.value(),
            'padding': self.padding_spin.value(),
            'preserve_empty_lines': self.preserve_empty_check.isChecked(),
            'auto_size': self.auto_size_check.isChecked(),
            'width_base': self.width_base_spin.value(),
            'height_base': self.height_base_spin.value()
        }
    
    def select_output_path(self):
        path, _ = QFileDialog.getSaveFileName(
            self, "保存图片", "", 
            "PNG Image (*.png);;JPEG Image (*.jpg);;All Files (*)"
        )
        if path:
            self.output_path_edit.setText(path)
    
    def generate_image(self):
        text = self.text_edit.toPlainText()
        if not text.strip():
            QMessageBox.warning(self, "警告", "请输入要转换的文本！")
            return
        
        # 确定输出路径
        output_path = self.output_path_edit.text().strip()
        if not output_path:
            # 默认保存到桌面
            desktop = os.path.join(os.path.expanduser("~"), "Desktop")
            output_path = os.path.join(desktop, "text_image.png")
        
        # 确保是png格式
        if not output_path.lower().endswith(('.png', '.jpg', '.jpeg')):
            output_path += '.png'
        
        settings = self.get_current_settings()
        
        self.generator = TextToImageGenerator(text, output_path, settings)
        self.generator.progress.connect(self.update_progress)
        self.generator.status.connect(self.update_status)
        self.generator.finished.connect(self.on_generation_finished)
        
        self.generator.start()
    
    def update_progress(self, value):
        self.progress_bar.setValue(value)
    
    def update_status(self, message):
        self.status_label.setText(message)
    
    def on_generation_finished(self, path):
        if path:
            QMessageBox.information(self, "成功", f"图片已保存到:\n{path}")
            # 在预览中显示生成的图片
            pixmap = QPixmap(path)
            scaled_pixmap = pixmap.scaled(400, 300, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            self.preview_label.setPixmap(scaled_pixmap)

def main():
    app = QApplication(sys.argv)
    window = TextToImageApp()
    window.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()