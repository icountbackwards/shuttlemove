#include "headers/renderer.h"

int initRenderer(Renderer* renderer){
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    renderer->window = glfwCreateWindow(renderer->width, renderer->height, renderer->windowTitle, NULL, NULL);
    if (renderer->window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(renderer->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    

    glViewport(0, 0, renderer->width, renderer->height);

    glfwSetFramebufferSizeCallback(renderer->window, framebuffer_size_callback);  
    glfwSetInputMode(renderer->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetScrollCallback(renderer->window, scroll_callback); 
    glfwSetCursorPosCallback(renderer->window, mouse_callback);  

    glfwSetWindowUserPointer(renderer->window, renderer);

    glEnable(GL_DEPTH_TEST);  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

    renderer->camera = Camera(glm::vec3(0.0f, 3.0f, 10.0f));//z = 10.0
    renderer->shader = new Shader("../shaders/shader.vert", "../shaders/shader.frag");

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    Renderer* rendererobj = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        rendererobj->camera.ProcessKeyboard(FORWARD, rendererobj->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rendererobj->camera.ProcessKeyboard(BACKWARD, rendererobj->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        rendererobj->camera.ProcessKeyboard(LEFT, rendererobj->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        rendererobj->camera.ProcessKeyboard(RIGHT, rendererobj->deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        if (rendererobj->gamestate->matchstate == OFF){
            rendererobj->gamestate->matchstate = BREAK;
            rendererobj->gamestate->breaktimer = 0;
            rendererobj->gamestate->player1Score = 0;
            rendererobj->gamestate->player2Score = 0;
            rendererobj->gamestate->thisrallyserver = 1;
            rendererobj->gamestate->thisrallyservelocation = 0; // RIGHT
            std::cout << "---GAME STARTED---\n";
            std::cout << "PLAYER 1 (NEAR): " << rendererobj->gamestate->racketPlayer1.playername << " run by engine : " << rendererobj->gamestate->racketPlayer1.enginename << "\n";
            std::cout << "PLAYER 2 (FAR): " << rendererobj->gamestate->racketPlayer2.playername << " run by engine : " << rendererobj->gamestate->racketPlayer2.enginename << "\n";
        }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    Renderer* rendererobj = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (rendererobj->firstMouse)
    {
        rendererobj->lastX = xpos;
        rendererobj->lastY = ypos;
        rendererobj->firstMouse = false;
    }

    float xoffset = xpos - rendererobj->lastX;
    float yoffset = rendererobj->lastY - ypos; // reversed since y-coordinates go from bottom to top

    rendererobj->lastX = xpos;
    rendererobj->lastY = ypos;

    rendererobj->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Renderer* rendererobj = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    rendererobj->  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}