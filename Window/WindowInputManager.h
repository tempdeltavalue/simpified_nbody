#include "Window.h"
#include "Camera.h"

#ifndef N_BODY_WINDOWINPUTMANAGER_H
#define N_BODY_WINDOWINPUTMANAGER_H


class WindowInputManager {
public:
    WindowInputManager(Window *win,  Camera* camera);
    // Callbacks
    void setKeyCallback();
    void setMouseButtonCallback();
    // Resizeable window
    void setFramebufferSizeCallback();
    // Cursor movement
    void setMouseMovementCallback();
    // Mouse scroll callback
    void setScrollCallback();

    //ParticleSimulation *getParticleSimulation();
private:
    Window *window;
    Camera* camera;
};


#endif //N_BODY_WINDOWINPUTMANAGER_H
