#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

namespace {

const unsigned WIDTH = 640;
const unsigned HEIGHT = 480;
const double PI = 3.14159265359;

} // !namespace

class DebugDraw : public b2Draw
  {
  public:
    DebugDraw(sf::RenderTarget& tgt) : b2Draw(), _target(tgt)
    {
    }

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        sf::Color c(color.r, color.g, color.b, color.a);
        sf::Vertex* vx = new sf::Vertex[vertexCount];
        for(size_t i = 0; i < vertexCount; ++i)
        {
            vx[i].color = sf::Color::Red;
            vx[i].position.x = vertices[i].x;
            vx[i].position.y = vertices[i].y;

            std::cout << vertices[i].x <<";" << vertices[i].y << ";" << (int)c.b << std::endl;
        }
        std::cout << std::endl;

        _target.draw(vx, vertexCount, sf::LinesStrip);

        delete[] vx;
    }

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        DrawPolygon(vertices, vertexCount, color);
    }

    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
    }

    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
    {
        DrawCircle(center, radius, color);
    }

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
    }

    void DrawTransform(const b2Transform& xf)
    {
    }

    private:
        sf::RenderTarget& _target;

  };

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv)
{
    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Box2D test");

    b2Vec2 newton(0.0f, 9.80f);

    b2World world(newton);

    float32 timeStep = 1.0f / 50.0f;

    int32 velocityIterations = 8;
    int32 positionIterations = 3;

    b2BodyDef myBodyDef;
    myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
    myBodyDef.position.Set(100, 20); //set the starting position
    myBodyDef.angle = 0; //set the starting angle

    b2Body* dynamicBody = world.CreateBody(&myBodyDef);
    b2PolygonShape boxShape;
    boxShape.SetAsBox(100,100);

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    dynamicBody->CreateFixture(&boxFixtureDef);

    /*
    b2RevoluteJointDef rjDef;
    rjDef.bodyA = box1._body;
    rjDef.bodyB = box2._body;
    rjDef.localAnchorA = b2Vec2(60.0f, 10.0f);
    rjDef.localAnchorB = b2Vec2(0.0f, 10.0f);
    b2Joint* rj = (b2RevoluteJoint*)world.CreateJoint(&rjDef);
    */

    DebugDraw dbd(window);
    dbd.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
    world.SetDebugDraw(&dbd);

    //the loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
            {
                bool down = (event.type == sf::Event::KeyPressed);
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;

                    default: break;
                }
            }
        }

        world.Step(timeStep, velocityIterations, positionIterations);

        window.clear();
        world.DrawDebugData();
        window.display();

        sf::sleep(sf::milliseconds(16));
    }

    world.DestroyBody(dynamicBody);
    //world.DestroyJoint(rj);
    return 0;
}
