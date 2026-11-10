import sys
import os
from pathlib import Path
from PIL import Image
import numpy as np
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QPushButton, QLabel, QFileDialog, 
                             QListWidget, QGroupBox, QColorDialog, QSpinBox,
                             QDoubleSpinBox, QComboBox, QCheckBox, QMessageBox,
                             QProgressBar, QLineEdit)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QRect
from PyQt5.QtGui import QPixmap, QColor, QImage, QDragEnterEvent, QDropEvent

class ImageProcessor(QThread):
    """图片处理线程"""
    progress = pyqtSignal(int)
    status = pyqtSignal(str)
    finished = pyqtSignal()
    
    def __init__(self, input_paths, output_dir, color_rgb, tolerance, 
                 mode, suffix, keep_structure=False):
        super().__init__()
        self.input_paths = input_paths
        self.output_dir = output_dir
        self.color_rgb = color_rgb
        self.tolerance = tolerance
        self.mode = mode  # 'keep_color' 或 'remove_color'
        self.suffix = suffix
        self.keep_structure = keep_structure
        
    def run(self):
        total = len(self.input_paths)
        for i, input_path in enumerate(self.input_paths):
            try:
                self.status.emit(f"处理中: {os.path.basename(input_path)}")
                self.process_single_image(input_path)
                self.progress.emit(int((i + 1) / total * 100))
            except Exception as e:
                self.status.emit(f"错误: {os.path.basename(input_path)} - {str(e)}")
        self.status.emit("处理完成！")
        self.finished.emit()
    
    def process_single_image(self, input_path):
        """处理单张图片"""
        # 打开图片并转换为RGBA模式
        img = Image.open(input_path)
        img = img.convert('RGBA')
        data = np.array(img)
        
        # 创建掩码
        r, g, b = data[:,:,0], data[:,:,1], data[:,:,2]
        target_r, target_g, target_b = self.color_rgb
        
        # 计算颜色差异
        diff = np.sqrt((r - target_r)**2 + (g - target_g)**2 + (b - target_b)**2)
        
        if self.mode == 'remove_color':
            # 删除指定颜色，使其透明
            mask = diff <= self.tolerance
        else:
            # 保留指定颜色，其他颜色透明
            mask = diff > self.tolerance
        
        # 设置透明度
        data[:,:,3] = np.where(mask, 0, 255)
        
        # 保存图片
        output_filename = f"{Path(input_path).stem}_{self.suffix}.png"
        
        if self.keep_structure:
            # 保持目录结构
            rel_path = os.path.relpath(input_path, os.path.commonpath(self.input_paths))
            output_path = os.path.join(self.output_dir, os.path.dirname(rel_path))
            os.makedirs(output_path, exist_ok=True)
            output_path = os.path.join(output_path, output_filename)
        else:
            output_path = os.path.join(self.output_dir, output_filename)
        
        result_img = Image.fromarray(data, 'RGBA')
        result_img.save(output_path)

class ColorPickWidget(QWidget):
    """颜色选择和显示组件"""
    color_changed = pyqtSignal(tuple)
    
    def __init__(self):
        super().__init__()
        self.current_color = (255, 0, 0)  # 默认红色
        self.init_ui()
        
    def init_ui(self):
        layout = QHBoxLayout()
        
        self.color_label = QLabel()
        self.color_label.setFixedSize(50, 30)
        self.color_label.setStyleSheet(f"background-color: rgb({self.current_color[0]}, {self.current_color[1]}, {self.current_color[2]}); border: 1px solid black;")
        
        self.pick_button = QPushButton("选择颜色")
        self.pick_button.clicked.connect(self.pick_color)
        
        self.color_info = QLabel(f"RGB: {self.current_color}")
        
        layout.addWidget(self.color_label)
        layout.addWidget(self.pick_button)
        layout.addWidget(self.color_info)
        layout.addStretch()
        
        self.setLayout(layout)
    
    def pick_color(self):
        color = QColorDialog.getColor()
        if color.isValid():
            self.current_color = (color.red(), color.green(), color.blue())
            self.color_label.setStyleSheet(f"background-color: rgb({self.current_color[0]}, {self.current_color[1]}, {self.current_color[2]}); border: 1px solid black;")
            self.color_info.setText(f"RGB: {self.current_color}")
            self.color_changed.emit(self.current_color)
    
    def get_color(self):
        return self.current_color

