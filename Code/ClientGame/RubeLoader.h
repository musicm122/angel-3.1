#include "rubestuff\b2dJson.h"
#include "rubestuff\b2dJsonImage_OpenGL.h"
#include "..\Util\DrawUtil.h"
#include "..\Angel\Actors\PhysicsActor.h"

class RubeLoader: b2Draw
{
public:
    b2World* m_world;
    std::string m_filename;
    std::vector<b2dJsonImage_OpenGL*> m_images;
    b2MouseJoint* m_mouseJoint;

    RubeLoader();
    
    void SetWorld(b2World* world);
    bool Load(std::string filename);
    virtual void afterLoadProcessing(b2dJson* json);
    void clear();
    void step();    
    void render();
    virtual void removeImage(b2dJsonImage_OpenGL* image);
    virtual void removeBody(b2Body* body);
    void reorderImages();
    b2World GetWorld();
    ~RubeLoader(void);
private:

    bool addPhysicsActor(PhysicsActor actor);
    bool addPhysicsActor(PhysicsActor actor, int layer=0);
    bool removePhysicsActor(PhysicsActor actor);

    //b2Draw inherited members 
    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	virtual void DrawTransform(const b2Transform& xf);

};

