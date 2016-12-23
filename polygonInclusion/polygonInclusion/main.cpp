#include <iostream>
#include <sstream>
#include <array>

#include <SFML/Graphics.hpp>

#define WIDTH   640
#define HEIGHT  480

std::array<sf::Color, 6> colors;

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
        points[i] = { t.transformPoint(shape.getPoint(i)), i < colors.size()?colors[i]:shape.getFillColor() };
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
T isLeft(const sf::Vector2<T>& p0, const sf::Vector2<T>& p1, const sf::Vector2<T>& p2)
{
    if(p0.y < p1.y)
        return ( (p1.x - p0.x) * (p2.y - p0.y) - (p2.x -  p0.x) * (p1.y - p0.y) );
    else
        return ( (p0.x - p1.x) * (p2.y - p1.y) - (p2.x -  p1.x) * (p0.y - p1.y) );
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
        ip1 = i+1;
        if(ip1 >= shape.getPointCount())
        {
            ip1 = 0;
        }

        int minYtmp, maxYtmp;
        if(shape.getPoint(i).y < shape.getPoint(ip1).y)
        {
            minYtmp = i;
            maxYtmp = ip1;
        }
        else
        {
            maxYtmp = i;
            minYtmp = ip1;
        }

        if(point.y >= shape.getPoint(minYtmp).y && point.y <= shape.getPoint(maxYtmp).y)
        {
            rightSide1 = minYtmp;
            rightSide2 = maxYtmp;
        }
        ++i;
        if(i >= shape.getPointCount())
        {
            i = 0;
        }
    }

    //leftSide
    i = minyi;
    while(i != maxyi)
    {
        ip1 = i-1;
        if(ip1 < 0)
        {
            ip1 = shape.getPointCount()-1;
        }

        int minYtmp, maxYtmp;
        if(shape.getPoint(i).y < shape.getPoint(ip1).y)
        {
            minYtmp = i;
            maxYtmp = ip1;
        }
        else
        {
            maxYtmp = i;
            minYtmp = ip1;
        }

        if(point.y >= shape.getPoint(minYtmp).y && point.y <= shape.getPoint(maxYtmp).y)
        {
            leftSide1 = minYtmp;
            leftSide2 = maxYtmp;
        }
        --i;
        if(i < 0)
        {
            i = shape.getPointCount()-1;
        }
    }

    return (isLeft(shape.getPoint(rightSide1), shape.getPoint(rightSide2), point) >= 0
        && isLeft(shape.getPoint(leftSide1), shape.getPoint(leftSide2), point) <= 0)
        ||
        (isLeft(shape.getPoint(rightSide1), shape.getPoint(rightSide2), point) <= 0
        && isLeft(shape.getPoint(leftSide1), shape.getPoint(leftSide2), point) >= 0);
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
    colors[0] = sf::Color::Green;
    colors[1] = sf::Color::Blue;
    colors[2] = sf::Color::Red;
    colors[3] = sf::Color::Red;
    colors[4] = sf::Color::Red;
    colors[5] = sf::Color::Red;
    shape.setPointCount(6);
    bool cw = false;
    if(cw)
    {
        shape.setPoint(0, sf::Vector2f(40,-140));
        shape.setPoint(1, sf::Vector2f(120,-40));
        shape.setPoint(2, sf::Vector2f(120,0));
        shape.setPoint(3, sf::Vector2f(60,100));
        shape.setPoint(4, sf::Vector2f(-90,50));
        shape.setPoint(5, sf::Vector2f(60,-80));
    }
    else
    {
        shape.setPoint(5, sf::Vector2f(40,-140));
        shape.setPoint(4, sf::Vector2f(120,-40));
        shape.setPoint(3, sf::Vector2f(120,0));
        shape.setPoint(2, sf::Vector2f(60,100));
        shape.setPoint(1, sf::Vector2f(-90,50));
        shape.setPoint(0, sf::Vector2f(60,-80));
    }
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
        }

        sf::Vector2f mouse(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

        sf::Color c(sf::Color::Blue);
        if(contains(shape, mouse))
        {
            c = sf::Color::Green;
        }

        window.clear({ 127, 127, 127 });
        sf::Color c2 = shape.getFillColor();
        if(isLeft(shape.getPoint(0) + shape.getPosition(), shape.getPoint(1) + shape.getPosition(), mouse)>0)
        {
            c2 = sf::Color::White;
        }
        drawShape(window, shape);
        drawLine(window, sf::Vector2f(WIDTH/2, HEIGHT/2), mouse, c);
        window.display();

        sf::sleep(sf::milliseconds(16));

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