class DropListWidget(QListWidget):
    """支持拖放的文件列表组件"""
    files_dropped = pyqtSignal(list)
    
    def __init__(self):
        super().__init__()
        self.setAcceptDrops(True)
        self.setSelectionMode(QListWidget.ExtendedSelection)
        
    def dragEnterEvent(self, event: QDragEnterEvent):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()
    
    def dropEvent(self, event: QDropEvent):
        files = []
        for url in event.mimeData().urls():
            file_path = url.toLocalFile()
            if file_path.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif', '.tiff')):
                files.append(file_path)
        if files:
            self.files_dropped.emit(files)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.input_files = []
        self.processor = None
        self.init_ui()
        
    def init_ui(self):
        self.setWindowTitle("颜色抠图工具 - 按颜色透明化")
        self.setGeometry(100, 100, 800, 600)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)
        
        # 左侧：文件列表区域
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        
        file_group = QGroupBox("文件列表（支持拖放）")
        file_layout = QVBoxLayout()
        
        self.file_list = DropListWidget()
        self.file_list.files_dropped.connect(self.add_files)
        file_layout.addWidget(self.file_list)
        
        btn_layout = QHBoxLayout()
        add_btn = QPushButton("添加图片")
        add_btn.clicked.connect(self.add_files_dialog)
        remove_btn = QPushButton("移除选中")
        remove_btn.clicked.connect(self.remove_selected)
        clear_btn = QPushButton("清空列表")
        clear_btn.clicked.connect(self.clear_list)
        
        btn_layout.addWidget(add_btn)
        btn_layout.addWidget(remove_btn)
        btn_layout.addWidget(clear_btn)
        file_layout.addLayout(btn_layout)
        
        file_group.setLayout(file_layout)
        left_layout.addWidget(file_group)
        
        # 右侧：设置区域
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)
        
        # 颜色选择
        color_group = QGroupBox("颜色设置")
        color_layout = QVBoxLayout()
        self.color_picker = ColorPickWidget()
        color_layout.addWidget(self.color_picker)
        color_group.setLayout(color_layout)
        right_layout.addWidget(color_group)
        
        # 抠图模式
        mode_group = QGroupBox("抠图模式")
        mode_layout = QVBoxLayout()
        self.mode_combo = QComboBox()
        self.mode_combo.addItems(["删除指定颜色（使其透明）", "保留指定颜色（其他透明）"])
        mode_layout.addWidget(self.mode_combo)
        mode_group.setLayout(mode_layout)
        right_layout.addWidget(mode_group)
        
        # 容差设置
        tolerance_group = QGroupBox("颜色容差")
        tolerance_layout = QHBoxLayout()
        tolerance_layout.addWidget(QLabel("容差值 (0-100):"))
        self.tolerance_spin = QDoubleSpinBox()
        self.tolerance_spin.setRange(0, 100)
        self.tolerance_spin.setValue(30)
        self.tolerance_spin.setSingleStep(5)
        tolerance_layout.addWidget(self.tolerance_spin)
        tolerance_group.setLayout(tolerance_layout)
        right_layout.addWidget(tolerance_group)
        
        # 输出设置
        output_group = QGroupBox("输出设置")
        output_layout = QVBoxLayout()
        
        suffix_layout = QHBoxLayout()
        suffix_layout.addWidget(QLabel("文件后缀:"))
        self.suffix_edit = QLineEdit("transparent")
        suffix_layout.addWidget(self.suffix_edit)
        output_layout.addLayout(suffix_layout)
        
        self.keep_structure_check = QCheckBox("保持原始目录结构")
        output_layout.addWidget(self.keep_structure_check)
        
        output_dir_layout = QHBoxLayout()
        self.output_dir_edit = QLineEdit()
        self.output_dir_edit.setPlaceholderText("输出目录（默认为输入文件所在目录）")
        output_dir_layout.addWidget(self.output_dir_edit)
        browse_btn = QPushButton("浏览")
        browse_btn.clicked.connect(self.select_output_dir)
        output_dir_layout.addWidget(browse_btn)
        output_layout.addLayout(output_dir_layout)
        
        output_group.setLayout(output_layout)
        right_layout.addWidget(output_group)
        
        # 处理控制
        control_group = QGroupBox("处理控制")
        control_layout = QVBoxLayout()
        
        self.progress_bar = QProgressBar()
        control_layout.addWidget(self.progress_bar)
        
        self.status_label = QLabel("就绪")
        control_layout.addWidget(self.status_label)
        
        btn_layout = QHBoxLayout()
        self.process_btn = QPushButton("开始处理")
        self.process_btn.clicked.connect(self.start_processing)
        self.process_btn.setStyleSheet("background-color: #4CAF50; color: white; padding: 5px;")
        
        self.stop_btn = QPushButton("停止")
        self.stop_btn.clicked.connect(self.stop_processing)
        self.stop_btn.setEnabled(False)
        
        btn_layout.addWidget(self.process_btn)
        btn_layout.addWidget(self.stop_btn)
        control_layout.addLayout(btn_layout)
        
        control_group.setLayout(control_layout)
        right_layout.addWidget(control_group)
        
        # 右侧布局设置
        right_layout.addStretch()
        
        # 添加到主布局
        main_layout.addWidget(left_widget, 2)
        main_layout.addWidget(right_widget, 1)
        
        # 设置样式
        self.setStyleSheet("""
            QGroupBox {
                font-weight: bold;
                border: 1px solid #ccc;
                border-radius: 5px;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px 0 5px;
            }
            QPushButton {
                padding: 5px;
            }
        """)
    
    def add_files_dialog(self):
        files, _ = QFileDialog.getOpenFileNames(
            self, "选择图片文件", "", 
            "Images (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)"
        )
        if files:
            self.add_files(files)
    
    def add_files(self, files):
        for file in files:
            if file not in self.input_files:
                self.input_files.append(file)
                self.file_list.addItem(file)
        self.status_label.setText(f"已添加 {len(files)} 个文件，共 {len(self.input_files)} 个文件")
    
    def remove_selected(self):
        for item in self.file_list.selectedItems():
            self.input_files.remove(item.text())
            self.file_list.takeItem(self.file_list.row(item))
        self.status_label.setText(f"当前共 {len(self.input_files)} 个文件")
    
    def clear_list(self):
        self.input_files.clear()
        self.file_list.clear()
        self.status_label.setText("列表已清空")
    
    def select_output_dir(self):
        dir_path = QFileDialog.getExistingDirectory(self, "选择输出目录")
        if dir_path:
            self.output_dir_edit.setText(dir_path)
    
    def start_processing(self):
        if not self.input_files:
            QMessageBox.warning(self, "警告", "请先添加要处理的图片文件！")
            return
        
        # 获取输出目录
        output_dir = self.output_dir_edit.text()
        if not output_dir:
            # 使用第一个输入文件的目录
            output_dir = os.path.dirname(self.input_files[0])
        
        # 创建输出目录
        os.makedirs(output_dir, exist_ok=True)
        
        # 获取参数
        color = self.color_picker.get_color()
        tolerance = self.tolerance_spin.value()
        mode = 'remove_color' if self.mode_combo.currentIndex() == 0 else 'keep_color'
        suffix = self.suffix_edit.text().strip()
        if not suffix:
            suffix = "transparent"
        keep_structure = self.keep_structure_check.isChecked()
        
        # 创建并启动处理线程
        self.processor = ImageProcessor(
            self.input_files, output_dir, color, tolerance, 
            mode, suffix, keep_structure
        )
        self.processor.progress.connect(self.update_progress)
        self.processor.status.connect(self.update_status)
        self.processor.finished.connect(self.processing_finished)
        
        self.process_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        self.progress_bar.setValue(0)
        self.processor.start()
    
    def stop_processing(self):
        if self.processor and self.processor.isRunning():
            self.processor.terminate()
            self.processor.wait()
            self.update_status("处理已停止")
            self.processing_finished()
    
    def update_progress(self, value):
        self.progress_bar.setValue(value)
    
    def update_status(self, message):
        self.status_label.setText(message)
    
    def processing_finished(self):
        self.process_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        QMessageBox.information(self, "完成", "图片处理完成！")

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()