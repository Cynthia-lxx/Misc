import pygame
import json
import os
from config import *


class MapEditor:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("地图编辑器")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 32)
        
        # 编辑模式
        self.mode = "obstacle"  # obstacle, spawn, erase
        self.brush_size = 10
        
        # 地图数据
        self.obstacles = []
        self.spawn_points = []
        
        # 加载现有地图
        self.load_map()
        
        # UI按钮
        self.buttons = {
            "obstacle": pygame.Rect(10, 10, 100, 40),
            "spawn": pygame.Rect(120, 10, 100, 40),
            "erase": pygame.Rect(230, 10, 100, 40),
            "save": pygame.Rect(340, 10, 100, 40),
            "load": pygame.Rect(450, 10, 100, 40),
            "clear": pygame.Rect(560, 10, 100, 40)
        }

    def load_map(self):
        """
        从文件加载地图数据
        """
        if os.path.exists("map_data.json"):
            try:
                with open("map_data.json", "r") as f:
                    data = json.load(f)
                    self.obstacles = data.get("obstacles", [])
                    self.spawn_points = data.get("spawn_points", [])
            except:
                print("无法加载地图数据")

    def save_map(self):
        """
        保存地图数据到文件
        """
        data = {
            "obstacles": self.obstacles,
            "spawn_points": self.spawn_points
        }
        with open("map_data.json", "w") as f:
            json.dump(data, f)
        print("地图已保存")

    def clear_map(self):
        """
        清空地图
        """
        self.obstacles.clear()
        self.spawn_points.clear()

    def handle_events(self):
        """
        处理事件
        """
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # 左键
                    self.handle_left_click(event.pos)
                elif event.button == 3:  # 右键
                    self.handle_right_click(event.pos)
                elif event.button == 4:  # 滚轮向上
                    self.brush_size = min(50, self.brush_size + 1)
                elif event.button == 5:  # 滚轮向下
                    self.brush_size = max(1, self.brush_size - 1)
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    return False
                elif event.key == pygame.K_o:
                    self.mode = "obstacle"
                elif event.key == pygame.K_s:
                    self.mode = "spawn"
                elif event.key == pygame.K_e:
                    self.mode = "erase"
        return True

    def handle_left_click(self, pos):
        """
        处理左键点击
        """
        x, y = pos
        
        # 检查按钮点击
        for mode, button in self.buttons.items():
            if button.collidepoint(x, y):
                if mode in ["obstacle", "spawn", "erase"]:
                    self.mode = mode
                elif mode == "save":
                    self.save_map()
                elif mode == "load":
                    self.load_map()
                elif mode == "clear":
                    self.clear_map()
                return
        
        # 在地图上绘制
        if self.mode == "obstacle":
            self.obstacles.append((x, y, self.brush_size))
        elif self.mode == "spawn":
            self.spawn_points.append((x, y))
        elif self.mode == "erase":
            # 删除附近的障碍物
            self.obstacles = [obs for obs in self.obstacles 
                             if not (abs(obs[0] - x) < obs[2] and abs(obs[1] - y) < obs[2])]
            # 删除附近的出生点
            self.spawn_points = [sp for sp in self.spawn_points 
                                if not (abs(sp[0] - x) < 10 and abs(sp[1] - y) < 10)]

    def handle_right_click(self, pos):
        """
        处理右键点击（添加较大的障碍物）
        """
        x, y = pos
        if self.mode == "obstacle":
            self.obstacles.append((x, y, self.brush_size * 2))

    def draw_ui(self):
        """
        绘制UI界面
        """
        # 绘制按钮
        modes = ["obstacle", "spawn", "erase"]
        actions = ["save", "load", "clear"]
        
        for mode in modes:
            color = GREEN if self.mode == mode else GRAY
            pygame.draw.rect(self.screen, color, self.buttons[mode])
            text = self.font.render(mode.capitalize(), True, BLACK)
            text_rect = text.get_rect(center=self.buttons[mode].center)
            self.screen.blit(text, text_rect)
            
        for action in actions:
            pygame.draw.rect(self.screen, BLUE, self.buttons[action])
            text = self.font.render(action.capitalize(), True, WHITE)
            text_rect = text.get_rect(center=self.buttons[action].center)
            self.screen.blit(text, text_rect)
            
        # 显示当前模式和画笔大小
        info_text = f"Mode: {self.mode.capitalize()} | Brush Size: {self.brush_size}"
        text = self.font.render(info_text, True, BLACK)
        self.screen.blit(text, (10, 60))
        
        # 显示操作说明
        instructions = [
            "O: 障碍物模式, S: 出生点模式, E: 擦除模式",
            "左键: 绘制, 右键: 绘制大障碍物, 滚轮: 调整画笔大小",
            "ESC: 退出编辑器"
        ]
        
        for i, instruction in enumerate(instructions):
            text = pygame.font.Font(None, 24).render(instruction, True, BLACK)
            self.screen.blit(text, (10, SCREEN_HEIGHT - 80 + i * 25))

    def draw_map(self):
        """
        绘制地图元素
        """
        # 绘制障碍物
        for x, y, size in self.obstacles:
            pygame.draw.circle(self.screen, BLACK, (x, y), size)
            
        # 绘制出生点
        for x, y in self.spawn_points:
            pygame.draw.circle(self.screen, GREEN, (x, y), 5)
            
        # 绘制光标
        mouse_pos = pygame.mouse.get_pos()
        if self.mode == "obstacle":
            pygame.draw.circle(self.screen, RED, mouse_pos, self.brush_size, 1)
        elif self.mode == "spawn":
            pygame.draw.circle(self.screen, GREEN, mouse_pos, 5, 1)
        elif self.mode == "erase":
            pygame.draw.circle(self.screen, BLUE, mouse_pos, self.brush_size, 1)

    def run(self):
        """
        运行编辑器主循环
        """
        running = True
        while running:
            running = self.handle_events()
            
            self.screen.fill(WHITE)
            self.draw_map()
            self.draw_ui()
            
            pygame.display.flip()
            self.clock.tick(FPS)
            
        pygame.quit()
        return self.obstacles, self.spawn_points


if __name__ == "__main__":
    # 定义灰色
    GRAY = (128, 128, 128)
    
    editor = MapEditor()
    editor.run()