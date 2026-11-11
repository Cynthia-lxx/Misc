import sys
import os
import subprocess
import platform
from typing import Optional
from PySide6.QtWidgets import *
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWebEngineWidgets import *
from PySide6.QtWebEngineCore import *

PTH = r"P:\Dev\ComfyUI_Refs\ComfyDL_UI"
MODEL_PTH = os.path.join(PTH, "models\\")
INPUT_PTH = os.path.join(PTH, "input\\")
OUTPUT_PTH = os.path.join(PTH, "output\\")

# ==================== 多语言支持 ====================

class LanguageManager:
    """多语言管理器 - 硬编码语言包"""
    
    # 语言包定义
    LANGUAGES = {
        'zh': {
            # 窗口标题
            'window_title': "ComfyDL UI",
            
            # 按钮文字
            'btn_back': "◀ 后退",
            'btn_forward': "前进 ▶",
            'btn_upload': "📎 上传文件",
            'btn_models': "📁 模型目录",
            'btn_input': "📁 输入目录",
            'btn_output': "📁 输出目录",
            
            # 状态栏消息
            'status_downloading': "下载中: {} - {}%",
            'status_download_finished': "下载完成: {}",
            
            # 消息框
            'msg_upload_title': "上传提示",
            'msg_upload_content': "已选择文件: {}\n\n请点击网页上的'选择文件'或'上传'按钮完成上传。\n（本按钮仅为辅助选择，实际上传需在网页上操作）",
            'msg_no_upload_element': "当前页面没有找到文件上传控件",
            'msg_upload_alert': "请在弹出的系统对话框中选择文件",
            
            # 下载相关
            'msg_download_finished_title': "下载完成",
            'msg_download_finished_content': "文件 {} 已保存到:\n{}",
            
            # 错误提示
            'msg_error_title': "错误",
            'msg_error_folder': "目录不存在: {}",
            
            # 其他
            'label_upload_hint': "",
        },
        'en': {
            # Window title
            'window_title': "ComfyDL UI",
            
            # Button texts
            'btn_back': "◀ Back",
            'btn_forward': "Forward ▶",
            'btn_upload': "📎 Upload File",
            'btn_models': "📁 Models",
            'btn_input': "📁 Input",
            'btn_output': "📁 Output",
            
            # Status bar messages
            'status_downloading': "Downloading: {} - {}%",
            'status_download_finished': "Download finished: {}",
            
            # Message boxes
            'msg_upload_title': "Upload Hint",
            'msg_upload_content': "Selected file: {}\n\nPlease click the 'Choose File' or 'Upload' button on the webpage to complete the upload.\n(This button is just a helper, actual upload must be done on the webpage)",
            'msg_no_upload_element': "No file upload control found on current page",
            'msg_upload_alert': "Please select a file in the system dialog",
            
            # Download related
            'msg_download_finished_title': "Download Complete",
            'msg_download_finished_content': "File {} has been saved to:\n{}",
            
            # Error messages
            'msg_error_title': "Error",
            'msg_error_folder': "Directory does not exist: {}",
            
            # Other
            'label_upload_hint': "",
        }
    }
    
    @staticmethod
    def get_system_language() -> str:
        """检测系统语言，返回 'zh' 或 'en'"""
        try:
            # 获取系统语言环境
            lang = os.environ.get('LANG', '')
            if not lang:
                # Windows系统
                import ctypes
                try:
                    # 获取Windows系统UI语言
                    windll = ctypes.windll.kernel32
                    lang_id = windll.GetUserDefaultUILanguage()
                    # 中文语言ID范围：0x0404 (繁体) 或 0x0804 (简体)
                    if lang_id in [0x0804, 0x0404, 0x0C04, 0x1004, 0x1404]:
                        return 'zh'
                except:
                    pass
                
                # 尝试从环境变量获取
                lang = os.environ.get('LANGUAGE', '')
                if not lang:
                    lang = os.environ.get('LC_ALL', '')
                if not lang:
                    lang = os.environ.get('LC_MESSAGES', '')
            
            # 判断是否为中文
            if lang.lower().startswith(('zh', 'zh_cn', 'zh_tw', 'zh_hk')):
                return 'zh'
        except:
            pass
        
        return 'en'  # 默认英文
    
    def __init__(self):
        """初始化语言管理器，自动检测系统语言"""
        self.current_language = self.get_system_language()
        self.strings = self.LANGUAGES[self.current_language]
    
    def set_language(self, lang_code: str):
        """手动设置语言：'zh' 或 'en'"""
        if lang_code in self.LANGUAGES:
            self.current_language = lang_code
            self.strings = self.LANGUAGES[lang_code]
    
    def get(self, key: str) -> str:
        """获取指定键的翻译文本"""
        return self.strings.get(key, key)


