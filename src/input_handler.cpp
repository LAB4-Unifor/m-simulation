#include "input_handler.h"
#include <iostream>

InputHandler::InputHandler() 
    : mousePosition(0.0f, 0.0f),
      mouseDelta(0.0f, 0.0f),
      lastMousePosition(0.0f, 0.0f),
      isOrbiting(false),
      isPanning(false),
      isZooming(false) {
    for (int i = 0; i < 5; i++) {
        mouseButtons[i] = false;
    }
}

void InputHandler::handleEvent(const SDL_Event& event, Camera& camera) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
            mousePosition.x = event.motion.x;
            mousePosition.y = event.motion.y;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button < 5) {
                mouseButtons[event.button.button] = true;
                
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isOrbiting = true;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    isPanning = true;
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    isZooming = true;
                }
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button < 5) {
                mouseButtons[event.button.button] = false;
                
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isOrbiting = false;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    isPanning = false;
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    isZooming = false;
                }
            }
            break;
            
        case SDL_MOUSEWHEEL:
            camera.zoom(event.wheel.y * 0.5f);
            break;
            
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_r:
                    camera.setAutoRotate(!camera.isAutoRotating());
                    break;
                case SDLK_SPACE:
                    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
                    camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
                    break;
            }
            break;
    }
}

void InputHandler::update(float deltaTime, Camera& camera) {
    mouseDelta = mousePosition - lastMousePosition;
    lastMousePosition = mousePosition;
    
    if (isOrbiting && (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)) {
        camera.orbit(mouseDelta.x * 0.01f, mouseDelta.y * 0.01f);
    }
    
    if (isPanning && (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)) {
        camera.pan(-mouseDelta.x, mouseDelta.y);
    }
    
    if (isZooming && mouseDelta.y != 0.0f) {
        camera.zoom(mouseDelta.y * 0.1f);
    }
    
    resetMouseDelta();
}

bool InputHandler::isMouseButtonDown(int button) const {
    if (button >= 0 && button < 5) {
        return mouseButtons[button];
    }
    return false;
}

void InputHandler::resetMouseDelta() {
    mouseDelta = glm::vec2(0.0f, 0.0f);
}
