#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL.h>
#include "camera.h"

class InputHandler {
public:
    InputHandler();
    
    void handleEvent(const SDL_Event& event, Camera& camera);
    void update(float deltaTime, Camera& camera);
    
    bool isMouseButtonDown(int button) const;
    glm::vec2 getMousePosition() const { return mousePosition; }
    glm::vec2 getMouseDelta() const { return mouseDelta; }
    
private:
    glm::vec2 mousePosition;
    glm::vec2 mouseDelta;
    glm::vec2 lastMousePosition;
    
    bool mouseButtons[5];
    bool isOrbiting;
    bool isPanning;
    bool isZooming;
    
    void resetMouseDelta();
};

#endif
