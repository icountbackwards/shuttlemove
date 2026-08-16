#define TINYOBJLOADER_IMPLEMENTATION
#include "headers/mesh.h"

#include <stb_image.h>

void createMesh(
    Mesh* mesh,
    float* vertices,
    unsigned int vertexCount
)
{
    mesh->vertexCount = vertexCount;

    glGenVertexArrays(
        1,
        &mesh->VAO
    );

    glGenBuffers(
        1,
        &mesh->VBO
    );

    glBindVertexArray(
        mesh->VAO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        mesh->VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertexCount * 5,
        vertices,
        GL_STATIC_DRAW
    );

    // Position
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void createTexturedMesh(
    Mesh* mesh,
    float* vertices,
    unsigned int vertexCount,
    const char* texturePath
)
{

    createMesh(mesh, vertices, vertexCount);


    glGenTextures(1, &mesh->texture);
    glBindTexture(GL_TEXTURE_2D, mesh->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );


    int width, height, channels;

    unsigned char* data = stbi_load(
        texturePath,
        &width,
        &height,
        &channels,
        0
    );


    if (!data)
    {
        std::cerr
            << "Failed: "
            << texturePath
            << ": "
            << stbi_failure_reason()
            << '\n';
        return;
    }

    std::cout
        << width << "x"
        << height
        << " channels="
        << channels << '\n';

    GLenum format;

    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    else
    {
        std::cerr
            << "Unsupported channel count: "
            << channels << '\n';

        stbi_image_free(data);
        return;
    }

    // Important for RGB textures whose row width isn't divisible by 4.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );


    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
    
void drawMesh(Renderer* renderer, Mesh* mesh, Shader* shader, glm::mat4 modelMatrix, float opacity){
    shader->setMat4("model", modelMatrix);
    glm::mat4 projection = glm::perspective(glm::radians(renderer->camera.Zoom), (float)renderer->width / (float)renderer->height, 0.1f, 100.0f);
    shader->setMat4("projection", projection);
    glm::mat4 view = renderer->camera.GetViewMatrix();
    shader->setMat4("view", view);
    shader->setFloat("opacity", opacity);

    glBindTexture(GL_TEXTURE_2D, mesh->texture);
    glBindVertexArray(mesh->VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
}

bool loadOBJ(
    const char* path,
    std::vector<float>& vertices,
    float targetSize
)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool success = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        path
    );

    if (!warn.empty())
        std::cout << "OBJ warning: " << warn << '\n';

    if (!err.empty())
        std::cerr << "OBJ error: " << err << '\n';

    if (!success)
        return false;

    if (attrib.vertices.empty())
    {
        std::cerr << "OBJ contains no vertices\n";
        return false;
    }

    // Find bounding box
    glm::vec3 minPos(FLT_MAX);
    glm::vec3 maxPos(-FLT_MAX);

    for (size_t i = 0; i < attrib.vertices.size(); i += 3)
    {
        glm::vec3 p(
            attrib.vertices[i + 0],
            attrib.vertices[i + 1],
            attrib.vertices[i + 2]
        );

        minPos = glm::min(minPos, p);
        maxPos = glm::max(maxPos, p);
    }

    glm::vec3 modelSize = maxPos - minPos;

    float maxDimension = std::max(
        modelSize.x,
        std::max(modelSize.y, modelSize.z)
    );

    if (maxDimension <= 0.0f)
    {
        std::cerr << "Invalid OBJ dimensions\n";
        return false;
    }

    float scale = targetSize / maxDimension;

    // Center model around origin
    glm::vec3 center = (minPos + maxPos) * 0.5f;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            // Position
            glm::vec3 p(
                attrib.vertices[
                    3 * index.vertex_index + 0
                ],
                attrib.vertices[
                    3 * index.vertex_index + 1
                ],
                attrib.vertices[
                    3 * index.vertex_index + 2
                ]
            );

            p = (p - center) * scale;

            vertices.push_back(p.x);
            vertices.push_back(p.y);
            vertices.push_back(p.z);

            // Texture coordinates
            if (index.texcoord_index >= 0)
            {
                float u =
                    attrib.texcoords[
                        2 * index.texcoord_index + 0
                    ];

                float v =
                    attrib.texcoords[
                        2 * index.texcoord_index + 1
                    ];

                vertices.push_back(u);
                vertices.push_back(v);
            }
            else
            {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }
    }

    return true;
}

bool createModelMesh(
    Mesh* mesh,
    const char* modelPath,
    const char* texturePath,
    float targetSize
)
{
    std::vector<float> vertices;

    if (!loadOBJ(
        modelPath,
        vertices,
        targetSize
    ))
    {
        std::cerr
            << "Failed to load model: "
            << modelPath
            << '\n';

        return false;
    }

    unsigned int vertexCount =
        static_cast<unsigned int>(
            vertices.size() / 5
        );

    std::cout
        << "Loaded "
        << vertexCount
        << " vertices\n";

    createTexturedMesh(
        mesh,
        vertices.data(),
        vertexCount,
        texturePath
    );

    return true;
}