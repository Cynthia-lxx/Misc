"""
Troll-vs-Troll Project
UNIHIKER M10 Benchmark Demo

This module serves as a comprehensive benchmark for the UNIHIKER M10 board,
testing all onboard sensors, display components, and measuring computational performance.
Features a page-based UI to navigate through different sensor readings and performance metrics.

Version: 1.3.2
"""

import time
import math
from unihiker import GUI, Audio
from pinpong.extension.unihiker import *

# TODO: Implement benchmark demo for UNIHIKER M10 - HIGH - Developer
# TODO: Test all onboard sensors (accelerometer, light sensor, etc.) - HIGH - Developer
# TODO: Test display components with page navigation - HIGH - Developer
# TODO: Measure computational performance (loop speed) - HIGH - Developer
# TODO: Implement page navigation system for limited screen space - MEDIUM - Developer


class BenchmarkDemo:
    """
    A comprehensive benchmark demo for UNIHIKER M10 board.
    Tests onboard sensors, display components, and computational performance.
    """
    
    def __init__(self):
        """
        Initialize the benchmark demo.
        """
        self.gui = GUI()
        self.audio = Audio()
        self.page_index = 0
        self.pages = [
            self.page_home,
            self.page_accelerometer,
            self.page_light_sensor,
            self.page_display,
            self.page_performance,
            self.page_audio,
            self.page_compass
        ]
        self.page_names = [
            "Home",
            "Accelerometer",
            "Light Sensor", 
            "Display Test",
            "Performance",
            "Audio Test",
            "Compass"
        ]
        
        # For performance testing
        self.loop_counter = 0
        self.start_time = time.time()
        self.fps_start_time = time.time()
        self.fps_counter = 0
        self.fps = 0
        
        # For sensor readings
        self.accel_data = (0, 0, 0)
        self.light_value = 0
        self.compass_value = 0

    def page_home(self):
        """
        Home page showing an overview of the benchmark.
        """
        self.gui.draw_text(x=120, y=50, text="Troll-vs-Troll", origin='center', font_size=16, color=(0, 255, 0))
        self.gui.draw_text(x=120, y=80, text="UNIHIKER M10 Benchmark", origin='center', font_size=14, color=(0, 200, 200))
        self.gui.draw_text(x=120, y=120, text=f"Page: {self.page_index + 1}/{len(self.pages)}", origin='center', font_size=12, color=(200, 200, 0))
        self.gui.draw_text(x=120, y=140, text=self.page_names[self.page_index], origin='center', font_size=14, color=(255, 255, 255))
        self.gui.draw_text(x=120, y=180, text="Press A/B to navigate", origin='center', font_size=12, color=(150, 150, 150))
        self.gui.draw_text(x=120, y=200, text="Press Home to exit", origin='center', font_size=12, color=(150, 150, 150))
        self.gui.draw_text(x=120, y=240, text=f"FPS: {self.fps:.1f}", origin='center', font_size=12, color=(255, 100, 100))

    def page_accelerometer(self):
        """
        Page showing accelerometer data.
        """
        try:
            # Read accelerometer data using pinpong
            from pinpong.extension.unihiker import acceleration
            self.accel_data = acceleration.read()
            
            self.gui.draw_text(x=120, y=30, text="Accelerometer Data", origin='center', font_size=16, color=(0, 255, 0))
            self.gui.draw_text(x=120, y=60, text=f"X: {self.accel_data[0]:.3f} g", origin='center', font_size=14, color=(200, 200, 200))
            self.gui.draw_text(x=120, y=85, text=f"Y: {self.accel_data[1]:.3f} g", origin='center', font_size=14, color=(200, 200, 200))
            self.gui.draw_text(x=120, y=110, text=f"Z: {self.accel_data[2]:.3f} g", origin='center', font_size=14, color=(200, 200, 200))
            
            # Draw a simple visualization
            x_pos = 120 + int(self.accel_data[0] * 50)
            y_pos = 180 + int(self.accel_data[1] * 50)
            
            # Draw reference cross using lines made of text elements
            for i in range(0, 200, 4):  # Horizontal line
                self.gui.draw_text(x=20+i, y=180, text="┉", origin='center', font_size=6, color=(100, 100, 100))
            for i in range(0, 200, 4):  # Vertical line
                self.gui.draw_text(x=120, y=80+i, text="┋", origin='center', font_size=6, color=(100, 100, 100))
            
            # Draw accelerometer ball
            self.gui.draw_text(x=x_pos, y=y_pos, text="●", origin='center', font_size=16, color=(0, 255, 255))
            
        except Exception as e:
            self.gui.draw_text(x=120, y=30, text="Accelerometer Data", origin='center', font_size=16, color=(0, 255, 0))
            self.gui.draw_text(x=120, y=80, text="Error reading accelerometer", origin='center', font_size=12, color=(255, 0, 0))
            self.gui.draw_text(x=120, y=100, text=str(e), origin='center', font_size=10, color=(255, 0, 0))

    def page_light_sensor(self):
        """
        Page showing light sensor data.
        """
        try:
            # Read light sensor data
            from pinpong.extension.unihiker import light
            self.light_value = light.read()
            
            self.gui.draw_text(x=120, y=30, text="Light Sensor", origin='center', font_size=16, color=(0, 255, 0))
            self.gui.draw_text(x=120, y=80, text=f"Light Level: {self.light_value}", origin='center', font_size=16, color=(200, 200, 200))
            
            # Draw a light indicator using text elements instead of rectangles
            bar_width = min(200, max(20, int(self.light_value / 1023 * 200)))
            # Using a series of characters to simulate a filled bar
            bar_chars = int(bar_width / 8)
            bar_text = "█" * bar_chars
            self.gui.draw_text(x=20 + bar_width//2, y=150, text=bar_text, origin='center', font_size=16, color=(255, 255, 0))
            
            # Draw outline for the bar using small text elements
            for i in range(0, 200, 8):  # Top border
                self.gui.draw_text(x=20+i, y=140, text="━", origin='center', font_size=10, color=(100, 100, 100))
            for i in range(0, 200, 8):  # Bottom border
                self.gui.draw_text(x=20+i, y=160, text="━", origin='center', font_size=10, color=(100, 100, 100))
            for i in range(0, 20, 2):  # Left border
                self.gui.draw_text(x=20, y=140+i, text="┃", origin='center', font_size=10, color=(100, 100, 100))
            for i in range(0, 20, 2):  # Right border
                self.gui.draw_text(x=220, y=140+i, text="┃", origin='center', font_size=10, color=(100, 100, 100))
            
            # Draw percentage
            percentage = int(self.light_value / 1023 * 100)
            self.gui.draw_text(x=120, y=180, text=f"{percentage}%", origin='center', font_size=14, color=(255, 255, 255))
            
        except Exception as e:
            self.gui.draw_text(x=120, y=30, text="Light Sensor", origin='center', font_size=16, color=(0, 255, 0))
            self.gui.draw_text(x=120, y=80, text="Error reading light sensor", origin='center', font_size=12, color=(255, 0, 0))
            self.gui.draw_text(x=120, y=100, text=str(e), origin='center', font_size=10, color=(255, 0, 0))

    def page_display(self):
        """
        Page testing display components.
        """
        self.gui.draw_text(x=120, y=30, text="Display Test", origin='center', font_size=16, color=(0, 255, 0))
        
        # Draw various shapes using text elements
        # Red circle substitute
        self.gui.draw_text(x=60, y=80, text="●", origin='center', font_size=24, color=(255, 0, 0))
        # Green square substitute  
        self.gui.draw_text(x=120, y=80, text="■", origin='center', font_size=24, color=(0, 255, 0))
        # Blue line substitute (using multiple text elements)
        for i in range(5):
            x = 160 + i * 8
            y = 60 + i * 8
            self.gui.draw_text(x=x, y=y, text="•", origin='center', font_size=10, color=(0, 0, 255))
        
        # Draw different text sizes and colors
        self.gui.draw_text(x=120, y=130, text="Small Text", origin='center', font_size=10, color=(255, 255, 0))
        self.gui.draw_text(x=120, y=150, text="Medium Text", origin='center', font_size=14, color=(255, 0, 255))
        self.gui.draw_text(x=120, y=170, text="Large Text", origin='center', font_size=18, color=(0, 255, 255))
        
        # Draw a simple animation indicator (moving dot)
        animation_x = 20 + ((time.time() * 100) % 200)
        self.gui.draw_text(x=int(animation_x), y=220, text="●", origin='center', font_size=10, color=(255, 255, 255))
        
        self.gui.draw_text(x=120, y=250, text="Animation Test", origin='center', font_size=12, color=(200, 200, 200))

    def page_performance(self):
        """
        Page showing computational performance metrics.
        """
        # Calculate FPS
        current_time = time.time()
        self.fps_counter += 1
        if current_time - self.fps_start_time >= 1.0:
            self.fps = self.fps_counter / (current_time - self.fps_start_time)
            self.fps_counter = 0
            self.fps_start_time = current_time
        
        self.gui.draw_text(x=120, y=30, text="Performance Test", origin='center', font_size=16, color=(0, 255, 0))
        self.gui.draw_text(x=120, y=70, text=f"Current FPS: {self.fps:.1f}", origin='center', font_size=14, color=(200, 200, 200))
        
        # Calculate loop speed (how many loops per second)
        self.loop_counter += 1
        elapsed = current_time - self.start_time
        if elapsed > 0:
            loops_per_sec = self.loop_counter / elapsed
            self.gui.draw_text(x=120, y=100, text=f"Loops/sec: {loops_per_sec:.1f}", origin='center', font_size=14, color=(200, 200, 200))
        
        # Performance stress test
        # Perform some calculations to test CPU
        start_calc = time.time()
        result = sum(math.sin(i * 0.1) for i in range(1000))
        calc_time = (time.time() - start_calc) * 1000  # in ms
        self.gui.draw_text(x=120, y=130, text=f"Calc Time: {calc_time:.2f}ms", origin='center', font_size=14, color=(200, 200, 200))
        
        # Draw performance bar using text elements
        calc_bar_width = max(0, min(200, int((10 - calc_time) * 20))) if calc_time < 10 else 200
        bar_chars = int(calc_bar_width / 8)
        bar_text = "█" * bar_chars
        self.gui.draw_text(x=20 + calc_bar_width//2, y=177, text=bar_text, origin='center', font_size=16, color=(0, 255, 0))
        
        # Draw outline for the bar
        for i in range(0, 200, 8):  # Top border
            self.gui.draw_text(x=20+i, y=170, text="━", origin='center', font_size=10, color=(100, 100, 100))
        for i in range(0, 200, 8):  # Bottom border
            self.gui.draw_text(x=20+i, y=185, text="━", origin='center', font_size=10, color=(100, 100, 100))
        for i in range(0, 15, 2):  # Left border
            self.gui.draw_text(x=20, y=170+i, text="┃", origin='center', font_size=10, color=(100, 100, 100))
        for i in range(0, 15, 2):  # Right border
            self.gui.draw_text(x=220, y=170+i, text="┃", origin='center', font_size=10, color=(100, 100, 100))
        
        self.gui.draw_text(x=120, y=190, text="Performance (higher is better)", origin='center', font_size=10, color=(200, 200, 200))
        
        # Memory usage would require psutil or similar, which might not be available on UNIHIKER
        self.gui.draw_text(x=120, y=230, text="CPU Performance Test", origin='center', font_size=12, color=(200, 200, 200))

    def page_audio(self):
        """
        Page testing audio components.
        """
        self.gui.draw_text(x=120, y=30, text="Audio Test", origin='center', font_size=16, color=(0, 255, 0))
        self.gui.draw_text(x=120, y=70, text="Audio system initialized", origin='center', font_size=14, color=(200, 200, 200))
        self.gui.draw_text(x=120, y=100, text="Press A to play test tone", origin='center', font_size=12, color=(200, 200, 200))
        self.gui.draw_text(x=120, y=120, text="Press B to stop playback", origin='center', font_size=12, color=(200, 200, 200))
        
        # Draw a simple speaker icon using text - replacing problematic Unicode
        self.gui.draw_text(x=120, y=180, text="[SND]", origin='center', font_size=24, color=(200, 200, 200))
        self.gui.draw_text(x=120, y=200, text="Speaker", origin='center', font_size=12, color=(200, 200, 200))

    def page_compass(self):
        """
        Page showing compass/magnetometer data if available.
        """
        self.gui.draw_text(x=120, y=30, text="Compass/Magnetic", origin='center', font_size=16, color=(0, 255, 0))
        self.gui.draw_text(x=120, y=80, text="Compass functionality", origin='center', font_size=14, color=(200, 200, 200))
        self.gui.draw_text(x=120, y=100, text="would be tested here", origin='center', font_size=14, color=(200, 200, 200))
        
        # Draw a simple compass using text elements
        center_x, center_y = 120, 180
        # Draw compass circle
        self.gui.draw_text(x=center_x, y=center_y, text="○", origin='center', font_size=48, color=(200, 200, 200))
        
        # Draw N, S, E, W
        self.gui.draw_text(x=center_x, y=center_y-50, text="N", origin='center', font_size=12, color=(255, 0, 0))
        self.gui.draw_text(x=center_x, y=center_y+50, text="S", origin='center', font_size=12, color=(255, 0, 0))
        self.gui.draw_text(x=center_x-50, y=center_y, text="W", origin='center', font_size=12, color=(255, 0, 0))
        self.gui.draw_text(x=center_x+50, y=center_y, text="E", origin='center', font_size=12, color=(255, 0, 0))
        
        # Draw a simple needle (static for now, would be dynamic with real compass)
        angle = math.radians(self.compass_value)
        needle_x = center_x + 30 * math.sin(angle)
        needle_y = center_y - 30 * math.cos(angle)
        self.gui.draw_text(x=needle_x, y=needle_y, text="●", origin='center', font_size=10, color=(255, 0, 0))

    def navigate_to_next_page(self):
        """
        Navigate to the next page.
        """
        self.page_index = (self.page_index + 1) % len(self.pages)
        
    def navigate_to_prev_page(self):
        """
        Navigate to the previous page.
        """
        self.page_index = (self.page_index - 1) % len(self.pages)

    def run(self):
        """
        Run the benchmark demo main loop.
        """
        # Set up button callbacks
        self.gui.on_a_click(self.navigate_to_next_page)
        self.gui.on_b_click(self.navigate_to_prev_page)
        
        # Main loop
        while True:
            # Clear screen by drawing a black background
            # Use many small elements to densely cover the screen
            # Grid of elements covering the whole screen
            for y in range(0, 320, 10):  # Every 10 pixels vertically
                for x in range(0, 240, 10):  # Every 10 pixels horizontally
                    self.gui.draw_text(x=x+5, y=y+5, text=" ", origin='center', font_size=16, color=(0, 0, 0))
            
            # Run the current page function
            self.pages[self.page_index]()
            
            # Update FPS counter
            self.fps_counter += 1
            current_time = time.time()
            if current_time - self.fps_start_time >= 1.0:
                self.fps = self.fps_counter / (current_time - self.fps_start_time)
                self.fps_counter = 0
                self.fps_start_time = current_time
            
            # Removed delay to allow maximum performance testing


def main():
    """
    Main function to run the benchmark demo.
    """
    print("Starting UNIHIKER M10 Benchmark Demo...")
    
    benchmark = BenchmarkDemo()
    benchmark.run()


if __name__ == "__main__":
    main()