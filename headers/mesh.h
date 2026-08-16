#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "headers/shader.h"
#include "headers/renderer.h"


#include <tiny_obj_loader.h>

#include <cfloat>
#include <algorithm>
#include <vector>

#include <string>
#include <iostream>

typedef struct {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int vertexCount;

    glm::vec3 position;
    glm::vec3 rotation;

    unsigned int texture;
    
} Mesh;

void createMesh(Mesh* mesh, float* vertices, unsigned int vertexCount);
void createTexturedMesh(Mesh* mesh, float* vertices, unsigned int vertexCount, const char* texturePath);
void drawMesh(Renderer* renderer, Mesh* mesh, Shader* shader, glm::mat4 modelMatrix, float opacity);
bool loadOBJ(const char* path, std::vector<float>& vertices, float targetSize);
bool createModelMesh(Mesh* mesh, const char* modelPath, const char* texturePath, float targetSize);