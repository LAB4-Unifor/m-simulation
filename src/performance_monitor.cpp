#include "performance_monitor.h"
#include <algorithm>

PerformanceMonitor::PerformanceMonitor() 
    : history_offset(0),
      current_fps(0.0f),
      current_frame_time(0.0f),
      draw_calls(0),
      vertices_rendered(0) {
    
    // Initialize history with zeros
    fps_history.assign(HISTORY_SIZE, 0.0f);
    frame_time_history.assign(HISTORY_SIZE, 0.0f);
}

void PerformanceMonitor::update(float deltaTime) {
    // Update current frame time
    current_frame_time = deltaTime * 1000.0f; // Convert to milliseconds
    
    // Calculate current FPS
    current_fps = 1.0f / (deltaTime + 1e-6f);
    
    // Add to history
    fps_history[history_offset] = current_fps;
    frame_time_history[history_offset] = current_frame_time;
    history_offset = (history_offset + 1) % HISTORY_SIZE;
}

void PerformanceMonitor::recordFrame(int drawCalls, int vertices) {
    this->draw_calls = drawCalls;
    this->vertices_rendered = vertices;
}