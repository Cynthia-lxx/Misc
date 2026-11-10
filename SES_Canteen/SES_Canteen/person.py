import pygame
import random
import math
import numpy as np
from config import PERSON_RADIUS, PERSON_SPEED, CANTEEN_POSITION


class Person:
    def __init__(self, x, y, person_id):
        self.id = person_id
        self.x = x
        self.y = y
        self.speed = PERSON_SPEED + random.uniform(-0.5, 0.5)  # 个体速度差异
        self.radius = PERSON_RADIUS
        self.color = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        self.path = []  # 路径点
        self.target = CANTEEN_POSITION
        self.arrived = False
        self.velocity = [0, 0]
        self.acceleration = 0.2
        self.max_speed = self.speed
        self.direction = random.uniform(0, 2 * math.pi)  # 初始方向
        self.target_direction = self.direction
        self.turn_speed = 0.1  # 转向速度

    def update(self, people, canteen_entrance, obstacles_grid=None):
        """
        更新人员位置
        """
        if self.arrived:
            return

        # 检查是否到达食堂
        distance_to_canteen = math.sqrt((self.x - canteen_entrance[0])**2 + (self.y - canteen_entrance[1])**2)
        if distance_to_canteen < self.radius + 10:  # 10是食堂入口的近似半径
            self.arrived = True
            return

        # 计算到目标的方向
        dx = canteen_entrance[0] - self.x
        dy = canteen_entrance[1] - self.y
        self.target_direction = math.atan2(dy, dx)

        # 平滑转向
        angle_diff = self.normalize_angle(self.target_direction - self.direction)
        if abs(angle_diff) > 0.01:  # 如果角度差较大，则进行转向
            turn_amount = min(self.turn_speed, abs(angle_diff))
            if angle_diff > 0:
                self.direction += turn_amount
            else:
                self.direction -= turn_amount

        # 添加一些自然的移动变化（但不会偏离太多）
        random_turn = random.uniform(-0.05, 0.05)
        self.direction += random_turn

        # 避免墙壁碰撞
        if obstacles_grid is not None:
            self.avoid_walls(obstacles_grid)

        # 避免与其他人员碰撞
        self.avoid_collisions(people)

        # 更新速度
        self.velocity[0] = math.cos(self.direction) * self.speed
        self.velocity[1] = math.sin(self.direction) * self.speed

        # 更新位置
        self.x += self.velocity[0]
        self.y += self.velocity[1]

        # 边界检查
        if self.x < self.radius:
            self.x = self.radius
            self.direction = math.pi - self.direction
        elif self.x > 1000 - self.radius:
            self.x = 1000 - self.radius
            self.direction = math.pi - self.direction

        if self.y < self.radius:
            self.y = self.radius
            self.direction = -self.direction
        elif self.y > 800 - self.radius:
            self.y = 800 - self.radius
            self.direction = -self.direction

        # 记录路径
        self.path.append((self.x, self.y))
        if len(self.path) > 50:  # 限制路径长度
            self.path.pop(0)

    def normalize_angle(self, angle):
        """
        将角度标准化到 [-π, π] 范围内
        """
        while angle > math.pi:
            angle -= 2 * math.pi
        while angle < -math.pi:
            angle += 2 * math.pi
        return angle

    def avoid_walls(self, obstacles_grid):
        """
        避免与墙壁碰撞
        """
        grid_x = int(self.x)
        grid_y = int(self.y)
        
        # 检查周围的障碍物
        for dx in range(-20, 21, 10):
            for dy in range(-20, 21, 10):
                check_x = grid_x + dx
                check_y = grid_y + dy
                
                if (0 <= check_x < obstacles_grid.shape[1] and 
                    0 <= check_y < obstacles_grid.shape[0] and 
                    obstacles_grid[check_y, check_x] == 1):
                    
                    # 计算远离障碍物的方向
                    away_dx = self.x - check_x
                    away_dy = self.y - check_y
                    distance = max(0.1, math.sqrt(away_dx**2 + away_dy**2))
                    
                    # 根据距离调整转向力度
                    avoidance_strength = 1.0 / distance
                    avoidance_angle = math.atan2(away_dy, away_dx)
                    
                    # 影响当前方向
                    self.direction = (
                        self.direction * 0.7 + 
                        avoidance_angle * 0.3 * avoidance_strength
                    )

    def avoid_collisions(self, people):
        """
        避免与其他人员碰撞
        """
        for other in people:
            if other.id == self.id:
                continue

            dx = self.x - other.x
            dy = self.y - other.y
            distance = math.sqrt(dx*dx + dy*dy)

            # 如果太近，尝试避开
            if distance < self.radius * 4 and distance > 0:
                # 计算避开的角度
                avoidance_angle = math.atan2(dy, dx)
                
                # 根据距离调整避让强度
                strength = (self.radius * 4 - distance) / (self.radius * 4)
                
                # 影响方向
                self.direction += strength * 0.1
                
                # 如果非常接近，降低速度
                if distance < self.radius * 2:
                    self.speed *= 0.95

    def draw(self, screen):
        """
        绘制人员
        """
        pygame.draw.circle(screen, self.color, (int(self.x), int(self.y)), self.radius)
        
        # 绘制朝向指示器
        end_x = self.x + math.cos(self.direction) * (self.radius + 2)
        end_y = self.y + math.sin(self.direction) * (self.radius + 2)
        pygame.draw.line(screen, (0, 0, 0), (self.x, self.y), (end_x, end_y), 2)
        
        # 绘制路径
        if len(self.path) > 1:
            for i in range(1, len(self.path)):
                alpha = i / len(self.path)
                color = (int(self.color[0] * alpha), 
                         int(self.color[1] * alpha), 
                         int(self.color[2] * alpha))
                pygame.draw.line(screen, color, self.path[i-1], self.path[i], 1)