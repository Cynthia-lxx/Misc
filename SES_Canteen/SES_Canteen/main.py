import pygame
import random
import sys
from config import *
from person import Person
from pathfinder import PathFinder


class CanteenRushSimulator:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("食堂抢饭模拟器")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 36)
        
        # 创建人员列表
        self.people = []
        self.person_id_counter = 0
        
        # 创建路径查找器
        self.pathfinder = PathFinder(SCREEN_WIDTH, SCREEN_HEIGHT)
        
        # 食堂位置
        self.canteen_position = CANTEEN_POSITION
        self.canteen_rect = pygame.Rect(
            self.canteen_position[0] - 30, 
            self.canteen_position[1] - 30, 
            60, 
            60
        )
        
        # 统计信息
        self.start_time = pygame.time.get_ticks()
        self.arrived_count = 0


    def add_person(self, x=None, y=None):
        """
        添加一个人到模拟中
        """
        if x is None or y is None:
            # 尝试从预设的出生点添加人员
            spawn_points = self.pathfinder.get_spawn_points()
            if spawn_points:
                x, y = random.choice(spawn_points)
            else:
                # 随机选择一个不是障碍物的位置
                while True:
                    x = random.randint(50, SCREEN_WIDTH - 50)
                    y = random.randint(50, SCREEN_HEIGHT - 50)
                    if self.pathfinder.grid[y, x] == 0:
                        break
                    
        person = Person(x, y, self.person_id_counter)
        self.person_id_counter += 1
        self.people.append(person)

    def spawn_people(self, count):
        """
        生成指定数量的人
        """
        for _ in range(count):
            self.add_person()

    def update(self):
        """
        更新模拟状态
        """
        # 更新人员位置
        for person in self.people:
            person.update(self.people, self.canteen_position, self.pathfinder.grid)
            
        # 更新密度图
        self.pathfinder.update_density(self.people)
        
        # 更新统计信息
        self.arrived_count = sum(1 for person in self.people if person.arrived)

    def draw(self):
        """
        绘制所有元素
        """
        self.screen.fill(WHITE)
        
        # 绘制障碍物
        for y in range(self.pathfinder.height):
            for x in range(self.pathfinder.width):
                if self.pathfinder.grid[y, x] == 1:
                    pygame.draw.rect(self.screen, (100, 100, 100), (x, y, 1, 1))
        
        # 绘制食堂
        pygame.draw.rect(self.screen, GREEN, self.canteen_rect)
        canteen_text = pygame.font.Font(None, 24).render("食堂", True, BLACK)
        self.screen.blit(canteen_text, (self.canteen_position[0]-20, self.canteen_position[1]-10))
        
        # 绘制人员
        for person in self.people:
            person.draw(self.screen)
            
        # 显示统计信息
        elapsed_time = (pygame.time.get_ticks() - self.start_time) / 1000
        info_text = f"人员总数: {len(self.people)}  已到达: {self.arrived_count}  时间: {elapsed_time:.1f}s"
        text_surface = self.font.render(info_text, True, BLACK)
        self.screen.blit(text_surface, (10, 10))
        
        # 显示操作说明
        instruction_text = "按 SPACE 添加10个人, C 清除所有人, E 打开地图编辑器, ESC 退出"
        instruction_surface = pygame.font.Font(None, 24).render(instruction_text, True, BLACK)
        self.screen.blit(instruction_surface, (10, SCREEN_HEIGHT - 30))

    def run(self):
        """
        运行主循环
        """
        self.spawn_people(20)  # 初始生成20个人
        
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        running = False
                    elif event.key == pygame.K_SPACE:
                        self.spawn_people(10)  # 按空格键添加10个人
                    elif event.key == pygame.K_c:
                        self.people.clear()  # 清除所有人
                        self.arrived_count = 0
                        self.start_time = pygame.time.get_ticks()
                    elif event.key == pygame.K_e:
                        self.run_map_editor()  # 打开地图编辑器
                        
            self.update()
            self.draw()
            
            pygame.display.flip()
            self.clock.tick(FPS)
            
        pygame.quit()
        sys.exit()

    def run_map_editor(self):
        """
        运行地图编辑器
        """
        # 我们需要临时关闭主模拟器窗口
        pygame.display.quit()
        
        # 导入并运行地图编辑器
        try:
            from map_editor import MapEditor
            editor = MapEditor()
            editor.run()
            
            # 重新初始化显示
            pygame.display.init()
            self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
            pygame.display.set_caption("食堂抢饭模拟器")
            
            # 重新加载路径查找器以获取新地图数据
            self.pathfinder = PathFinder(SCREEN_WIDTH, SCREEN_HEIGHT)
        except ImportError:
            print("无法导入地图编辑器")
            pygame.display.init()
            self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
            pygame.display.set_caption("食堂抢饭模拟器")


if __name__ == "__main__":
    simulator = CanteenRushSimulator()
    simulator.run()