# ==================== 自定义证书错误处理 ====================

class CustomWebEnginePage(QWebEnginePage):
    """自定义网页页面，忽略SSL证书错误"""
    
    def certificateError(self, error):
        """忽略所有SSL证书错误"""
        # 接受证书并继续加载
        error.acceptCertificate()
        return True


# ==================== 浏览器窗口类 ====================

class SimpleBrowserWindow(QMainWindow):
    """简化浏览器窗口 - 可被外部调用"""
    
    def __init__(self, initial_url: str = "http://127.0.0.1:8188", lang_manager: Optional[LanguageManager] = None):
        super().__init__()
        
        # 语言管理器
        self.lang = lang_manager if lang_manager else LanguageManager()
        
        # 当前显示的URL
        self.current_url = initial_url
        
        # 历史记录（用于前进后退）
        self.history = []
        self.current_history_index = -1
        
        # 下载目录
        self.download_dir = QStandardPaths.writableLocation(
            QStandardPaths.DownloadLocation
        )
        
        # 创建浏览器组件
        self.browser = QWebEngineView()
        
        # ---- 关键修改：通过 browser.settings() 获取设置对象 ----
        settings = self.browser.settings()
        # 允许本地文档访问远程URL
        settings.setAttribute(QWebEngineSettings.WebAttribute.LocalContentCanAccessRemoteUrls, True)
        # 允许本地文档访问文件URL
        settings.setAttribute(QWebEngineSettings.WebAttribute.LocalContentCanAccessFileUrls, True)
        # 禁用错误页面（避免SSL错误页面干扰）
        settings.setAttribute(QWebEngineSettings.WebAttribute.ErrorPageEnabled, False)
        
        # 配置profile
        profile = QWebEngineProfile.defaultProfile()
        profile.downloadRequested.connect(self.on_download_requested)
        profile.setHttpCacheType(QWebEngineProfile.HttpCacheType.DiskHttpCache)
        
        # 使用自定义页面忽略SSL证书错误
        self.page = CustomWebEnginePage(self.browser)
        self.browser.setPage(self.page)
        
        # 初始化界面
        self.init_ui()
        
        # 加载初始页面（确保使用http）
        if '127.0.0.1' in initial_url:
            initial_url = initial_url.replace('https://', 'http://')
            self.current_url = initial_url
        
        self.load_url(initial_url)
    
    def init_ui(self):
        """初始化用户界面"""
        self.setWindowTitle(self.lang.get('window_title'))
        self.setGeometry(100, 100, 1200, 800)
        
        # 中央部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(5, 5, 5, 5)
        main_layout.setSpacing(5)
        
        # ---------- 工具栏 ----------
        toolbar_layout = QHBoxLayout()
        toolbar_layout.setSpacing(10)
        
        # 1. 后退按钮
        self.back_btn = QPushButton(self.lang.get('btn_back'))
        self.back_btn.setFixedHeight(35)
        self.back_btn.clicked.connect(self.go_back)
        self.back_btn.setEnabled(False)
        toolbar_layout.addWidget(self.back_btn)
        
        # 2. 前进按钮
        self.forward_btn = QPushButton(self.lang.get('btn_forward'))
        self.forward_btn.setFixedHeight(35)
        self.forward_btn.clicked.connect(self.go_forward)
        self.forward_btn.setEnabled(False)
        toolbar_layout.addWidget(self.forward_btn)
        
        # 3. 当前URL显示标签
        self.url_label = QLabel(self.current_url)
        self.url_label.setStyleSheet("""
            QLabel {
                border: 1px solid #ccc;
                border-radius: 3px;
                padding: 5px;
                background-color: #f8f8f8;
                font-size: 12px;
            }
        """)
        self.url_label.setWordWrap(True)
        self.url_label.setMinimumHeight(30)
        toolbar_layout.addWidget(self.url_label, 1)
        
        # 4. 上传文件按钮
        self.upload_btn = QPushButton(self.lang.get('btn_upload'))
        self.upload_btn.setFixedHeight(35)
        self.upload_btn.clicked.connect(self.upload_file)
        toolbar_layout.addWidget(self.upload_btn)
        
        # 5. 模型目录按钮
        self.models_btn = QPushButton(self.lang.get('btn_models'))
        self.models_btn.setFixedHeight(35)
        self.models_btn.clicked.connect(lambda: self.open_folder(MODEL_PTH, '模型'))
        toolbar_layout.addWidget(self.models_btn)
        
        # 6. 输入目录按钮
        self.input_btn = QPushButton(self.lang.get('btn_input'))
        self.input_btn.setFixedHeight(35)
        self.input_btn.clicked.connect(lambda: self.open_folder(INPUT_PTH, '输入'))
        toolbar_layout.addWidget(self.input_btn)
        
        # 7. 输出目录按钮
        self.output_btn = QPushButton(self.lang.get('btn_output'))
        self.output_btn.setFixedHeight(35)
        self.output_btn.clicked.connect(lambda: self.open_folder(OUTPUT_PTH, '输出'))
        toolbar_layout.addWidget(self.output_btn)
        
        main_layout.addLayout(toolbar_layout)
        
        # ---------- 浏览器主体 ----------
        main_layout.addWidget(self.browser, 1)
        
        # 连接信号
        self.browser.loadFinished.connect(self.on_load_finished)
        self.browser.urlChanged.connect(self.on_url_changed)
        
        # 状态栏
        self.statusBar().showMessage(self.lang.get('label_upload_hint'))
    
    # ==================== 核心导航功能 ====================
    
    def load_url(self, url: str):
        """加载指定的URL（外部调用接口）"""
        # 自动将https转为http（针对本地服务器）
        if '127.0.0.1' in url:
            url = url.replace('https://', 'http://')
        
        # 如果没有协议，自动添加http
        if not url.startswith(('http://', 'https://', 'file://')):
            url = 'http://' + url
            
        self.current_url = url
        self.browser.setUrl(QUrl(url))
        self.url_label.setText(url)
        
        # 清空历史并添加当前URL
        self.history.clear()
        self.current_history_index = -1
        self.add_to_history(url)
    
    def add_to_history(self, url: str):
        """添加到历史记录"""
        if self.current_history_index < len(self.history) - 1:
            self.history = self.history[:self.current_history_index + 1]
        
        self.history.append(url)
        self.current_history_index = len(self.history) - 1
        self.update_nav_buttons()
    
    def update_nav_buttons(self):
        """更新导航按钮状态"""
        self.back_btn.setEnabled(self.current_history_index > 0)
        self.forward_btn.setEnabled(self.current_history_index < len(self.history) - 1)
    
    def go_back(self):
        """后退"""
        if self.current_history_index > 0:
            self.current_history_index -= 1
            url = self.history[self.current_history_index]
            self.browser.setUrl(QUrl(url))
            self.url_label.setText(url)
            self.update_nav_buttons()
    
    def go_forward(self):
        """前进"""
        if self.current_history_index < len(self.history) - 1:
            self.current_history_index += 1
            url = self.history[self.current_history_index]
            self.browser.setUrl(QUrl(url))
            self.url_label.setText(url)
            self.update_nav_buttons()
    
    def on_load_finished(self, ok: bool):
        """页面加载完成"""
        if ok:
            current_url = self.browser.url().toString()
            if (self.current_history_index == -1 or 
                self.history[self.current_history_index] != current_url):
                self.add_to_history(current_url)
    
    def on_url_changed(self, url: QUrl):
        """URL改变时更新显示"""
        self.url_label.setText(url.toString())
    
    # ==================== 文件上传 ====================
    
    def upload_file(self):
        """上传文件"""
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            self.lang.get('msg_upload_title'),
            "",
            "所有文件 (*.*)"
        )
        
        if file_path:
            # 检查页面是否有文件上传控件
            js_check = """
            (function() {
                var inputs = document.querySelectorAll('input[type="file"]');
                return inputs.length > 0;
            })();
            """
            
            def check_result(has_input):
                if has_input:
                    QMessageBox.information(
                        self,
                        self.lang.get('msg_upload_title'),
                        self.lang.get('msg_upload_content').format(
                            os.path.basename(file_path)
                        )
                    )
                else:
                    QMessageBox.warning(
                        self,
                        self.lang.get('msg_error_title'),
                        self.lang.get('msg_no_upload_element')
                    )
            
            self.browser.page().runJavaScript(js_check, check_result)
    
    # ==================== 文件目录管理 ====================
    
    def open_folder(self, folder_path: str, folder_name: str = ""):
        """打开指定文件夹"""
        # 确保路径存在，不存在则创建
        if not os.path.exists(folder_path):
            try:
                os.makedirs(folder_path, exist_ok=True)
                self.statusBar().showMessage(f"已创建目录: {folder_path}")
            except Exception as e:
                QMessageBox.warning(
                    self,
                    self.lang.get('msg_error_title'),
                    f"无法创建目录 {folder_path}:\n{str(e)}"
                )
                return
        
        # 打开文件夹
        try:
            system = platform.system()
            if system == "Windows":
                os.startfile(folder_path)
            elif system == "Darwin":
                subprocess.run(["open", folder_path])
            else:
                subprocess.run(["xdg-open", folder_path])
            self.statusBar().showMessage(f"已打开: {folder_path}")
        except Exception as e:
            QMessageBox.warning(
                self,
                self.lang.get('msg_error_title'),
                self.lang.get('msg_error_folder').format(folder_path) + f"\n{str(e)}"
            )
    
    # ==================== 文件下载 ====================
    
    def on_download_requested(self, download: QWebEngineDownloadRequest):
        """处理下载请求"""
        suggested_name = download.suggestedFileName()
        if not suggested_name:
            suggested_name = "download_file"
        
        save_path = os.path.join(self.download_dir, suggested_name)
        
        # 处理重名
        counter = 1
        base, ext = os.path.splitext(save_path)
        while os.path.exists(save_path):
            save_path = f"{base}_{counter}{ext}"
            counter += 1
        
        download.setDownloadDirectory(self.download_dir)
        download.setDownloadFileName(os.path.basename(save_path))
        download.accept()
        
        download.downloadProgress.connect(
            lambda received, total: self.show_download_progress(
                received, total, suggested_name
            )
        )
        download.finished.connect(
            lambda: self.show_download_finished(suggested_name)
        )
    
    def show_download_progress(self, received: int, total: int, filename: str):
        """显示下载进度"""
        if total > 0:
            percent = int(received / total * 100)
            self.statusBar().showMessage(
                self.lang.get('status_downloading').format(filename, percent)
            )
    
    def show_download_finished(self, filename: str):
        """下载完成"""
        self.statusBar().showMessage(
            self.lang.get('status_download_finished').format(filename)
        )
        QMessageBox.information(
            self,
            self.lang.get('msg_download_finished_title'),
            self.lang.get('msg_download_finished_content').format(
                filename, self.download_dir
            )
        )


