#include "headers/gamestate.h"

void initGameState(GameState* gamestate){
    gamestate->matchstate = OFF;
    gamestate->player1Score = 0;
    gamestate->player2Score = 0;

    initPlayer(&gamestate->player1, glm::vec3(-1.5f, 0.85f, 3.0f), glm::vec3(0.7f, 0.0f, 0.0f), 0.0f, 90.0f, 90.0f);
    initPlayer(&gamestate->player2, glm::vec3(1.5f, 0.85f, -3.0f), glm::vec3(-0.7f, 0.0f, 0.0f), 0.0f, 270.0f, 90.0f);

    initRacketPlayer(&gamestate->racketPlayer1, glm::vec3(-0.8f, 0.85f, 3.0f), glm::vec3(0.0f), 0.0f, 90.0f, 90.0f);
    initRacketPlayer(&gamestate->racketPlayer2, glm::vec3(0.8f, 0.85f, -3.0f), glm::vec3(0.0f), 0.0f, 270.0f, 90.0f);
    gamestate->racketPlayer1.playerID = 1;
    gamestate->racketPlayer2.playerID = 2;

    initShuttlecock(&gamestate->shuttlecock, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f), 0.005f, 0.8f, 0.003f, 1.225f);
}


void run(GameState* gamestate, float deltaTime)
{
    moveRacketPlayer(&gamestate->racketPlayer1,gamestate,deltaTime);

    moveRacketPlayer(&gamestate->racketPlayer2,gamestate,deltaTime);

    gamestate->racketPlayer1.modelMatrix =
        getRacketModelMatrix(
            &gamestate->racketPlayer1
        );

    gamestate->racketPlayer2.modelMatrix =
        getRacketModelMatrix(
            &gamestate->racketPlayer2
        );

    moveShuttlecock(
        &gamestate->shuttlecock,
        gamestate,
        deltaTime
    );

    if(gamestate->rallyended == 1){
        gamestate->rallyended = 0;
        gamestate->lasthitter = 0;
        gamestate->player1Score++;
        gamestate->thisrallyserver = 1;
        if(gamestate->player1Score % 2 == 0){
            gamestate->thisrallyservelocation = 0;
        }else{
            gamestate->thisrallyservelocation = 1;
        }
        gamestate->matchstate = BREAK;
        gamestate->breaktimer = 0;

        std::cout << "PLAYER 1 SCORES\n";
        std::cout << gamestate->player1Score << " : " << gamestate->player2Score << "\n";
    }
    if(gamestate->rallyended == 2){
        gamestate->rallyended = 0;
        gamestate->lasthitter = 0;
        gamestate->player2Score++;
        gamestate->thisrallyserver = 2;
        if(gamestate->player1Score % 2 == 0){
            gamestate->thisrallyservelocation = 0;
        }else{
            gamestate->thisrallyservelocation = 1;
        }
        gamestate->matchstate = BREAK;
        gamestate->breaktimer = 0;
        std::cout << "PLAYER 2 SCORES\n";
        std::cout << gamestate->player1Score << " : " << gamestate->player2Score << "\n";
    }
}

void initPlayer(Player* player, glm::vec3 position, glm::vec3 racketPosition, float racketPitch, float racketYaw, float racketRoll){
    
    player->position = position;
    player->racketPosition = racketPosition;
    player->racketPitch = racketPitch;
    player->racketYaw = racketYaw;
    player->racketRoll = racketRoll;
}

void movePlayer(Player* player, GameState* gamestate, float deltaTime){
    glm::vec3 movement(randomFloat(-0.7f, 0.7f), 0.0f, randomFloat(-0.7f, 0.7f));
    player->position += movement * deltaTime;
}

