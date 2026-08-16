#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <iostream>

#include <algorithm>
#include <cmath>


typedef struct {
    glm::vec3 position;
    glm::vec3 racketPosition;
    float racketPitch;
    float racketYaw;
    float racketRoll;
} Player;

typedef struct{
    glm::vec3 position;          // racket object's world position
    glm::quat rotation;          // racket object's world rotation
    glm::vec3 velocity;

    float pitch;
    float yaw;
    float roll;

    glm::vec3 normal;      

    float radius;  
    glm::vec3 netLocalCenter =
    glm::vec3(-0.001f, 0.375f, -3.10f);

    float netRadius = 1.4f;

    float scale = 1.0f;

    glm::mat4 modelMatrix;
    const char* playername;
    const char* enginename;

    int playerID;
    float moveSpeed = 10.0f;

} RacketPlayer;


typedef struct {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 direction{0.0f, -1.0f, 0.0f};

    float dragCoefficient;
    float crossSectionArea;
    float airDensity;

    float mass;

    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

    float rotationSpeed = 8.0f; 

    float collisionRadius = 0.03f;

    

} Shuttlecock;



enum MatchState {
    INRALLY,
    BREAK,
    OFF
};

enum class CourtRegion
{
    P1_ODD,
    P1_EVEN,
    P2_ODD,
    P2_EVEN,

    P1_SHORT,
    P2_SHORT,

    P1_BACK,
    P2_BACK,

    LEFT_SIDE,
    RIGHT_SIDE,

    OUTSIDE
};

typedef struct {
    int player1Score;
    int player2Score;

    Player player1;
    Player player2;
    Shuttlecock shuttlecock;

    RacketPlayer racketPlayer1;
    RacketPlayer racketPlayer2;

    MatchState matchstate = BREAK;
    int breaktimer = 0; 
    const int breaktime = 20;

    int thisrallyserver; // 1 PLAYER1 ----- 2 PLAYER2
    int thisrallyservelocation; // 1 LEFT 0 RIGHT (ODD EVEN)
    int serveended = 0;// 0 = serve, 1 check, 2 = good

    int rallyended = 0;
    int lasthitter = 0;
    
} GameState;



void initPlayer(Player* player, glm::vec3 position, glm::vec3 racketPosition, float racketPitch, float racketYaw, float racketRoll);
void movePlayer(Player* player, GameState* gamestate, float deltaTime);

void initRacketPlayer(RacketPlayer* racketPlayer, glm::vec3 position, glm::vec3 velocity, float pitch, float yaw, float roll);
void moveRacketPlayer(RacketPlayer* racketPlayer, GameState* gamestate, float deltaTime);

void initShuttlecock(Shuttlecock* shuttlecock, glm::vec3 position, glm::vec3 velocity, float mass, float dragCoefficient, float crossSectionArea, float airDensity);
void moveShuttlecock(Shuttlecock* shuttlecock, GameState* gamestate, float deltaTime);

void initGameState(GameState* gamestate);
void run(GameState* gamestate, float deltaTime);

float randomFloat(float min, float max);

bool collideShuttlecockRacket(
    Shuttlecock* shuttlecock,
    const RacketPlayer* racket,
    const glm::vec3& previousPosition,
    const glm::mat4& racketModel);

void collideShuttlecockFloor(Shuttlecock* shuttlecock);
glm::mat4 getRacketModelMatrix(const RacketPlayer* racket);
bool collideShuttlecockCourtNet(
    Shuttlecock* shuttlecock,
    const glm::vec3& previousPosition
);
void collideShuttlecockFloor(Shuttlecock* shuttlecock, GameState* gamestate);
CourtRegion getCourtRegion(const glm::vec3& position);
