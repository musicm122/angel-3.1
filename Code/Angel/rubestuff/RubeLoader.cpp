#include "StdAfx.h"
#include "RubeLoader.h"

using namespace std;

//--Comparer
bool compareImagesByRenderOrder_ascending(const b2dJsonImage_OpenGL* a, const b2dJsonImage_OpenGL* b)
{
    return a->renderOrder < b->renderOrder;
}
//----------

RubeLoader::RubeLoader()
{
    m_world = nullptr;
    m_filename = "";
    m_mouseJoint = nullptr;
}

bool RubeLoader::Load(std::string filename)
{
    this->clear();
    m_filename = filename;
    b2dJson json;
    std::string errMsg = "";
    m_world = json.readFromFile(filename.c_str(), errMsg);

    if ( ! m_world )
    {
        sysLog.Log("Failed to load scene from " + filename + " : " +errMsg );
        return false;
    }
    // Set up a debug draw so we can see what's going on in the physics engine.

    SetFlags( b2Draw::e_shapeBit );
    m_world->SetDebugDraw(this);

    sysLog.Log("Loaded scene from " + filename );

    // do whatever else needs to be done when loading
    afterLoadProcessing(&json);

    return true;
}

void RubeLoader::SetWorld(b2World* world)
{
    this->m_world= world;
}

b2World RubeLoader::GetWorld()
{
    return *m_world;
}
void RubeLoader::afterLoadProcessing(b2dJson* json)
{
    //load images
    vector<b2dJsonImage*> images;
    json->getAllImages(images);
    for (int i = 0; i < (int)images.size(); i++) {
        b2dJsonImage_OpenGL* img = new b2dJsonImage_OpenGL( images[i] );
        m_images.push_back( img );
    }
    reorderImages();
}

void RubeLoader::clear()
{
    for (int i = 0; i < (int)m_images.size(); i++)
    {
        delete m_images[i];
    }
    m_images.clear();

    if ( m_world )
    {
        delete m_world;
    }

    m_world = nullptr;
}

void RubeLoader::step()
{
    if ( m_world ){
        m_world->Step( 1/60.0f, 8, 3 );
    }
}

void RubeLoader::render()
{
    if(m_world)
    {
        m_world->DrawDebugData();
        if ( m_mouseJoint ) 
        {
            b2Vec2 p1 = m_mouseJoint->GetAnchorB();
            b2Vec2 p2 = m_mouseJoint->GetTarget();
            b2Color c;
            c.Set(0.0f, 1.0f, 0.0f);
            glColor3b(c.r, c.g, c.b);

            DrawPoint(Vector2(p2.x,p2.y), 4.0f);

            c.Set(0.8f, 0.8f, 0.8f);
            DrawSegment(p1, p2, c);
        }
        glEnable(GL_BLEND);

        //draw images
        for (int i = 0; i < (int)m_images.size(); i++)
        {
            m_images[i]->render();
        }
    }
}
void RubeLoader::removeImage(b2dJsonImage_OpenGL* image)
{
    for (int i = m_images.size() - 1; i >= 0; i--) 
    {
        b2dJsonImage_OpenGL* img = m_images[i];
        if ( img == image ) 
        {
            m_images.erase( m_images.begin() + i );
            delete img;
        }
    }
}

void RubeLoader::removeBody(b2Body* body)
{
    if ( !body )
        return;

    //destroy the body in the physics world
    m_world->DestroyBody( body );

    //remove all images that were attached to the body we just deleted
    for (int i = m_images.size() - 1; i >= 0; i--) 
    {
        b2dJsonImage_OpenGL* img = m_images[i];
        if ( img->body == body ) 
        {
            m_images.erase( m_images.begin() + i );
            delete img;
        }
    }
}
void RubeLoader::reorderImages()
{
    std::sort(m_images.begin(), m_images.end(), compareImagesByRenderOrder_ascending);
}

RubeLoader::~RubeLoader(void)
{
    this->clear();
}

void RubeLoader::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void RubeLoader::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glVertexPointer(2, GL_FLOAT, 0, vertices);

    glColor4f(color.r, color.g, color.b,0.5f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

    glColor4f(color.r, color.g, color.b,1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void RubeLoader::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
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

void RubeLoader::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
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

void RubeLoader::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    glColor4f(color.r, color.g, color.b,1);
    GLfloat				glVertices[] = {
        p1.x,p1.y,p2.x,p2.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINES, 0, 2);
}

void RubeLoader::DrawTransform(const b2Transform& xf)
{

}