void moveShuttlecock(
    Shuttlecock* shuttlecock,
    GameState* gamestate,
    float deltaTime)
{
    if (shuttlecock == nullptr || gamestate == nullptr)
        return;

    constexpr float GRAVITY = 9.80665f;

    glm::vec3 previousPosition = shuttlecock->position;

    // ============================================================
    // GRAVITY
    // ============================================================

    glm::vec3 gravityForce(
        0.0f,
        -shuttlecock->mass * GRAVITY,
        0.0f
    );

    // ============================================================
    // DRAG
    // ============================================================

    float speed = glm::length(shuttlecock->velocity);

    glm::vec3 dragForce(0.0f);

    if (speed > 0.0001f)
    {
        glm::vec3 direction =
            shuttlecock->velocity / speed;

        float dragMagnitude =
            0.5f *
            shuttlecock->airDensity *
            shuttlecock->dragCoefficient *
            shuttlecock->crossSectionArea *
            speed * speed;

        dragForce =
            -direction * dragMagnitude;
    }

    // ============================================================
    // ACCELERATION
    // ============================================================

    glm::vec3 totalForce =
        gravityForce + dragForce;

    shuttlecock->acceleration =
        totalForce / shuttlecock->mass;

    // ============================================================
    // INTEGRATION
    // ============================================================

    shuttlecock->velocity +=
        shuttlecock->acceleration * deltaTime;

    shuttlecock->position +=
        shuttlecock->velocity * deltaTime;

    // ============================================================
    // RACKET COLLISION
    // ============================================================

    bool collided1 =
        collideShuttlecockRacket(
            shuttlecock,
            &gamestate->racketPlayer1,
            previousPosition,
            gamestate->racketPlayer1.modelMatrix
        );

    bool collided2 =
        collideShuttlecockRacket(
            shuttlecock,
            &gamestate->racketPlayer2,
            previousPosition,
            gamestate->racketPlayer2.modelMatrix
        );

    // ============================================================
    // CONFIRMED HIT -> SEND SHUTTLE TO OPPONENT
    // ============================================================

    if (collided1 || collided2)
    {
        int hitter = collided1 ? 1 : 2;

        gamestate->lasthitter = hitter;

        // Pick target on opponent's court.
        glm::vec3 target;

        target.x = randomFloat(-2.0f, 2.0f);
        target.y = 0.3f;

        if (hitter == 1)
        {
            // Player 1 (+Z) -> Player 2 (-Z)
            target.z = randomFloat(-4.5f, -2.0f);
        }
        else
        {
            // Player 2 (-Z) -> Player 1 (+Z)
            target.z = randomFloat(2.0f, 4.5f);
        }

        // Direction toward opponent.
        glm::vec3 horizontalDirection =
            target - shuttlecock->position;

        horizontalDirection.y = 0.0f;

        float length =
            glm::length(horizontalDirection);

        if (length > 0.001f)
        {
            horizontalDirection /= length;
        }
        else
        {
            horizontalDirection =
                (hitter == 1)
                ? glm::vec3(0.0f, 0.0f, -1.0f)
                : glm::vec3(0.0f, 0.0f, 1.0f);
        }

        // Strong, obvious rally shot.
        constexpr float HORIZONTAL_SPEED = 14.0f;
        constexpr float UPWARD_SPEED = 9.0f;

        shuttlecock->velocity =
            horizontalDirection * HORIZONTAL_SPEED;

        shuttlecock->velocity.y =
            UPWARD_SPEED;

        //std::cout
        //    << "RACKET " << hitter << " HIT\n"
        //    << "velocity = "
        //    << shuttlecock->velocity.x << ", "
        //    << shuttlecock->velocity.y << ", "
        //    << shuttlecock->velocity.z << '\n';
    }

    // ============================================================
    // SERVE STATE
    // ============================================================

    if (gamestate->serveended == 0)
    {
        if (
            gamestate->thisrallyserver == 1 &&
            collided1
        )
        {
            gamestate->serveended++;
        }

        if (
            gamestate->thisrallyserver == 2 &&
            collided2
        )
        {
            gamestate->serveended++;
        }
    }

    if (gamestate->serveended == 1)
    {
        if (
            gamestate->thisrallyserver == 1 &&
            collided2
        )
        {
            gamestate->serveended++;
        }

        if (
            gamestate->thisrallyserver == 2 &&
            collided1
        )
        {
            gamestate->serveended++;
        }
    }

    // ============================================================
    // COURT NET
    // ============================================================

    collideShuttlecockCourtNet(
        shuttlecock,
        previousPosition
    );

    // ============================================================
    // FLOOR
    // ============================================================

    collideShuttlecockFloor(
        shuttlecock,
        gamestate
    );

    // ============================================================
    // ROTATION
    // ============================================================

    float newSpeed =
        glm::length(shuttlecock->velocity);

    if (newSpeed > 0.0001f)
    {
        glm::vec3 targetDirection =
            glm::normalize(shuttlecock->velocity);

        glm::vec3 modelForward(
            0.0f,
            0.0f,
            -1.0f
        );

        glm::quat targetRotation =
            glm::quat(
                glm::normalize(modelForward),
                targetDirection
            );

        float t =
            glm::clamp(
                shuttlecock->rotationSpeed *
                deltaTime,
                0.0f,
                1.0f
            );

        shuttlecock->rotation =
            glm::normalize(
                glm::slerp(
                    shuttlecock->rotation,
                    targetRotation,
                    t
                )
            );

        shuttlecock->direction =
            glm::normalize(
                shuttlecock->rotation *
                modelForward
            );
    }
}

float randomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_real_distribution<float> distribution(min, max);

    return distribution(generator);
}

void initShuttlecock(
    Shuttlecock* shuttlecock,
    glm::vec3 position,
    glm::vec3 velocity,
    float mass,
    float dragCoefficient,
    float crossSectionArea,
    float airDensity)
{
    shuttlecock->position = position;
    shuttlecock->velocity = velocity;

    shuttlecock->mass = mass;
    shuttlecock->dragCoefficient = dragCoefficient;
    shuttlecock->crossSectionArea = crossSectionArea;
    shuttlecock->airDensity = airDensity;

    shuttlecock->collisionRadius = 0.03f;

    shuttlecock->rotation =
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    shuttlecock->direction =
        glm::vec3(0.0f, 0.0f, -1.0f);

    shuttlecock->rotationSpeed = 10.0f;
}

void initRacketPlayer(RacketPlayer* racketPlayer, glm::vec3 position, glm::vec3 velocity, float pitch, float yaw, float roll){
    
    racketPlayer->position = position;
    racketPlayer->velocity = velocity;
    racketPlayer->pitch = pitch;
    racketPlayer->yaw = yaw;
    racketPlayer->roll = roll;
    
}



void moveRacketPlayer(
    RacketPlayer* racketPlayer,
    GameState* gamestate,
    float deltaTime)
{
    Shuttlecock* shuttle = &gamestate->shuttlecock;

    // Must match collideShuttlecockRacket()
    constexpr glm::vec3 LOCAL_NET_CENTER(
        0.0f,
        0.0f,
        -0.2131f
    );

    // ------------------------------------------------------------
    // Only intercept shuttle when it is on this player's side
    // ------------------------------------------------------------

    bool shuttleOnMySide =
        (racketPlayer->playerID == 1)
        ? shuttle->position.z > 0.0f
        : shuttle->position.z < 0.0f;

    if (!shuttleOnMySide)
    {
        racketPlayer->velocity = glm::vec3(0.0f);
        return;
    }

    // ------------------------------------------------------------
    // Calculate racket rotation
    // ------------------------------------------------------------

    glm::mat4 rotation(1.0f);

    rotation = glm::rotate(
        rotation,
        glm::radians(racketPlayer->yaw),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    rotation = glm::rotate(
        rotation,
        glm::radians(racketPlayer->pitch),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    rotation = glm::rotate(
        rotation,
        glm::radians(racketPlayer->roll),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::vec3 netOffset =
        glm::vec3(
            rotation *
            glm::vec4(
                LOCAL_NET_CENTER,
                0.0f
            )
        );

    // ------------------------------------------------------------
    // Predict where shuttle will be HALF a frame from now.
    //
    // This is important:
    // putting the racket exactly at shuttle.position can make
    // racket and shuttle move together without crossing.
    //
    // Putting the racket at the midpoint of the shuttle's next
    // movement makes the shuttle travel THROUGH the racket.
    // ------------------------------------------------------------

    glm::vec3 predictedMidpoint =
        shuttle->position +
        shuttle->velocity *
        deltaTime *
        0.5f;

    // ------------------------------------------------------------
    // Don't make racket chase crazy high shuttle positions.
    //
    // Only begin interception once the shuttle is low enough.
    // ------------------------------------------------------------

    constexpr float MAX_HIT_HEIGHT = 2.0f;

    if (shuttle->position.y > MAX_HIT_HEIGHT)
    {
        // Follow horizontally while waiting for it to descend.

        glm::vec3 target =
            racketPlayer->position;

        target.x = shuttle->position.x;

        target.x =
            std::clamp(
                target.x,
                -2.5f,
                2.5f
            );

        glm::vec3 oldPosition =
            racketPlayer->position;

        racketPlayer->position =
            target;

        if (deltaTime > 0.000001f)
        {
            racketPlayer->velocity =
                (racketPlayer->position - oldPosition)
                / deltaTime;
        }

        return;
    }

    // ------------------------------------------------------------
    // Put actual racket STRING CENTER at predicted midpoint
    // ------------------------------------------------------------

    glm::vec3 targetPosition =
        predictedMidpoint -
        netOffset;

    // ------------------------------------------------------------
    // Clamp court position
    // ------------------------------------------------------------

    targetPosition.x =
        std::clamp(
            targetPosition.x,
            -2.5f,
            2.5f
        );

    if (racketPlayer->playerID == 1)
    {
        targetPosition.z =
            std::clamp(
                targetPosition.z,
                0.8f,
                5.6f
            );
    }
    else
    {
        targetPosition.z =
            std::clamp(
                targetPosition.z,
                -5.6f,
                -0.8f
            );
    }

    // Limit how high the actual racket origin can go.
    targetPosition.y =
        std::clamp(
            targetPosition.y,
            0.8f,
            2.0f
        );

    // ------------------------------------------------------------
    // For the demo: snap close enough to guarantee interception.
    //
    // We're using the actual collision detector to determine
    // whether the hit happens, so snapping here isn't causing
    // the fake "1 meter away" collision from before.
    // ------------------------------------------------------------

    glm::vec3 oldPosition =
        racketPlayer->position;

    racketPlayer->position =
        targetPosition;

    // ------------------------------------------------------------
    // Racket velocity
    // ------------------------------------------------------------

    if (deltaTime > 0.000001f)
    {
        racketPlayer->velocity =
            (
                racketPlayer->position -
                oldPosition
            )
            / deltaTime;
    }
    else
    {
        racketPlayer->velocity =
            glm::vec3(0.0f);
    }
}

void collideShuttlecockFloor(Shuttlecock* shuttlecock, GameState* gamestate)
{
    constexpr float FLOOR_Y = 0.0f;

    float bottom =
        shuttlecock->position.y -
        shuttlecock->collisionRadius;

    if (bottom <= FLOOR_Y)
    {
        shuttlecock->position.y =
            FLOOR_Y + shuttlecock->collisionRadius;

        // For badminton, once it hits the floor,
        // the rally is basically over.
        shuttlecock->velocity =
            glm::vec3(0.0f);
        //std::cout << "Shuttlecock hits the floor" << "\n";
        CourtRegion region = getCourtRegion(shuttlecock->position);
        switch (gamestate->serveended){
            case 0:
                if(gamestate->thisrallyserver == 1){
                    if(gamestate->thisrallyservelocation == 0){
                        if (region != CourtRegion::P2_EVEN){
                            gamestate->rallyended = 2;
                        }else{
                            gamestate->rallyended = 1;
                        }
                    }else{
                        if (region != CourtRegion::P2_ODD){
                            gamestate->rallyended = 2;
                        }else{
                            gamestate->rallyended = 1;
                        }
                    }
                }else{
                    if(gamestate->thisrallyservelocation == 0){
                        if (region != CourtRegion::P1_EVEN){
                            gamestate->rallyended = 1;
                        }else{
                            gamestate->rallyended = 2;
                        }

                    }else{
                        if (region != CourtRegion::P1_ODD){
                            gamestate->rallyended = 1;
                        }else{
                            gamestate->rallyended = 2;
                        }
                    }
                }
                break;
            case 1:
                if(gamestate->lasthitter == 1){
                    if(region == CourtRegion::P2_BACK || region == CourtRegion::P2_EVEN || region == CourtRegion::P2_ODD){
                        gamestate->rallyended = 1;
                    }else{
                        gamestate->rallyended = 2;
                    }
                }else if (gamestate->lasthitter == 2){
                    if(region == CourtRegion::P1_BACK || region == CourtRegion::P1_EVEN || region == CourtRegion::P1_ODD){
                        gamestate->rallyended = 2;
                    }else{
                        gamestate->rallyended = 1;
                    }
                }
                
                break;
            case 2:
                if(gamestate->lasthitter == 1){
                    if(region == CourtRegion::P2_BACK || region == CourtRegion::P2_EVEN || region == CourtRegion::P2_ODD){
                        gamestate->rallyended = 1;
                    }else{
                        gamestate->rallyended = 2;
                    }
                }else if(gamestate->lasthitter == 2){
                    if(region == CourtRegion::P1_BACK || region == CourtRegion::P1_EVEN || region == CourtRegion::P1_ODD){
                        gamestate->rallyended = 2;
                    }else{
                        gamestate->rallyended = 1;
                    }
                }
                break;
        }
    }
}

bool collideShuttlecockRacket(
    Shuttlecock* shuttlecock,
    const RacketPlayer* racket,
    const glm::vec3& previousPosition,
    const glm::mat4& racketModel)
{
    if (shuttlecock == nullptr || racket == nullptr)
        return false;

    // Actual racket string-bed geometry
    constexpr float NET_Y = 0.0f;

    constexpr float NET_CENTER_X = 0.0f;
    constexpr float NET_CENTER_Z = -0.2131f;

    constexpr float NET_RADIUS_X = 0.0922f;
    constexpr float NET_RADIUS_Z = 0.1162f;

    // Give the strings some collision thickness.
    constexpr float NET_THICKNESS = 0.10f;

    glm::mat4 inverseModel =
        glm::inverse(racketModel);

    // Convert shuttle position into racket-local coordinates.
    glm::vec3 local =
        glm::vec3(
            inverseModel *
            glm::vec4(
                shuttlecock->position,
                1.0f
            )
        );

    // ------------------------------------------------------------
    // Check distance from the STRING PLANE
    // ------------------------------------------------------------

    float planeDistance =
        std::abs(local.y - NET_Y);

    if (
        planeDistance >
        NET_THICKNESS + shuttlecock->collisionRadius
    )
    {
        return false;
    }

    // ------------------------------------------------------------
    // Check whether shuttle is inside racket head ellipse
    // ------------------------------------------------------------

    float dx =
        local.x - NET_CENTER_X;

    float dz =
        local.z - NET_CENTER_Z;

    // Slightly enlarge racket for reliable demo collision.
    float radiusX =
        NET_RADIUS_X +
        shuttlecock->collisionRadius +
        0.04f;

    float radiusZ =
        NET_RADIUS_Z +
        shuttlecock->collisionRadius +
        0.04f;

    float ellipse =
        (dx * dx) /
        (radiusX * radiusX)
        +
        (dz * dz) /
        (radiusZ * radiusZ);

    if (ellipse > 1.0f)
        return false;

    // ============================================================
    // COLLISION CONFIRMED
    // ============================================================

    //std::cout
    //    << "\n*** RACKET COLLISION ***\n"
    //    << "Player: "
    //    << racket->playerID
    //    << "\nLocal shuttle: "
    //    << local.x << ", "
    //    << local.y << ", "
    //    << local.z
    //    << "\n";

    // Do NOT calculate reflection here.
    //
    // moveShuttlecock() already detects this return value
    // and assigns the controlled outgoing rally velocity.

    return true;
}

glm::mat4 getRacketModelMatrix(const RacketPlayer* racket)
{
    glm::mat4 model(1.0f);

    model = glm::translate(
        model,
        racket->position
    );

    model = glm::rotate(
        model,
        glm::radians(racket->yaw),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    model = glm::rotate(
        model,
        glm::radians(racket->pitch),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    model = glm::rotate(
        model,
        glm::radians(racket->roll),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    return model;
}

bool collideShuttlecockCourtNet(
    Shuttlecock* shuttlecock,
    const glm::vec3& previousPosition)
{
    if (shuttlecock == nullptr)
        return false;

    // ------------------------------------------------
    // Court net AABB
    // ------------------------------------------------

    constexpr glm::vec3 NET_MIN(
        -3.05f,
         0.0f,
        -0.025f
    );

    constexpr glm::vec3 NET_MAX(
         3.05f,
         1.55f,
         0.025f
    );

    // Expand the net by the shuttlecock collision radius.
    // This converts sphere-vs-box into point-vs-expanded-box.
    glm::vec3 expandedMin =
        NET_MIN -
        glm::vec3(shuttlecock->collisionRadius);

    glm::vec3 expandedMax =
        NET_MAX +
        glm::vec3(shuttlecock->collisionRadius);

    glm::vec3 movement =
        shuttlecock->position -
        previousPosition;

    float tEnter = 0.0f;
    float tExit  = 1.0f;

    glm::vec3 hitNormal(0.0f);

    // ------------------------------------------------
    // Swept segment vs AABB
    // ------------------------------------------------

    for (int axis = 0; axis < 3; ++axis)
    {
        float start =
            previousPosition[axis];

        float direction =
            movement[axis];

        float minValue =
            expandedMin[axis];

        float maxValue =
            expandedMax[axis];

        // No movement along this axis.
        if (std::abs(direction) < 0.000001f)
        {
            // If we're outside the box on this axis,
            // collision is impossible.
            if (
                start < minValue ||
                start > maxValue
            )
            {
                return false;
            }

            continue;
        }

        float t1 =
            (minValue - start) /
            direction;

        float t2 =
            (maxValue - start) /
            direction;

        glm::vec3 normal1(0.0f);

        // Entering through minimum face.
        normal1[axis] = -1.0f;

        // Make t1 the entering intersection.
        if (t1 > t2)
        {
            std::swap(t1, t2);

            normal1[axis] = 1.0f;
        }

        if (t1 > tEnter)
        {
            tEnter = t1;
            hitNormal = normal1;
        }

        tExit =
            std::min(tExit, t2);

        if (tEnter > tExit)
            return false;
    }

    // Intersection happened outside this frame.
    if (
        tEnter < 0.0f ||
        tEnter > 1.0f
    )
    {
        return false;
    }

    // ------------------------------------------------
    // Collision confirmed
    // ------------------------------------------------

    glm::vec3 collisionPoint =
        previousPosition +
        movement * tEnter;

    // Safety: if normal somehow wasn't established.
    if (glm::length(hitNormal) < 0.5f)
        return false;

    // ------------------------------------------------
    // Collision response
    // ------------------------------------------------

    float velocityAlongNormal =
        glm::dot(
            shuttlecock->velocity,
            hitNormal
        );

    // Only respond if moving INTO the surface.
    if (velocityAlongNormal >= 0.0f)
        return false;

    // The net absorbs a lot of energy.
    constexpr float restitution =
        0.15f;

    shuttlecock->velocity =
        shuttlecock->velocity -
        (1.0f + restitution) *
        velocityAlongNormal *
        hitNormal;

    // Prevent remaining inside the collider.
    shuttlecock->position =
        collisionPoint +
        hitNormal * 0.001f;

    return true;
}


CourtRegion getCourtRegion(const glm::vec3& position)
{
    float x = position.x;
    float z = position.z;

    // ============================================================
    // Completely outside the physical court
    // ============================================================

    if (x < -3.05f || x > 3.05f ||
        z < -6.70f || z > 6.70f)
    {
        return CourtRegion::OUTSIDE;
    }

    // ============================================================
    // Singles side regions
    // ============================================================

    if (x < -2.59f)
    {
        return CourtRegion::LEFT_SIDE;
    }

    if (x > 2.59f)
    {
        return CourtRegion::RIGHT_SIDE;
    }

    // From this point:
    //
    // -2.59 <= x <= 2.59
    //
    // so we're inside the singles court width.


    // ============================================================
    // PLAYER 1 SIDE (+Z)
    // ============================================================

    // Short region
    if (z >= 0.0f && z < 0.8f)
    {
        return CourtRegion::P1_SHORT;
    }

    // Main service region
    if (z >= 0.8f && z <= 5.75f)
    {
        if (x < 0.0f)
        {
            return CourtRegion::P1_ODD;
        }
        else
        {
            return CourtRegion::P1_EVEN;
        }
    }

    // Back region
    if (z > 5.75f && z <= 6.70f)
    {
        return CourtRegion::P1_BACK;
    }


    // ============================================================
    // PLAYER 2 SIDE (-Z)
    // ============================================================

    // Short region
    if (z <= 0.0f && z > -0.8f)
    {
        return CourtRegion::P2_SHORT;
    }

    // Main service region
    if (z <= -0.8f && z >= -5.75f)
    {
        if (x > 0.0f)
        {
            return CourtRegion::P2_ODD;
        }
        else
        {
            return CourtRegion::P2_EVEN;
        }
    }

    // Back region
    if (z < -5.75f && z >= -6.70f)
    {
        return CourtRegion::P2_BACK;
    }

    return CourtRegion::OUTSIDE;
}