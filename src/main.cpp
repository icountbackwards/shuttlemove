#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "headers/shader.h"
#include "headers/camera.h"
#include "headers/renderer.h"
#include "headers/mesh.h"
#include "headers/gamestate.h"



float courtVertices[] = {
    // x      y     z       u     v

    -3.05f,  0.0f, -6.70f,  0.0f, 0.0f,
     3.05f,  0.0f, -6.70f,  1.0f, 0.0f,
     3.05f,  0.0f,  6.70f,  1.0f, 1.0f,

    -3.05f,  0.0f, -6.70f,  0.0f, 0.0f,
     3.05f,  0.0f,  6.70f,  1.0f, 1.0f,
    -3.05f,  0.0f,  6.70f,  0.0f, 1.0f
};

float netVertices[] = {
    // x       y       z        u     v

    // Front
    -3.05f, 0.0f,  -0.025f,   0.0f, 0.0f,
     3.05f, 0.0f,  -0.025f,   1.0f, 0.0f,
     3.05f, 1.55f, -0.025f,   1.0f, 1.0f,

    -3.05f, 0.0f,  -0.025f,   0.0f, 0.0f,
     3.05f, 1.55f, -0.025f,   1.0f, 1.0f,
    -3.05f, 1.55f, -0.025f,   0.0f, 1.0f,

    // Back
    -3.05f, 0.0f,   0.025f,   0.0f, 0.0f,
     3.05f, 1.55f,  0.025f,   1.0f, 1.0f,
     3.05f, 0.0f,   0.025f,   1.0f, 0.0f,

    -3.05f, 0.0f,   0.025f,   0.0f, 0.0f,
    -3.05f, 1.55f,  0.025f,   0.0f, 1.0f,
     3.05f, 1.55f,  0.025f,   1.0f, 1.0f,

    // Left side
    -3.05f, 0.0f,  -0.025f,   0.0f, 0.0f,
    -3.05f, 1.55f, -0.025f,   1.0f, 0.0f,
    -3.05f, 1.55f,  0.025f,   1.0f, 1.0f,

    -3.05f, 0.0f,  -0.025f,   0.0f, 0.0f,
    -3.05f, 1.55f,  0.025f,   1.0f, 1.0f,
    -3.05f, 0.0f,   0.025f,   0.0f, 1.0f,

    // Right side
     3.05f, 0.0f,  -0.025f,   0.0f, 0.0f,
     3.05f, 1.55f,  0.025f,   1.0f, 1.0f,
     3.05f, 1.55f, -0.025f,   1.0f, 0.0f,

     3.05f, 0.0f,  -0.025f,   0.0f, 0.0f,
     3.05f, 0.0f,   0.025f,   0.0f, 1.0f,
     3.05f, 1.55f,  0.025f,   1.0f, 1.0f,

    // Top
    -3.05f, 1.55f, -0.025f,   0.0f, 0.0f,
     3.05f, 1.55f, -0.025f,   1.0f, 0.0f,
     3.05f, 1.55f,  0.025f,   1.0f, 1.0f,

    -3.05f, 1.55f, -0.025f,   0.0f, 0.0f,
     3.05f, 1.55f,  0.025f,   1.0f, 1.0f,
    -3.05f, 1.55f,  0.025f,   0.0f, 1.0f,

    // Bottom
    -3.05f, 0.0f, -0.025f,    0.0f, 0.0f,
     3.05f, 0.0f,  0.025f,    1.0f, 1.0f,
     3.05f, 0.0f, -0.025f,    1.0f, 0.0f,

    -3.05f, 0.0f, -0.025f,    0.0f, 0.0f,
    -3.05f, 0.0f,  0.025f,    0.0f, 1.0f,
     3.05f, 0.0f,  0.025f,    1.0f, 1.0f
};

unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};  

float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
};

Renderer renderer;

Mesh shuttlecockmesh;
Mesh racket1mesh;
Mesh racket2mesh;
Mesh player1mesh;
Mesh player2mesh;
Mesh courtmesh;
Mesh netmesh;
Mesh fencemesh;

GameState gamestate;

constexpr float COURT_HALF_WIDTH       = 3.05f;
constexpr float SINGLES_HALF_WIDTH     = 2.59f;
constexpr float COURT_HALF_LENGTH      = 6.6f;
constexpr float SHORT_SERVICE_LINE     = 0.8f;
constexpr float DOUBLES_LONG_LINE      = 5.75f; // 6.70 - 0.76

