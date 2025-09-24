#include "input_handler.h"
#include <SDL2/SDL.h>

InputHandler::InputHandler() 
    : leftMouseDown(false), 
      rightMouseDown(false),
      mousePos(0.0f, 0.0f),
      lastMousePos(0.0f, 0.0f),
      mouseMoved(false) {
    
    // Initialize all keys to false
    for (int i = 0; i < 512; ++i) {
        keyStates[i] = false;
    }
}

void InputHandler::handleEvent(const SDL_Event& event, Camera& camera) {
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                leftMouseDown = true;
                lastMousePos = mousePos;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                rightMouseDown = true;
                lastMousePos = mousePos;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                leftMouseDown = false;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                rightMouseDown = false;
            }
            break;
            
        case SDL_MOUSEMOTION:
            if (leftMouseDown) {
                // Calculate delta from last position
                glm::vec2 delta = mousePos - lastMousePos;
                camera.orbit(-delta.x * 0.01f, -delta.y * 0.01f);
                lastMousePos = mousePos;
                mouseMoved = true;
            } else if (rightMouseDown) {
                glm::vec2 delta = mousePos - lastMousePos;
                camera.pan(delta.x * 0.01f, delta.y * 0.01f);
                lastMousePos = mousePos;
                mouseMoved = true;
            }
            break;
            
        case SDL_MOUSEWHEEL:
            camera.zoom(-event.wheel.y * 0.1f);
            break;
            
        case SDL_KEYDOWN:
            keyStates[event.key.keysym.scancode] = true;
            // Handle keyboard shortcuts
            if (event.key.keysym.sym == SDLK_1) {
                camera.setRadius(5.0f);
                camera.setFov(45.0f);
            } else if (event.key.keysym.sym == SDLK_2) {
                camera.setRadius(3.0f);
                camera.setFov(60.0f);
            } else if (event.key.keysym.sym == SDLK_3) {
                camera.setRadius(10.0f);
                camera.setFov(30.0f);
            }
            break;
            
        case SDL_KEYUP:
            keyStates[event.key.keysym.scancode] = false;
            break;
    }
}

void InputHandler::updateMousePosition(int x, int y) {
    mousePos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

void InputHandler::resetMouseState() {
    leftMouseDown = false;
    rightMouseDown = false;
    mouseMoved = false;
}

void InputHandler::setKeyState(int key, bool state) {
    keyStates[key] = state;
}

bool InputHandler::isKeyDown(int key) const {
    return keyStates[key];
}