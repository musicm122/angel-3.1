#include "StdAfx.h"
#include "RORGameManager.h"


const int RORGameManager::WorldZoom = 1;

RORGameManager::RORGameManager(void)
{
    //worldLoader = new RubeLoader();

    theSwitchboard.SubscribeTo(this,"HideStartScreen");
    theSwitchboard.SubscribeTo(this,"ShowStartScreen");
    theSwitchboard.Broadcast(new Message("ShowStartScreen"));
    StartScreenShowing = true;
}

void RORGameManager::ShowStartScreen()
{
    StartScreenShowing = true;
    theWorld.LoadLevel("StartScreen");
}

void RORGameManager::HideStartScreen()
{
    StartScreenShowing = false;
    theWorld.UnloadAll();
    theSwitchboard.UnsubscribeFrom(this,"HideStartScreen");
    theSwitchboard.UnsubscribeFrom(this,"ShowStartScreen");
    InitGameProcess();
}


#pragma region Init Methods

void RORGameManager::InitGameProcess()
{
    InitWorld();

    //theWorld.SetupPhysics("./Resources/Scripts/images.json");

    InitCamera();
    InitPlayer();
    InitBoundaries();

}

void RORGameManager::InitCamera()
{
    _cameraLockMin = theTuning.GetFloat("CameraLockMin");
    _cameraLockMax = theTuning.GetFloat("CameraLockMax");
}

void RORGameManager::InitPlayer()
{
    _player= new Player(Vector2(-3.0f, 7.0f));
    theWorld.Add(_player);
    _player->SetUp();
}

void RORGameManager::InitWorld()
{
    _debugDraw = false;
    theWorld.SetupPhysics(Vector2(0, -100.0f));
    //C:\Users\terrancesmith\Downloads\Angel Engine\Angel-3.1\Angel-3.1\Code\ClientGame\Resources\Scripts
    //theWorld.SetupPhysics("./Resources/Resources/Scripts/images.json");
    
    theWorld.GetPhysicsWorld().SetDebugDraw(this);
    this->SetFlags(b2Draw::e_shapeBit);
    //theWorld.SetupPhysics();
}

void RORGameManager::InitGround()
{
    theWorld.LoadLevel("Level1");
    /*PhysicsActor *ground = (PhysicsActor*)Actor::Create("ground_actor");
    ground->InitPhysics();
    theWorld.Add(ground);*/
}

void RORGameManager::InitLeftBoundary(float halfScreenWidth)
{
    PhysicsActor* leftBlocker = (PhysicsActor*)Actor::Create("left_blocker");
    leftBlocker->SetPosition((_cameraLockMin - halfScreenWidth) - (leftBlocker->GetSize().X * 0.5f), 0.0f);
    leftBlocker->InitPhysics();
    theWorld.Add(leftBlocker);
}

void RORGameManager::InitRightBoundary(float halfScreenWidth)
{
    PhysicsActor* rightBlocker = (PhysicsActor*)Actor::Create("right_blocker");
    rightBlocker->SetPosition((_cameraLockMax + halfScreenWidth) + (rightBlocker->GetSize().X * 0.5f), 0.0f);
    rightBlocker->InitPhysics();
    theWorld.Add(rightBlocker);
}

void RORGameManager::InitBoundaries()
{
    float halfWidth = GetHalfScreenWidth();
    InitGround();
    InitLeftBoundary(halfWidth);
    InitRightBoundary(halfWidth);
}
#pragma endregion 


float RORGameManager::GetHalfScreenWidth()
{
    Vector2 botLeft  = theCamera.GetWorldMinVertex();
    Vector2 topRight = theCamera.GetWorldMaxVertex();
    float halfWidth = (topRight.X - botLeft.X) * 0.5f;
    return halfWidth;
}


void RORGameManager::Update(float dt)
{

    if (theInput.IsKeyDown(ANGEL_KEY_D))
    {
        _debugDraw = true;
    }
    else 
    {
        _debugDraw = false;
    }

    if(!StartScreenShowing){
        //fixes camera on player's x pos if players x position is less than the camera lock location
        float playerX= _player->GetPosition().X;
        if ( (playerX > _cameraLockMin) && (playerX < _cameraLockMax) )
        {
            if (theCamera.GetLockedActor() != _player)
            {
                theCamera.LockTo(_player, true, false);
            }
        }
        else 
        {
            //otherwise unlock camera on player 
            //(think mario games: following player when player reaches more than half way across the screen)
            if (theCamera.GetLockedActor() == _player)
            {
                theCamera.LockTo(NULL);

                float camWidth = _cameraLockMax - _cameraLockMin;
                float midX = _cameraLockMin + (camWidth * 0.5f);
                if (_player->GetPosition().X < midX)
                {
                    theCamera.SetPosition(_cameraLockMin, 0.0f);
                }
                else
                {
                    theCamera.SetPosition(_cameraLockMax, 0.0f);
                }
            }
        }
    }

}

void RORGameManager::Render()
{
    if (_debugDraw)
    {
        theWorld.GetPhysicsWorld().DrawDebugData();
    }
}



void RORGameManager::ReceiveMessage(Message* message)
{
    if (message->GetMessageName() == "ShowStartScreen")
    {
        ShowStartScreen();
    }
    else if (message->GetMessageName() == "HideStartScreen")
    {
        HideStartScreen();
    }
}



void RORGameManager::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void RORGameManager::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glVertexPointer(2, GL_FLOAT, 0, vertices);

    glColor4f(color.r, color.g, color.b,0.5f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

    glColor4f(color.r, color.g, color.b,1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void RORGameManager::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const int vertexCount=16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;

    GLfloat				glVertices[vertexCount*2];
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertices[i*2]=v.x;
        glVertices[i*2+1]=v.y;
        theta += k_increment;
    }

    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_FLOAT, 0, glVertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
}

void RORGameManager::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const int vertexCount=16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;

    GLfloat				glVertices[vertexCount*2];
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertices[i*2]=v.x;
        glVertices[i*2+1]=v.y;
        theta += k_increment;
    }

    glColor4f(color.r, color.g, color.b,0.5f);
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    glColor4f(color.r, color.g, color.b,1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);

    // Draw the axis line
    DrawSegment(center,center+radius*axis,color);
}

void RORGameManager::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    glColor4f(color.r, color.g, color.b,1);
    GLfloat				glVertices[] = {
        p1.x,p1.y,p2.x,p2.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINES, 0, 2);
}

void RORGameManager::DrawTransform(const b2Transform& xf)
{

}

void RORGameManager::loadLevel1(b2World* m_world)
{

}