// ============================================================
// PLAYER 1: +Z side, facing toward -Z
//
// From P1's perspective:
//   right court = +X = EVEN
//   left court  = -X = ODD
// ============================================================

// P1 ODD / left service court
glm::vec3 inServeArea1OddMin(-2.59f,0.0f,0.8f);

glm::vec3 inServeArea1OddMax(0.0f,0.0f,6.6f);

// P1 EVEN / right service court
glm::vec3 inServeArea1EvenMin(0.0f,0.0f,0.6f);

glm::vec3 inServeArea1EvenMax(2.59f,0.0f,6.6f);


// ============================================================
// PLAYER 2: -Z side, facing toward +Z
//
// From P2's perspective:
//   right court = -X = EVEN
//   left court  = +X = ODD
// ============================================================

// P2 ODD / left service court
glm::vec3 inServeArea2OddMin(0.0f,0.0f,-6.6f);

glm::vec3 inServeArea2OddMax(2.59f,0.0f,-0.8f);

// P2 EVEN / right service court
glm::vec3 inServeArea2EvenMin(-2.59f,0.0f,-6.6f);

glm::vec3 inServeArea2EvenMax(0.0f,0.0f,-0.8f);

Mesh p1OddGreenMesh;
Mesh p1OddRedMesh;

Mesh p1EvenGreenMesh;
Mesh p1EvenRedMesh;

Mesh p2OddGreenMesh;
Mesh p2OddRedMesh;

Mesh p2EvenGreenMesh;
Mesh p2EvenRedMesh;

Mesh leftSideRedMesh;
Mesh rightSideRedMesh;

Mesh p1BackGreenMesh;
Mesh p2BackGreenMesh;
Mesh p1BackRedMesh;
Mesh p2BackRedMesh;

Mesh p1ShortGreenMesh;
Mesh p1ShortRedMesh;

Mesh p2ShortGreenMesh;
Mesh p2ShortRedMesh;

void createCourtRegionMesh(
    Mesh* mesh,
    float minX,
    float maxX,
    float minZ,
    float maxZ,
    const char* texturePath);

const bool DISPLAYREGION = false;