# ==================== 外部调用接口函数 ====================

def open_browser(url: str = "http://127.0.0.1:8188/", 
                 language: Optional[str] = None,
                 show_window: bool = True) -> SimpleBrowserWindow:
    """
    打开浏览器窗口（可被其他Python程序调用的接口函数）
    
    Args:
        url: 要打开的网页地址，例如 "http://127.0.0.1:8188"
        language: 语言设置，'zh' 或 'en'，None则自动检测
        show_window: 是否立即显示窗口
    
    Returns:
        SimpleBrowserWindow: 浏览器窗口实例，可用于后续控制
    """
    # 创建应用程序实例（如果不存在）
    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
        app.setApplicationName("ComfyDL UI")
    
    # 创建语言管理器
    lang_manager = LanguageManager()
    if language:
        lang_manager.set_language(language)
    
    # 创建浏览器窗口
    window = SimpleBrowserWindow(initial_url=url, lang_manager=lang_manager)
    
    if show_window:
        window.show()
    
    # 返回窗口实例，供外部控制
    return window


# ==================== 主程序入口（独立运行时） ====================

def main():
    """当脚本被直接运行时，打开一个示例浏览器"""
    import sys
    
    app = QApplication(sys.argv)
    app.setApplicationName("ComfyDL UI")
    
    # 创建语言管理器（自动检测系统语言）
    lang_manager = LanguageManager()
    
    # 创建浏览器窗口
    window = SimpleBrowserWindow(
        initial_url="http://127.0.0.1:8188",
        lang_manager=lang_manager
    )
    window.show()
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
