#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define WIDTH   640
#define HEIGHT  480

#define M_PI 3.14159265359

class PhysicBox : public sf::Drawable
{
    public:

        PhysicBox() : _body(nullptr), _dynamic(true)
        {
        }

        void initBox(const sf::Vector2f& position, const sf::Vector2f& halfSize)
        {
            _bodyDef.position.Set(position.x, position.y);

            _bodyShape.SetAsBox(halfSize.x, halfSize.y);

            _bodyVisual.setSize(halfSize * 2.0f);
            _bodyVisual.setOrigin(halfSize);
            _bodyVisual.setPosition(position);
        }

        void initPhysics(b2World& world, float density, float friction)
        {
            if (_dynamic)
            {
                _bodyDef.type = b2_dynamicBody;
            }
            else
            {
                _bodyDef.type = b2_staticBody;
            }
            _bodyDef.fixedRotation = true;

            if (_body)
            {
                world.DestroyBody(_body);
            }
            _body = world.CreateBody(&_bodyDef);

            if (_dynamic)
            {
                _fixture.density = density;
                _fixture.friction = friction;
                _fixture.restitution = 0.2f;
                _fixture.shape = &_bodyShape;
                _body->CreateFixture(&_fixture);
            }
            else
            {
                _body->CreateFixture(&_bodyShape, 0.0f);
            }
        }

        void update()
        {
            if (!_body) return;
            b2Vec2 position = _body->GetPosition();
            float32 angle = _body->GetAngle() * 180.0f / M_PI*1.0f;

            _bodyVisual.setRotation(angle);
            _bodyVisual.setPosition(position.x, position.y);
        }

        void setColor(const sf::Color& c)
        {
            _bodyVisual.setFillColor(c);
        }

        b2Body* _body;
        sf::RectangleShape _bodyVisual;
        bool _dynamic;

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            target.draw(_bodyVisual, states);
        }

        b2BodyDef _bodyDef;
        b2PolygonShape _bodyShape;
        b2FixtureDef _fixture;
};

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv)
{

    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Box2D test");

    b2Vec2 newton(0.0f, 0.0f);

    b2World world(newton);

    float32 timeStep = 1.0f / 60.0f;

    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    /*PhysicBox ground;
    ground._dynamic = false;
    ground.setColor({ 185, 122, 87 });
    ground.initBox({ WIDTH / 2.0f, HEIGHT - 50.0f }, { 100.0f, 10.0f });
    ground.initPhysics(world, 0.0f, 0.0f);*/

    PhysicBox borders[4];
    borders[0].initBox({ WIDTH / 2.0f, 0.0f }, { WIDTH / 2.0f, 10.0f });
    borders[1].initBox({ WIDTH, HEIGHT / 2.0f }, { 10.0f, HEIGHT / 2.0f });
    borders[2].initBox({ WIDTH / 2.0f, HEIGHT }, { WIDTH / 2.0f, 10.0f });
    borders[3].initBox({ 0.0f, HEIGHT / 2.0f }, { 10.0f, HEIGHT / 2.0f });
    for (int i = 0; i < 4; ++i)
    {
        borders[i]._dynamic = false;
        borders[i].setColor({ 185, 122, 87 });
        borders[i].initPhysics(world, 0.0f, 0.0f);
    }

    PhysicBox box1;
    box1.setColor(sf::Color::Red);
    box1.initBox({ WIDTH / 2.0f, 50.0f }, { 25.0f, 25.0f });
    box1.initPhysics(world, 0.1f, 0.1f);
    //box1._body->SetAngularVelocity(3.14159f);
    //box1._body->SetLinearVelocity({ 10.0f, 5.0f });
    //box1._body->ApplyLinearImpulseToCenter({ 0.0f, 50000.0f }, true);

    PhysicBox box2;
    box2.setColor(sf::Color::Green);
    box2.initBox({ WIDTH / 2.0f - 150, 100.0f }, { 25.0f, 25.0f });
    box2.initPhysics(world, 0.1f, 0.1);
    //box2._body->SetAngularVelocity(3.14159f);
    //box2._body->SetLinearVelocity({ 100.0f, -50.0f });
    //box2._body->ApplyLinearImpulseToCenter({ 50000.0f, 0.0f }, true);

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
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Z:
                        box1._body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -500.0f), true);
                        break;
                    case sf::Keyboard::S:
                        box1._body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, 500.0f), true);
                        break;
                    case sf::Keyboard::Q:
                        box1._body->ApplyLinearImpulseToCenter(b2Vec2(-500.0f, 0.0f), true);
                        break;
                    case sf::Keyboard::D:
                        box1._body->ApplyLinearImpulseToCenter(b2Vec2(500.0f, 0.0f), true);
                        break;
                    case sf::Keyboard::Up:
                        box2._body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -500.0f), true);
                        break;
                    case sf::Keyboard::Down:
                        box2._body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, 500.0f), true);
                        break;
                    case sf::Keyboard::Left:
                        box2._body->ApplyLinearImpulseToCenter(b2Vec2(-500.0f, 0.0f), true);
                        break;
                    case sf::Keyboard::Right:
                        box2._body->ApplyLinearImpulseToCenter(b2Vec2(500.0f, 0.0f), true);
                        break;
                    default: break;
                }
            }
        }

        world.Step(timeStep, velocityIterations, positionIterations);

        box1.update();
        box2.update();

        window.clear({ 127, 127, 127 });
        //window.draw(ground);
        for (int i = 0; i < 4; ++i) window.draw(borders[i]);
        window.draw(box1);
        window.draw(box2);
        window.display();

        sf::sleep(sf::milliseconds(16));
    }


    return 0;
}
