#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <vector>
#include <string>
#include <chrono>
#include <GL/glew.h>

// Change to local include
#include "imgui.h"
#include "implot.h"

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void update(float deltaTime);
    void renderGUI();
    
    void beginFrame();
    void endFrame();
    
    void beginGPUTimer(const std::string& name);
    void endGPUTimer(const std::string& name);
    
    void trackMemoryUsage();
    
    void incrementDrawCalls() { drawCalls++; }
    void addVerticesRendered(int count) { verticesRendered += count; }
    void addTrianglesRendered(int count) { trianglesRendered += count; }
    
    float getCurrentFPS() const { return currentFPS; }
    float getCurrentFrameTime() const { return currentFrameTime; }
    size_t getCurrentMemoryUsage() const { return currentMemoryUsage; }
    int getCurrentDrawCalls() const { return drawCalls; }
    int getCurrentVerticesRendered() const { return verticesRendered; }
    int getCurrentTrianglesRendered() const { return trianglesRendered; }
    
    void setHistorySize(int size) { historySize = size; }
    void setGPUTimingEnabled(bool enabled) { gpuTimingEnabled = enabled; }
    
private:
    std::chrono::high_resolution_clock::time_point frameStartTime;
    float currentFPS;
    float currentFrameTime;
    std::vector<float> fpsHistory;
    std::vector<float> frameTimeHistory;
    int historySize;
    
    size_t currentMemoryUsage;
    std::vector<size_t> memoryHistory;
    
    int drawCalls;
    int verticesRendered;
    int trianglesRendered;
    std::vector<int> drawCallsHistory;
    std::vector<int> verticesHistory;
    std::vector<int> trianglesHistory;
    
    bool gpuTimingEnabled;
    struct GPUTimer {
        GLuint queryStart;
        GLuint queryEnd;
        bool started;
        bool ended;
        std::string name;
        float durationMs;
    };
    
    std::vector<GPUTimer> gpuTimers;
    
    void updateHistories();
    void updateGPUTimers();
    GPUTimer* getGPUTimer(const std::string& name);
    size_t getCurrentProcessMemoryUsage();
};

#endif