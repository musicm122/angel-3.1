#pragma once

#include "../Actors/PhysicsActor.h"
#include "../Infrastructure/World.h"
#include "../Infrastructure/Log.h"
#include "../Util/MathUtil.h"
#include <Box2D/Box2D.h>
struct b2ShapeDef;

enum EdgeVertexNumber
{
    V0 = 0,
    V1 = 1,
    V2 = 2,
    V3 = 3,
};

class PhysicsEdgeActor:public PhysicsActor
{
public:
    PhysicsEdgeActor(void);
    void InitPhysics();

    virtual void CustomInitPhysics();
    virtual const String GetClassName() const { return "PhysicsEdgeActor"; }
        
    b2Vec2 GetVertexAt(EdgeVertexNumber num);
    void SetVertexAt(EdgeVertexNumber num, b2Vec2 val);
    b2Vec2 GetFirstVertex();
    b2Vec2 GetLastVertex();
    
    void Set(b2Vec2 start, b2Vec2 end);
    void Set(b2EdgeShape edge);
    b2EdgeShape GetB2EdgeShape();
    
    ~PhysicsEdgeActor(void);

private:
	friend class World;
    b2EdgeShape _edge;
	void _syncPosRot(float x, float y, float rotation);
};
