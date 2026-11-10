import heapq
import math
import numpy as np
import json
import os
from config import CANTEEN_POSITION


class PathFinder:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.grid = np.zeros((height, width))  # 0表示可通过，1表示障碍物
        self.density_map = np.zeros((height, width))  # 人流密度图
        self.load_map_data()

    def load_map_data(self):
        """
        从文件加载地图数据
        """
        if os.path.exists("map_data.json"):
            try:
                with open("map_data.json", "r") as f:
                    data = json.load(f)
                    # 加载障碍物
                    for obs in data.get("obstacles", []):
                        x, y, size = obs
                        for dx in range(-size, size+1):
                            for dy in range(-size, size+1):
                                if dx*dx + dy*dy <= size*size:  # 圆形障碍物
                                    nx, ny = int(x + dx), int(y + dy)
                                    if 0 <= nx < self.width and 0 <= ny < self.height:
                                        self.grid[ny, nx] = 1
                print("地图数据加载成功")
            except Exception as e:
                print(f"加载地图数据时出错: {e}")

    def add_obstacle(self, x, y, width, height):
        """
        添加障碍物到地图
        """
        for i in range(max(0, y), min(self.height, y + height)):
            for j in range(max(0, x), min(self.width, x + width)):
                self.grid[i, j] = 1

    def update_density(self, people):
        """
        根据人员位置更新密度图
        """
        # 重置密度图
        self.density_map = np.zeros((self.height, self.width))
        
        # 更新人员密度
        for person in people:
            x, y = int(person.x), int(person.y)
            if 0 <= x < self.width and 0 <= y < self.height:
                # 增加人员周围区域的密度
                for dx in range(-10, 11):
                    for dy in range(-10, 11):
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < self.width and 0 <= ny < self.height:
                            # 距离越近，密度越高
                            distance = max(0.1, math.sqrt(dx*dx + dy*dy))
                            self.density_map[ny, nx] += 1.0 / (distance + 1)

    def heuristic(self, a, b):
        """
        启发式函数，使用欧几里得距离
        """
        return math.sqrt((a[0] - b[0])**2 + (a[1] - b[1])**2)

    def get_neighbors(self, x, y):
        """
        获取相邻节点
        """
        neighbors = []
        for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)]:
            nx, ny = x + dx, y + dy
            if 0 <= nx < self.width and 0 <= ny < self.height and self.grid[ny, nx] == 0:
                # 对角线移动成本更高
                cost = 1.0 if dx == 0 or dy == 0 else 1.414
                neighbors.append((nx, ny, cost))
        return neighbors

    def get_cost(self, x, y):
        """
        获取通过某点的代价，考虑人流密度
        """
        if 0 <= x < self.width and 0 <= y < self.height:
            # 基础代价 + 密度代价
            return 1.0 + self.density_map[y, x] * 5
        return float('inf')

    def find_path(self, start, goal):
        """
        使用A*算法寻找最优路径
        """
        # 优先队列：(优先级, 坐标)
        frontier = [(0, start)]
        came_from = {}
        cost_so_far = {start: 0}
        
        while frontier:
            current_priority, current = heapq.heappop(frontier)
            
            if current == goal:
                break
                
            for next_point in self.get_neighbors(current[0], current[1]):
                nx, ny, move_cost = next_point
                new_cost = cost_so_far[current] + move_cost + self.get_cost(nx, ny)
                
                next_pos = (nx, ny)
                if next_pos not in cost_so_far or new_cost < cost_so_far[next_pos]:
                    cost_so_far[next_pos] = new_cost
                    priority = new_cost + self.heuristic(next_pos, goal)
                    heapq.heappush(frontier, (priority, next_pos))
                    came_from[next_pos] = current
        
        # 重构路径
        current = goal
        path = []
        while current in came_from:
            path.append(current)
            current = came_from[current]
        path.append(start)
        path.reverse()
        
        return path

    def get_optimal_path(self, person_x, person_y, canteen_x, canteen_y):
        """
        获取指定人员到食堂的最优路径
        """
        start = (int(person_x), int(person_y))
        goal = (int(canteen_x), int(canteen_y))
        
        # 如果起点或终点不可达，调整到最近的可达点
        if self.grid[start[1], start[0]] == 1:
            # 找到最近的可通过点
            start = self.find_nearest_passable(start[0], start[1])
            
        if self.grid[goal[1], goal[0]] == 1:
            goal = self.find_nearest_passable(goal[0], goal[1])
            
        return self.find_path(start, goal)

    def find_nearest_passable(self, x, y):
        """
        找到最近的可通过点
        """
        queue = [(x, y)]
        visited = set()
        visited.add((x, y))
        
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
        
        while queue:
            current_x, current_y = queue.pop(0)
            
            if 0 <= current_x < self.width and 0 <= current_y < self.height:
                if self.grid[current_y, current_x] == 0:
                    return (current_x, current_y)
                    
                for dx, dy in directions:
                    nx, ny = current_x + dx, current_y + dy
                    if (nx, ny) not in visited:
                        visited.add((nx, ny))
                        queue.append((nx, ny))
        
        # 如果找不到，返回原始点
        return (x, y)

    def get_spawn_points(self):
        """
        获取出生点列表
        """
        spawn_points = []
        if os.path.exists("map_data.json"):
            try:
                with open("map_data.json", "r") as f:
                    data = json.load(f)
                    spawn_points = data.get("spawn_points", [])
            except:
                pass
        return spawn_points