#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include "grid_renderer.h"
#include "performance_monitor.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height, const std::string& title);
    void cleanup();
    void clear();
    void swapBuffers();
    
    PerformanceMonitor& getPerformanceMonitor() { return performanceMonitor; }
    
    SDL_Window* getWindow() const { return window; }
    SDL_GLContext getGLContext() const { return glContext; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    GridRenderer& getGridRenderer() { return gridRenderer; }
    
private:
    SDL_Window* window;
    SDL_GLContext glContext;
    int width;
    int height;
    GridRenderer gridRenderer;
    PerformanceMonitor performanceMonitor;
};

#endif