int main()
{
    if (initRenderer(&renderer) != 0)
    {
        return -1;
    }

    createModelMesh(&shuttlecockmesh, "../assets/shuttlecock.obj", "../assets/gold.jpg", 0.085f); //0.085
    createModelMesh(&racket1mesh, "../assets/racket.obj", "../assets/red.png", 0.67f);
    createModelMesh(&racket2mesh, "../assets/racket.obj", "../assets/red.png", 0.67f);
    createModelMesh(&player1mesh, "../assets/human.obj", "../assets/darkblue.jpg", 1.7f);
    createModelMesh(&player2mesh, "../assets/human.obj", "../assets/red.png", 1.7f);

    createTexturedMesh(&courtmesh, courtVertices, 6, "../assets/floor.jpg");
    createTexturedMesh(&netmesh, netVertices, 36, "../assets/gold.jpg");

    initGameState(&gamestate);

    gamestate.shuttlecock.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 racket1postitionodd(-0.8f, 0.85f, 3.0f);
    glm::vec3 racket1postitioneven(0.8f, 0.85f, 3.0f);
    glm::vec3 racket2postitionodd(0.8f, 0.85f, -3.0f);
    glm::vec3 racket2postitioneven(-0.8f, 0.85f, -3.0f);

    glm::vec3 shuttlecockDropSpot1Odd(-0.8f, 5.0f, 2.9f);
    glm::vec3 shuttlecockDropSpot1Even(0.8f, 5.0f, 2.9f);
    glm::vec3 shuttlecockDropSpot2Odd(0.8f, 5.0f, -2.9f);
    glm::vec3 shuttlecockDropSpot2Even(-0.8f, 5.0f, -2.9f);

    // ============================================================
    // SERVICE REGIONS
    // ============================================================

    // P1 odd
    createCourtRegionMesh(&p1OddGreenMesh,-2.59f, 0.0f,0.8f, 5.75f,"../assets/green.png");
    createCourtRegionMesh(&p1OddRedMesh,-2.59f, 0.0f,0.8f, 5.75f,"../assets/red.png");

    // P1 even
    createCourtRegionMesh(&p1EvenGreenMesh,0.0f, 2.59f,0.8f, 5.75f,"../assets/green.png");
    createCourtRegionMesh(&p1EvenRedMesh,0.0f, 2.59f,0.8f, 5.75f,"../assets/red.png");


    // P2 odd
    createCourtRegionMesh(&p2OddGreenMesh,0.0f, 2.59f,-5.75f, -0.8f,"../assets/green.png");
    createCourtRegionMesh(&p2OddRedMesh,0.0f, 2.59f,-5.75f, -0.8f,"../assets/red.png");

    // P2 even
    createCourtRegionMesh(&p2EvenGreenMesh,-2.59f, 0.0f,-5.75f, -0.8f,"../assets/green.png");
    createCourtRegionMesh(&p2EvenRedMesh,-2.59f, 0.0f,-5.75f, -0.8f,"../assets/red.png");


    // ============================================================
    // SINGLES SIDE OUT-OF-BOUNDS
    // ============================================================

    createCourtRegionMesh(&leftSideRedMesh,-3.05f, -2.59f,-6.70f, 6.70f,"../assets/red.png");

    createCourtRegionMesh(&rightSideRedMesh,2.59f, 3.05f,-6.70f, 6.70f,"../assets/red.png");


    // ============================================================
    // BACK REGIONS
    //
    // This is the portion between the doubles long-service line
    // and baseline. It remains IN for singles.
    // ============================================================

    // Player 1 back
    createCourtRegionMesh(&p1BackGreenMesh,-2.59f, 2.59f,5.75f, 6.70f,"../assets/green.png");
    createCourtRegionMesh(&p1BackRedMesh,-2.59f, 2.59f,5.75f, 6.70f,"../assets/red.png");

    // Player 2 back
    createCourtRegionMesh(&p2BackGreenMesh,-2.59f, 2.59f,-6.70f, -5.75f,"../assets/green.png");
    createCourtRegionMesh(&p2BackRedMesh,-2.59f, 2.59f,-6.70f, -5.75f,"../assets/red.png");

    // Player 1 short region: net -> P1 short service line
    createCourtRegionMesh(&p1ShortGreenMesh,-2.59f, 2.59f,0.0f, 1.7f,"../assets/green.png");

    createCourtRegionMesh(&p1ShortRedMesh,-2.59f, 2.59f,0.0f, 1.7f,"../assets/red.png");

    // Player 2 short region: P2 short service line -> net
    createCourtRegionMesh(&p2ShortGreenMesh,-2.59f, 2.59f,-1.7f, 0.0f,"../assets/green.png");

    createCourtRegionMesh(&p2ShortRedMesh,-2.59f, 2.59f,-1.7f, 0.0f,"../assets/red.png");


    renderer.gamestate = &gamestate;

    gamestate.racketPlayer1.playername = "BOT1";
    gamestate.racketPlayer1.enginename = "RANDOMIZED-ENGINE";
    gamestate.racketPlayer2.playername = "BOT2";
    gamestate.racketPlayer2.enginename = "RANDOMIZED-ENGINE";

    std::cout << "----------DONE INITIALITING----------\n";
    std::cout << "      PRESS SPACEBAR TO START GAME   \n";


    // render loop
    // -----------
    while (!glfwWindowShouldClose(renderer.window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        renderer.deltaTime = currentFrame - renderer.lastFrame;
        renderer.lastFrame = currentFrame;

        switch (gamestate.matchstate){
            case OFF:
                break;
            case BREAK:
                gamestate.breaktimer++;
                if (gamestate.breaktimer >= gamestate.breaktime){
                    std::cout << "------------ START RALLY--------------\n";
                    gamestate.matchstate = INRALLY;
                }
                if (gamestate.breaktimer == 1){
                        
                        if(gamestate.thisrallyservelocation == 1){
                            //ODD
                            gamestate.racketPlayer1.position = racket1postitionodd;
                            gamestate.racketPlayer2.position = racket2postitionodd;
                            if(gamestate.thisrallyserver == 1){
                                gamestate.shuttlecock.position = shuttlecockDropSpot1Odd;
                            }else{
                                gamestate.shuttlecock.position = shuttlecockDropSpot2Odd;
                            }
                        }
                        if(gamestate.thisrallyservelocation == 0){
                            //EVEN
                            gamestate.racketPlayer1.position = racket1postitioneven;
                            gamestate.racketPlayer2.position = racket2postitioneven;
                            if(gamestate.thisrallyserver == 1){
                                gamestate.shuttlecock.position = shuttlecockDropSpot1Even;
                            }else{
                                gamestate.shuttlecock.position = shuttlecockDropSpot2Even;
                            }
                        }
                        gamestate.racketPlayer1.pitch = 0;
                        gamestate.racketPlayer1.yaw = 90;
                        gamestate.racketPlayer1.roll = 90;
                        gamestate.racketPlayer2.pitch = 0;
                        gamestate.racketPlayer2.yaw = 270;
                        gamestate.racketPlayer2.roll = 90;
                        
                        
                    }
                break;
            case INRALLY:
                run(&gamestate, renderer.deltaTime);
                break;
        }

        processInput(renderer.window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        renderer.shader->use();

        
        glm::mat4 model = glm::mat4(1.0f); 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        drawMesh(&renderer, &courtmesh, renderer.shader, model, 1.0f);

        glDepthMask(GL_FALSE);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        drawMesh(&renderer, &netmesh, renderer.shader, model, 0.3f);

        drawMesh(
            &renderer,
            &racket1mesh,
            renderer.shader,
            gamestate.racketPlayer1.modelMatrix,
            1.0f
        );

        drawMesh(
            &renderer,
            &racket2mesh,
            renderer.shader,
            gamestate.racketPlayer2.modelMatrix,
            1.0f
        );
        
        //drawMesh(&renderer, &racket2mesh, renderer.shader, model, 1.0f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, gamestate.shuttlecock.position);
        model *= glm::mat4_cast(gamestate.shuttlecock.rotation);
        drawMesh(&renderer, &shuttlecockmesh, renderer.shader, model, 1.0f);

        glm::mat4 identity(1.0f);

        if(gamestate.matchstate == INRALLY && DISPLAYREGION){
            Mesh regions[] = { // serveended = 2 default values
                p1OddGreenMesh, p1EvenGreenMesh, p2OddGreenMesh, p2EvenGreenMesh,
                leftSideRedMesh, rightSideRedMesh, p1BackGreenMesh, p2BackGreenMesh, p1ShortGreenMesh, p2ShortGreenMesh
            };
            if (gamestate.serveended == 0 || gamestate.serveended == 1){
                if(gamestate.thisrallyserver == 1){
                    if (gamestate.thisrallyservelocation == 0){
                        regions[0] = p1OddRedMesh;
                        regions[1] = p1EvenRedMesh;
                        regions[2] = p2OddRedMesh;
                    }else{
                        regions[0] = p1OddRedMesh;
                        regions[1] = p1EvenRedMesh;
                        regions[3] = p2EvenRedMesh;
                    }
                }else{
                    if (gamestate.thisrallyservelocation == 0){
                        regions[2] = p2OddRedMesh;
                        regions[3] = p2EvenRedMesh;
                        regions[0] = p1OddRedMesh;
                    }else{
                        regions[2] = p2OddRedMesh;
                        regions[3] = p2EvenRedMesh;
                        regions[1] = p1EvenRedMesh;
                    }
                }
            }

            for(int i = 0; i < 10; i++){
                drawMesh(&renderer, &regions[i], renderer.shader, identity, 0.4f);
            }
        }

        glDepthMask(GL_TRUE);

        glfwSwapBuffers(renderer.window);
        glfwPollEvents();
    }
  
    glfwTerminate();
    return 0;
}

void createCourtRegionMesh(
    Mesh* mesh,
    float minX,
    float maxX,
    float minZ,
    float maxZ,
    const char* texturePath)
{
    constexpr float REGION_Y = 0.003f;

    float vertices[] = {
        // x     y         z      u     v

        minX, REGION_Y, minZ,    0.0f, 0.0f,
        maxX, REGION_Y, minZ,    1.0f, 0.0f,
        maxX, REGION_Y, maxZ,    1.0f, 1.0f,

        minX, REGION_Y, minZ,    0.0f, 0.0f,
        maxX, REGION_Y, maxZ,    1.0f, 1.0f,
        minX, REGION_Y, maxZ,    0.0f, 1.0f
    };

    createTexturedMesh(
        mesh,
        vertices,
        6,
        texturePath
    );
}

