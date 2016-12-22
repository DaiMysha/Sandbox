#include <iostream>
#include <sstream>
#include <array>

#include <SFML/Graphics.hpp>

#define WIDTH   640
#define HEIGHT  480

std::ostream& operator<<(std::ostream& out, const sf::Vector2f& v)
{
    out << v.x << "," << v.y;
    return out;
}

void drawShape(sf::RenderTarget& window, const sf::ConvexShape& shape)
{
    sf::Vertex* points = new sf::Vertex[shape.getPointCount()+1];

    auto t = shape.getTransform();

    for (int i = 0; i < shape.getPointCount(); ++i)
    {
        points[i] = { t.transformPoint(shape.getPoint(i)), shape.getFillColor() };
    }
    points[shape.getPointCount()] = points[0];

    window.draw(points, shape.getPointCount()+1, sf::LinesStrip);

    delete[] points;
}

void drawLine(sf::RenderTarget& window, const sf::Vector2f& p1, const sf::Vector2f& p2, sf::Color c)
{
    sf::Vertex points[3];

    points[0] = { p1, c };
    points[1] = { p2, c };

    points[2] = points[0];

    window.draw(points, 3, sf::LinesStrip);

    delete[] points;
}

template <typename T>
float getAngleBetweenVectors(const sf::Vector2<T>& o, const sf::Vector2<T>& v) {
    return (float)-(atan2(static_cast<double>(o.y), static_cast<double>(o.x)) - atan2(static_cast<double>(v.y), static_cast<double>(v.x)));
}

template <typename T>
float norm2(const sf::Vector2<T>& a)
{
    return static_cast<float>(a.x*a.x + a.y*a.y);
}

template <typename T>
float norm(const sf::Vector2<T>& a)
{
    return sqrt(norm2(a));
}

template <typename T>
sf::Vector2<T> normalize(const sf::Vector2<T>& vec)
{
    T n = norm(vec);
    return sf::Vector2<T>(T(vec.x / n), T(vec.y / n));
}

template <typename T>
T dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
    return a.x*b.x + a.y*b.y;
}

//line define by P0>P1
//tests if P2 is left
///returns >0 if left, 0 if on the line, <0 if right
template <typename T>
T isLeft(const sf::Vector2<T>& P0, const sf::Vector2<T>& P1, const sf::Vector2<T>& P2)
{
    return ( (P1.x - P0.x) * (P2.y - P0.y) - (P2.x -  P0.x) * (P1.y - P0.y) );
}

bool contains(const sf::ConvexShape& shape, sf::Vector2f point)
{
    size_t minyi = 0, maxyi = 0;
    float miny = shape.getPoint(0).y;
    float maxy = miny;

    point = point - shape.getPosition();

    for(size_t i = 0; i < shape.getPointCount(); ++i)
    {
        float y = shape.getPoint(i).y;
        if(y < miny)
        {
            minyi = i;
            miny = y;
        }
        else if(y > maxy)
        {
            maxyi = i;
            maxy = y;
        }
    }

    //std::cout << "min/max : " << miny << "/" << maxy << " / " << point.y << std::endl;

    if(point.y < miny || point.y > maxy)
    {
        return false;
    }

    //find the two segments that surround the point in y axis
    //first going right side
    size_t rightSide1 = minyi, leftSide1 = minyi;
    size_t rightSide2 = minyi, leftSide2 = minyi;
    int i = minyi;
    int ip1;
    while(i != maxyi)
    {
        if(i >= shape.getPointCount())
        {
            i = 0;
        }
        ip1 = i+1;
        if(ip1 >= shape.getPointCount())
        {
            ip1 = 0;
        }

        if(point.y > shape.getPoint(i).y && point.y < shape.getPoint(ip1).y)
        {
            rightSide1 = i;
            rightSide2 = ip1;
        }
        ++i;
    }

    //leftSide
    i = minyi;
    while(i != maxyi)
    {
        if(i < 0)
        {
            i = shape.getPointCount()-1;
        }
        ip1 = i-1;
        if(ip1 < 0)
        {
            ip1 = shape.getPointCount()-1;
        }

        if(point.y > shape.getPoint(i).y && point.y < shape.getPoint(ip1).y)
        {
            leftSide1 = ip1;
            leftSide2 = i;
        }
        --i;
    }

    return isLeft(shape.getPoint(rightSide1), shape.getPoint(rightSide2), point) >= 0
        && isLeft(shape.getPoint(leftSide1), shape.getPoint(leftSide2), point) >= 0;
}

int main(int argc, char** argv)
{
    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Polygon inclusion test");

    sf::Clock fpsTest;
    size_t frames = 0;

    sf::Vector2f mouseClick;

    sf::ConvexShape shape;

    shape.setFillColor(sf::Color::Red);
    shape.setPointCount(5);
    shape.setPoint(0, sf::Vector2f(40,-140));
    shape.setPoint(1, sf::Vector2f(120,-40));
    shape.setPoint(2, sf::Vector2f(60,100));
    shape.setPoint(3, sf::Vector2f(-90,50));
    shape.setPoint(4, sf::Vector2f(60,-80));
    shape.setPosition(WIDTH/2, HEIGHT/2);

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
            /*else if (event.type == sf::Event::MouseButtonPressed)
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
                        boxes[affectedBox].setRotation(angle * 180.0f / (float)PI);
                    }
                    calcNormals(boxes, normals);
                    calcProjections(projections, boxes, normals);
                    for (size_t i = 0; i < collisions.size(); ++i)
                    {
                        collisions[i] = projections[i].collides(projections[i+normals.size()]);
                    }
                }
            }*/
        }

        sf::Vector2f mouse(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

        sf::Color c(sf::Color::Blue);
        if(contains(shape, mouse))
        {
            c = sf::Color::Green;
        }

        window.clear({ 127, 127, 127 });
        drawShape(window, shape);
        drawLine(window, sf::Vector2f(WIDTH/2, HEIGHT/2), mouse, c);
        window.display();

        ++frames;
        if (fpsTest.getElapsedTime().asMilliseconds() > 500)
        {
            std::cout << "fps : " << frames * 2 << std::endl;
            fpsTest.restart();
            frames = 0;
        }

    }

    return 0;
}
