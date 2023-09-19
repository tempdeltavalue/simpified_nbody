

#include "WindowInputManager.h"
#include <iostream>

WindowInputManager::WindowInputManager(Window *win, Camera* camera): window(win), camera(camera) {
    this->setFramebufferSizeCallback();
    this->setMouseMovementCallback();
    this->setMouseButtonCallback();
    this->setScrollCallback();
}

//ParticleSimulation* WindowInputManager::getParticleSimulation(){
//    return this->particleSimulation;
//}

/**
 * Resizeable window callback
 */
void WindowInputManager::setFramebufferSizeCallback() {
    // Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process.
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetFramebufferSizeCallback(this->window->getWindow(), [](GLFWwindow* win, int width, int height)->void
    {
        // retrieve the pointer to the instance
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win));
        // Update the window size
        windowInputManager->window->updateWindowSize(width, height);

        // Update the aspect ratio

        windowInputManager->camera->setAspectRatio(glm::vec2(width, height));
    });
}


/**
 * Cursor/mouse movement
 */
void WindowInputManager::setMouseMovementCallback() {
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetCursorPosCallback(this->window->getWindow(), [](GLFWwindow* win, double xPos, double yPos)->void
    {
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance

        // Rotate the camera it the mouse is pressed
        windowInputManager->camera->rotateCallback(glm::vec2(xPos, yPos));
    });
}

/**
 * Mouse scroll
 */
void WindowInputManager::setScrollCallback(){
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetScrollCallback(this->window->getWindow(), [](GLFWwindow* win, double xOffset, double yOffset)->void
    {
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance

        // Zoom in / Zoom out
        windowInputManager->camera->zoomCallback(yOffset);
    });
}

/**
 * Mouse buttons
 */
void WindowInputManager::setMouseButtonCallback() {
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetMouseButtonCallback(this->window->getWindow(), [](GLFWwindow* win, int button, int action, int mods)->void
    {
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            // The user started to click!
            windowInputManager->camera->setIsDragging(true);
            double prevMouseXPos, prevMouseYPos;
            glfwGetCursorPos(win, &prevMouseXPos, &prevMouseYPos);
            windowInputManager->camera->setPreviousMousePos(glm::vec2(prevMouseXPos, prevMouseYPos));
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            // The user is not clicking anymore
            windowInputManager->camera->setIsDragging(false);
        }
    });
}
