#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define WIDTH   640
#define HEIGHT  480

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv) {

    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Box2D test");

    b2Vec2 newton(0.0f, 98.0f);

    b2World world(newton);

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(WIDTH/2.0f, HEIGHT-50.0f);

    //ground
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);

    sf::RectangleShape groundShape;
    groundShape.setFillColor({ 185, 122, 87 });
    groundShape.setSize({ 100, 20.0f });
    groundShape.setPosition(WIDTH / 2.0f, HEIGHT - 50.0f);
    groundShape.setOrigin(groundShape.getSize() / 2.0f);

    //object 1
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(WIDTH / 2.0f, 50.0f);
    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(25, 25);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    body->CreateFixture(&fixtureDef);

    sf::RectangleShape shape;
    shape.setFillColor(sf::Color::Blue);
    shape.setSize({ 50, 50 });
    shape.setOrigin(shape.getSize() / 2.0f);
    body->SetAngularVelocity(3.14159f);
    body->SetLinearVelocity({ 10.0f, 5.0f });

    //object 2
    b2BodyDef bodyDef2;
    bodyDef2.type = b2_dynamicBody;
    bodyDef2.position.Set(WIDTH / 2.0f - 150, 100.0f);
    b2Body* body2 = world.CreateBody(&bodyDef2);

    b2PolygonShape dynamicBox2;
    dynamicBox2.SetAsBox(25, 25);

    b2FixtureDef fixtureDef2;
    fixtureDef2.shape = &dynamicBox2;
    fixtureDef2.density = 1.0f;
    fixtureDef2.friction = 0.3f;

    body2->CreateFixture(&fixtureDef2);

    sf::RectangleShape shape2;
    shape2.setFillColor(sf::Color::Green);
    shape2.setSize({ 50, 50 });
    shape2.setOrigin(shape2.getSize() / 2.0f);
    body2->SetAngularVelocity(3.14159f);
    body2->SetLinearVelocity({ 100.0f, -50.0f });

    float32 timeStep = 1.0f / 60.0f;

    int32 velocityIterations = 6;
    int32 positionIterations = 2;

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
                    default: break;
                }
            }
        }

        world.Step(timeStep, velocityIterations, positionIterations);

        b2Vec2 position = body->GetPosition();
        float32 angle = body->GetAngle() * 180.0f / 3.14159f;

        shape.setRotation(angle);
        shape.setPosition(position.x, position.y);

        position = body2->GetPosition();
        angle = body2->GetAngle() * 180.0f / 3.14159f;

        shape2.setRotation(angle);
        shape2.setPosition(position.x, position.y);



        window.clear({ 127, 127, 127 });
        window.draw(groundShape);
        window.draw(shape);
        window.draw(shape2);
        window.display();
        sf::sleep(sf::milliseconds(16));
    }


    return 0;
}
