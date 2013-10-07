#include "stdafx.h"
#pragma once 
#include "PhysicsEdgeActor.h"


PhysicsEdgeActor::PhysicsEdgeActor(void)
{
    this->_shapeType = SHAPETYPE_EDGE;   
    this->_edge = b2EdgeShape();
    PhysicsActor::SetShapeType(SHAPETYPE_EDGE);    
}

PhysicsEdgeActor::~PhysicsEdgeActor(void)
{
    if( _physBody != NULL )
    {
        _physBody->SetUserData(NULL);
        theWorld.GetPhysicsWorld().DestroyBody(_physBody);
    }
}

void PhysicsEdgeActor::Set(b2Vec2 v1, b2Vec2 v2)
{
    _edge.Set(v1, v2);
}

void PhysicsEdgeActor::Set(b2EdgeShape edge)
{
    this->_edge = edge;
}

void PhysicsEdgeActor::SetVertexAt(EdgeVertexNumber num, b2Vec2 val)
{
    switch (num)
    {
    case V0:
        this->_edge.m_vertex0 = val;
        break;
    case V1:
        this->_edge.m_vertex1 = val;
        break;
    case V2:
        this->_edge.m_vertex2 = val;
        break;
    case V3:
        this->_edge.m_vertex3= val;
        break;
    default:
        break;
    }
}

b2Vec2 PhysicsEdgeActor::GetVertexAt(EdgeVertexNumber num)
{
    switch (num)
    {
    case V0:
        return this->_edge.m_vertex0;
        break;
    case V1:
        return this->_edge.m_vertex1;
        break;
    case V2:
        return this->_edge.m_vertex2;
        break;
    case V3:
        return this->_edge.m_vertex3;
        break;
    default:
        break;
    }
}


b2EdgeShape PhysicsEdgeActor::GetB2EdgeShape()
{
    return this->_edge;
}


void PhysicsActor::InitPhysics()
{
    //todo:add specific init logic for additional shapes (see eShapeType for shapes)
    if (!theWorld.IsPhysicsSetUp())
    {
        sysLog.Log("ERROR: World physics must be initialized before Actor's.");
        return;
    }
   
    this->_shapeType = SHAPETYPE_EDGE;
    b2Shape* shape = NULL;
   
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = _density;
    fixtureDef.friction = _friction;
    fixtureDef.restitution = _restitution;

    fixtureDef.filter.groupIndex = _groupIndex;
    fixtureDef.isSensor = _isSensor;

    InitShape( shape );

    b2BodyDef bd;
    bd.userData = this;
    bd.position.Set(_position.X, _position.Y);
    bd.angle = MathUtil::ToRadians(_rotation);
    bd.fixedRotation = _fixedRotation;
    if (MathUtil::FuzzyEquals(_density, 0.0f))
    {
        bd.type = b2_staticBody;
    }
    else 
    {
        bd.type = b2_dynamicBody;
    }

    _physBody = theWorld.GetPhysicsWorld().CreateBody(&bd);
    _physBody->CreateFixture(&fixtureDef);
    _physBody->SetUserData(this);
    CustomInitPhysics();
}