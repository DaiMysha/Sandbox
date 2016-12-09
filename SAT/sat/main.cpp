#include <iostream>
#include <sstream>
#include <array>

#include <SFML/Graphics.hpp>

#define WIDTH   640
#define HEIGHT  480

const double PI = 3.14159265359;

void drawBox(sf::RenderTarget& window, const sf::RectangleShape& box)
{
    sf::Vertex points[5];

    auto t = box.getTransform();
    
    for (int i = 0; i < 4; ++i)
    {
        points[i] = { t.transformPoint(box.getPoint(i)), box.getFillColor() };
    }
    points[4] = points[0];

    window.draw(points, 5, sf::LinesStrip);
}

template <typename T>
float getAngleBetweenVectors(const sf::Vector2<T>& o, const sf::Vector2<T>& v) {
    return -(atan2(static_cast<double>(o.y), static_cast<double>(o.x)) - atan2(static_cast<double>(v.y), static_cast<double>(v.x)));
}

int main(int argc, char** argv)
{

    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SAT test");

    std::array<sf::RectangleShape, 2> boxes;
    boxes[0].setFillColor(sf::Color::Blue);
    boxes[0].setPosition(150, 50);
    boxes[0].setSize({ 150, 50 });
    boxes[0].setRotation(30);

    boxes[1].setFillColor(sf::Color::Red);
    boxes[1].setPosition(300, 250);
    boxes[1].setSize({ 150, 50 });

    for (size_t i = 0; i < boxes.size(); ++i)
    {
        boxes[i].setOrigin(boxes[i].getSize() / 2.0f);
    }

    sf::Vector2f mouseClick;
    bool dragging = false;
    bool rotating = false;
    int affectedBox = -1;

    sf::Vector2f xVector(1.0f, 0.0f);

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
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                mouseClick = { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };

                for (size_t i = 0; i < boxes.size(); ++i)
                {
                    if (affectedBox == -1 && boxes[i].getGlobalBounds().contains(mouseClick.x, mouseClick.y ))
                    {
                        affectedBox = i;
                    }
                }
                if (affectedBox != -1)
                {
                    switch (event.mouseButton.button)
                    {
                    case sf::Mouse::Left://move
                        dragging = true;
                        break;
                    case sf::Mouse::Right://move
                        rotating = true;
                        break;
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                affectedBox = -1;
                dragging = false;
                rotating = false;
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                if (affectedBox != -1)
                {
                    if (dragging)
                    {
                        sf::Vector2f newPos = boxes[affectedBox].getPosition();
                        newPos = { newPos.x + event.mouseMove.x - mouseClick.x, newPos.y + event.mouseMove.y - mouseClick.y };
                        boxes[affectedBox].setPosition(newPos);
                        mouseClick = { (float)event.mouseMove.x, (float)event.mouseMove.y };
                    }
                    else if (rotating)
                    {
                        mouseClick = { (float)event.mouseMove.x, (float)event.mouseMove.y };
                        float angle = getAngleBetweenVectors(xVector, mouseClick - boxes[affectedBox].getPosition());
                        boxes[affectedBox].setRotation(angle * 180.0f / PI);
                    }
                }
            }
        }

        window.clear({ 127, 127, 127 });
        for (size_t i = 0; i < boxes.size(); ++i)
        {
            drawBox(window, boxes[i]);
        }
        window.display();

        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}
