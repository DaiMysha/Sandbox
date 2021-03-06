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
        sf::Vertex* vx = new sf::Vertex[vertexCount+1];
        for(size_t i = 0; i < vertexCount; ++i)
        {
            vx[i].color = sf::Color::Red;
            vx[i].position.x = vertices[i].x;
            vx[i].position.y = vertices[i].y;
        }
        vx[vertexCount] = vx[0];

        _target.draw(vx, vertexCount+1, sf::LinesStrip);

        delete[] vx;
    }

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        DrawPolygon(vertices, vertexCount, color);
    }

    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
        sf::CircleShape circle;
        circle.setPosition(center.x, center.y);
        circle.setRadius(radius);
        circle.setOrigin(radius, radius);
        circle.setOutlineColor(sf::Color(100,100,255));
        circle.setOutlineThickness(1);
        circle.setFillColor(sf::Color(0,0,0,0));

        _target.draw(circle);
    }

    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
    {
        DrawCircle(center, radius, color);
    }

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        sf::Vertex vx[2];
        vx[0].color = sf::Color::Green;
        vx[0].position = {p1.x, p1.y};

        vx[1].color = vx[0].color;
        vx[1].position = {p2.x, p2.y};

        _target.draw(vx, 2, sf::LinesStrip);
    }

    void DrawTransform(const b2Transform& xf)
    {
    }

    private:
        sf::RenderTarget& _target;

  };

void applyFriction(b2Body* body, float32 friction)
{
    return;
    b2Vec2 vel = body->GetLinearVelocity();
    if (vel.Length())
    {
        vel.Normalize();
        vel *= -1 * friction;
        body->ApplyForceToCenter(vel, true);
    }
}

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv)
{
    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Box2D test");

    b2Vec2 newton(0.0f, 5000);

    b2World world(newton);

    float friction = 0.f;

    float32 timeStep = 1.0f / 50.0f;

    int32 velocityIterations = 8;
    int32 positionIterations = 3;

    b2BodyDef myBodyDef;
    myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
    myBodyDef.angularDamping = 5;
    myBodyDef.position.Set(50, 20); //set the starting position
    myBodyDef.angle = 0; //set the starting angle

    b2PolygonShape boxShape;
    boxShape.SetAsBox(20,5);

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    boxFixtureDef.filter.groupIndex = -1;

    b2Body* dynamicBody = world.CreateBody(&myBodyDef);
    dynamicBody->CreateFixture(&boxFixtureDef);

    b2Body* dynamicBody2 = world.CreateBody(&myBodyDef);
    dynamicBody2->CreateFixture(&boxFixtureDef);

    b2Body* dynamicBody3 = world.CreateBody(&myBodyDef);
    dynamicBody3->CreateFixture(&boxFixtureDef);

    b2BodyDef circleBodyDef;
    circleBodyDef.type = b2_staticBody; //this will be a dynamic body
    circleBodyDef.position.Set(WIDTH/2, HEIGHT/4); //set the starting position
    circleBodyDef.angle = 0; //set the starting angle
    b2Body* staticCircle = world.CreateBody(&circleBodyDef);
    b2CircleShape circleShape;
    circleShape.m_radius = 10;
    circleShape.m_type = b2Shape::e_circle;
    b2FixtureDef circleFixtureDef;
    circleFixtureDef.shape = &circleShape;
    circleFixtureDef.density = 1;
    staticCircle->CreateFixture(&circleFixtureDef);

    b2RevoluteJointDef rjDef;
    rjDef.collideConnected = false;
    rjDef.bodyA = staticCircle;
    rjDef.bodyB = dynamicBody;
    rjDef.referenceAngle = 180*DEGTORAD;
    rjDef.localAnchorA = {0, 0};
    rjDef.localAnchorB = {20,0};
    b2Joint* rj = (b2RevoluteJoint*)world.CreateJoint(&rjDef);

    rjDef.localAnchorA = {-20,0};
    rjDef.bodyA = dynamicBody;
    rjDef.bodyB = dynamicBody2;
    b2Joint* rj2 = (b2RevoluteJoint*)world.CreateJoint(&rjDef);
    rjDef.bodyA = dynamicBody2;
    rjDef.bodyB = dynamicBody3;
    b2Joint* rj3 = (b2RevoluteJoint*)world.CreateJoint(&rjDef);


    b2BodyDef groundDef;
    groundDef.type = b2_staticBody; //this will be a dynamic body
    groundDef.position.Set(0, HEIGHT - 10); //set the starting position
    groundDef.angle = 0; //set the starting angle
    boxShape.SetAsBox(WIDTH,10);
    b2FixtureDef groundFixture;
    groundFixture.shape = &boxShape;
    groundFixture.density = 1;
    b2Body* ground = world.CreateBody(&groundDef);
    ground->CreateFixture(&groundFixture);

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

                    case sf::Keyboard::F1:
                        world.Step(timeStep, velocityIterations, positionIterations);
                        break;

                    case sf::Keyboard::F2:
                        staticCircle->SetTransform({WIDTH/4, HEIGHT/4}, 0);
                        break;

                    case sf::Keyboard::F3:
                        staticCircle->SetTransform({WIDTH*3/4, HEIGHT/4}, 0);
                        break;

                    case sf::Keyboard::D:
                        staticCircle->SetTransform(staticCircle->GetPosition() + b2Vec2(10,0), 0);
                        break;

                    case sf::Keyboard::Q:
                        staticCircle->SetTransform(staticCircle->GetPosition() + b2Vec2(-10,0), 0);
                        break;

                    case sf::Keyboard::Z:
                        staticCircle->SetTransform(staticCircle->GetPosition() + b2Vec2(0,-10), 0);
                        break;

                    case sf::Keyboard::S:
                        staticCircle->SetTransform(staticCircle->GetPosition() + b2Vec2(0,10), 0);
                        break;

                    default: break;
                }
            }
            else if(event.type == sf::Event::MouseMoved)
            {
                staticCircle->SetTransform({event.mouseMove.x, event.mouseMove.y}, 0);
            }
        }

        applyFriction(dynamicBody, friction);
        applyFriction(dynamicBody2, friction);
        applyFriction(dynamicBody3, friction);

        world.Step(timeStep, velocityIterations, positionIterations);

        window.clear();
        world.DrawDebugData();
        window.display();

        sf::sleep(sf::milliseconds(1000.0f/50.0f));
    }

    world.DestroyJoint(rj);
    world.DestroyJoint(rj2);
    world.DestroyJoint(rj3);
    world.DestroyBody(staticCircle);
    world.DestroyBody(dynamicBody);
    world.DestroyBody(dynamicBody2);
    world.DestroyBody(dynamicBody3);
    world.DestroyBody(ground);
    return 0;
}
