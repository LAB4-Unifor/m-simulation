#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "camera.h"

class InputHandler {
private:
    bool leftMouseDown;
    bool rightMouseDown;
    glm::vec2 mousePos;
    glm::vec2 lastMousePos;
    bool mouseMoved;
    bool keyStates[512];  // For keyboard state tracking

public:
    InputHandler();
    void handleEvent(const SDL_Event& event, Camera& camera);
    void updateMousePosition(int x, int y);
    void resetMouseState();
    void setKeyState(int key, bool state);
    bool isKeyDown(int key) const;
};
