#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "headers/shader.h"
#include "headers/camera.h"
#include "headers/gamestate.h"

typedef struct {
    // settings
    unsigned int width = 800;
    unsigned int height = 600;

    // camera
    Camera camera;
    float lastX = width / 2.0f;
    float lastY = height / 2.0f;
    bool firstMouse = true;

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    const char* windowTitle = "shuttlemove";

    GLFWwindow* window = nullptr;

    Shader* shader = nullptr;

    GameState* gamestate = nullptr;

} Renderer;

int initRenderer(Renderer* renderer);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);