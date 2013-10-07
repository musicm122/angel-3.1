#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include "Player.h"
//#include "rubestuff/b2dJson.h"
//#include "RubeLoader.h"



class RORGameManager : public GameManager, b2Draw
{
public:
	RubeLoader* worldLoader;
    
	RORGameManager(void);

	static const int WorldZoom;

	bool StartScreenShowing;

	void InitGameProcess();
	
	void ShowStartScreen();
	void HideStartScreen();
	void loadLevel1(b2World* world);
	virtual void Update(float dt);
	virtual void Render();

	virtual void ReceiveMessage(Message* message);
	
	//b2Draw interface
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	virtual void DrawTransform(const b2Transform& xf);
private:
	Player* _player;

	void InitCamera();
	void InitPlayer();
	void InitWorld();

	void InitGround();
	void InitBoundaries();
	
	void InitLeftBoundary(float halfScreenWidth);
	void InitRightBoundary(float halfScreenWidth);
	
	float GetHalfScreenWidth();

	bool _debugDraw;
	float _cameraLockMin;
	float _cameraLockMax;


};