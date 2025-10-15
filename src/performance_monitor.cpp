#include "performance_monitor.h"
#include <iostream>
#include <imgui.h>
#include <implot.h>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <cstdio>
#endif

PerformanceMonitor::PerformanceMonitor() 
    : currentFPS(0.0f),
      currentFrameTime(0.0f),
      historySize(300),
      currentMemoryUsage(0),
      drawCalls(0),
      verticesRendered(0),
      trianglesRendered(0),
      gpuTimingEnabled(false) {
    
    fpsHistory.resize(historySize, 0.0f);
    frameTimeHistory.resize(historySize, 0.0f);
    memoryHistory.resize(historySize, 0);
    drawCallsHistory.resize(historySize, 0);
    verticesHistory.resize(historySize, 0);
    trianglesHistory.resize(historySize, 0);
    
    if (GLEW_ARB_timer_query) {
        gpuTimingEnabled = true;
        std::cout << "GPU timing supported" << std::endl;
    } else {
        std::cout << "GPU timing not supported" << std::endl;
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    for (auto& timer : gpuTimers) {
        glDeleteQueries(1, &timer.queryStart);
        glDeleteQueries(1, &timer.queryEnd);
    }
}

void PerformanceMonitor::update(float deltaTime) {
    if (deltaTime > 0.0f) {
        currentFPS = 1.0f / deltaTime;
        currentFrameTime = deltaTime * 1000.0f;
    }
    
    trackMemoryUsage();
    
    if (gpuTimingEnabled) {
        updateGPUTimers();
    }
    
    updateHistories();
    
    drawCalls = 0;
    verticesRendered = 0;
    trianglesRendered = 0;
}

void PerformanceMonitor::renderGUI() {
    if (ImGui::Begin("Performance Monitor")) {
        ImGui::Text("FPS: %.1f (%.2f ms)", currentFPS, currentFrameTime);
        ImGui::Text("Memory: %.2f MB", currentMemoryUsage / (1024.0f * 1024.0f));
        ImGui::Text("Draw Calls: %d", drawCalls);
        ImGui::Text("Vertices: %d", verticesRendered);
        ImGui::Text("Triangles: %d", trianglesRendered);
        
        ImGui::Separator();
        ImGui::Text("FPS History");
        ImPlot::BeginPlot("##FPS History", ImVec2(-1, 80));
        ImPlot::PlotLine("FPS", fpsHistory.data(), fpsHistory.size());
        ImPlot::EndPlot();
        
        ImGui::Text("Frame Time History");
        ImPlot::BeginPlot("##Frame Time History", ImVec2(-1, 80));
        ImPlot::PlotLine("ms", frameTimeHistory.data(), frameTimeHistory.size());
        ImPlot::EndPlot();
        
        ImGui::Text("Memory History");
        ImPlot::BeginPlot("##Memory History", ImVec2(-1, 80));
        
        // Replace the lambda version with a precomputed array
        static std::vector<float> memoryHistoryMB;
        memoryHistoryMB.resize(memoryHistory.size());
        for (size_t i = 0; i < memoryHistory.size(); ++i) {
            memoryHistoryMB[i] = memoryHistory[i] / (1024.0f * 1024.0f);
        }
        ImPlot::PlotLine("MB", memoryHistoryMB.data(), memoryHistoryMB.size());
        
        ImPlot::EndPlot();
        
        if (gpuTimingEnabled && !gpuTimers.empty()) {
            ImGui::Separator();
            ImGui::Text("GPU Timings:");
            
            for (const auto& timer : gpuTimers) {
                if (timer.ended) {
                    ImGui::Text("%s: %.2f ms", timer.name.c_str(), timer.durationMs);
                }
            }
        }
    }
    ImGui::End();
}

void PerformanceMonitor::beginFrame() {
    frameStartTime = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::endFrame() {
    auto frameEndTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime);
    currentFrameTime = duration.count() / 1000.0f;
    
    if (currentFrameTime > 0.0f) {
        currentFPS = 1000.0f / currentFrameTime;
    }
}

void PerformanceMonitor::beginGPUTimer(const std::string& name) {
    if (!gpuTimingEnabled) return;
    
    GPUTimer* timer = getGPUTimer(name);
    if (!timer) {
        GPUTimer newTimer;
        glGenQueries(1, &newTimer.queryStart);
        glGenQueries(1, &newTimer.queryEnd);
        newTimer.name = name;
        newTimer.started = false;
        newTimer.ended = false;
        newTimer.durationMs = 0.0f;
        gpuTimers.push_back(newTimer);
        timer = &gpuTimers.back();
    }
    
    if (!timer->started) {
        glQueryCounter(timer->queryStart, GL_TIMESTAMP);
        timer->started = true;
        timer->ended = false;
    }
}

void PerformanceMonitor::endGPUTimer(const std::string& name) {
    if (!gpuTimingEnabled) return;
    
    GPUTimer* timer = getGPUTimer(name);
    if (timer && timer->started && !timer->ended) {
        glQueryCounter(timer->queryEnd, GL_TIMESTAMP);
        timer->ended = true;
    }
}

void PerformanceMonitor::trackMemoryUsage() {
    currentMemoryUsage = getCurrentProcessMemoryUsage();
}

void PerformanceMonitor::updateHistories() {
    std::rotate(fpsHistory.begin(), fpsHistory.begin() + 1, fpsHistory.end());
    fpsHistory[historySize - 1] = currentFPS;
    
    std::rotate(frameTimeHistory.begin(), frameTimeHistory.begin() + 1, frameTimeHistory.end());
    frameTimeHistory[historySize - 1] = currentFrameTime;
    
    std::rotate(memoryHistory.begin(), memoryHistory.begin() + 1, memoryHistory.end());
    memoryHistory[historySize - 1] = currentMemoryUsage;
    
    std::rotate(drawCallsHistory.begin(), drawCallsHistory.begin() + 1, drawCallsHistory.end());
    drawCallsHistory[historySize - 1] = drawCalls;
    
    std::rotate(verticesHistory.begin(), verticesHistory.begin() + 1, verticesHistory.end());
    verticesHistory[historySize - 1] = verticesRendered;
    
    std::rotate(trianglesHistory.begin(), trianglesHistory.begin() + 1, trianglesHistory.end());
    trianglesHistory[historySize - 1] = trianglesRendered;
}

void PerformanceMonitor::updateGPUTimers() {
    for (auto& timer : gpuTimers) {
        if (timer.started && timer.ended) {
            GLuint64 startTime, endTime;
            GLint startAvailable = 0, endAvailable = 0;
            
            glGetQueryObjectiv(timer.queryStart, GL_QUERY_RESULT_AVAILABLE, &startAvailable);
            glGetQueryObjectiv(timer.queryEnd, GL_QUERY_RESULT_AVAILABLE, &endAvailable);
            
            if (startAvailable && endAvailable) {
                glGetQueryObjectui64v(timer.queryStart, GL_QUERY_RESULT, &startTime);
                glGetQueryObjectui64v(timer.queryEnd, GL_QUERY_RESULT, &endTime);
                
                timer.durationMs = (endTime - startTime) / 1000000.0f;
                
                timer.started = false;
                timer.ended = false;
            }
        }
    }
}

PerformanceMonitor::GPUTimer* PerformanceMonitor::getGPUTimer(const std::string& name) {
    for (auto& timer : gpuTimers) {
        if (timer.name == name) {
            return &timer;
        }
    }
    return nullptr;
}

size_t PerformanceMonitor::getCurrentProcessMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.PrivateUsage;
    }
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    FILE* file = fopen("/proc/self/status", "r");
    if (file) {
        char line[128];
        while (fgets(line, 128, file) != NULL) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                size_t memory = 0;
                sscanf(line + 6, "%zu", &memory);
                fclose(file);
                return memory * 1024;
            }
        }
        fclose(file);
    }
#endif
    return 0;
}
