#pragma once
#include <vector>
#include <array>

class PerformanceMonitor {
private:
    static const int HISTORY_SIZE = 120;
    std::vector<float> fps_history;
    std::vector<float> frame_time_history;
    int history_offset;
    float current_fps;
    float current_frame_time;
    int draw_calls;
    int vertices_rendered;

public:
    PerformanceMonitor();
    void update(float deltaTime);
    void recordFrame(int drawCalls, int vertices);
    void render();
    float getCurrentFPS() const { return current_fps; }
    float getCurrentFrameTime() const { return current_frame_time; }
    int getDrawCalls() const { return draw_calls; }
    int getVerticesRendered() const { return vertices_rendered; }
};