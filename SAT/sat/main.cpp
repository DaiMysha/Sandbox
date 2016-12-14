#include <iostream>
#include <sstream>
#include <array>

#include <SFML/Graphics.hpp>

#define WIDTH   640
#define HEIGHT  480

const double PI = 3.14159265359;

std::ostream& operator<<(std::ostream& out, const sf::Vector2f& v)
{
    out << v.x << "," << v.y;
    return out;
}

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

class Axis : public sf::Drawable
{
    public:

        Axis(const sf::Vector2f& o = { 0, 0 }, const sf::Vector2f& d = { 0, 0 }, const sf::Color& c = sf::Color::Black)
        {
            origin = o;
            direction = d;
            color = c;
        }

        bool operator!=(const Axis& o) const
        {
            return origin != o.origin || direction != o.direction;
        }

        sf::Vector2f origin;
        sf::Vector2f direction;
        sf::Color color;

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class Segment : public sf::Drawable
{
    public:
        std::array<sf::Vector2f,2> points;
        sf::Color color;

        Segment(const Axis& a) : Segment(a.origin, a.origin + a.direction, a.color)
        {
        }

        Segment(const sf::Vector2f& a = { 0, 0 }, const sf::Vector2f& b = { 0, 0 }, const sf::Color& c = sf::Color::Black)
        {
            points[0] = a;
            points[1] = b;
            color = c;
        }

        Axis getNormal()
        {
            sf::Vector2f d = points[1] - points[0];
            Axis a;
            a.origin = points[0] + d / 2.0f;
            a.direction = normalize(sf::Vector2f(d.y, -d.x));
            a.color = color;
            return a;
        }

        sf::Vector2f getDirection()
        {
            return points[1] - points[0];
        }

        float norm()
        {
            return ::norm(points[1] - points[0]);
        }

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class ProjectedSegment
{
    public:
        ProjectedSegment(const Axis& a = Axis(), float mn = 0.0f, float mx = 0.0f, const sf::Color& c = sf::Color::Black)
        {
            axis = a;
            mini = mn;
            maxi = mx;
            color = c;
        }

        operator Segment() const
        {
            return Segment(axis.origin + mini * axis.direction, axis.origin + maxi * axis.direction, color);
        }

        ProjectedSegment collides(const ProjectedSegment& other)
        {

            float x1 = 0.0, x2 = 0.0f;
            if (other.axis != axis || mini > other.maxi || other.mini > maxi)
            {
                return ProjectedSegment();
            }
            /*std::cout << "Axis : " << axis.origin << " > " << axis.direction << std::endl;
            std::cout << "min/max : " << mini << ">" << maxi
                << " ; "
                << other.mini << ">" << other.maxi
                << std::endl;*/

            x1 = std::max(mini, other.mini);
            x2 = std::min(maxi, other.maxi);

            /*std::cout << "Projection : " << x1 << ">" << x2 << std::endl;
            std::cout << " " << std::endl;*/

            return ProjectedSegment(axis, x1, x2, sf::Color::White);
        }

        float length() const
        {
            return std::abs(maxi - mini);
        }

        Axis axis;
        float mini;
        float maxi;
        sf::Color color;

    protected:
};

void calcNormals(std::array<sf::RectangleShape, 2>& boxes, std::array<Axis, 4>& normals)
{
    int colorReduction = 3;
    {
        sf::Transform t = boxes[0].getTransform();
        {
            Segment s(t.transformPoint(boxes[0].getPoint(0)), t.transformPoint(boxes[0].getPoint(1)), boxes[0].getFillColor());
            s.color.r /= colorReduction;
            s.color.g /= colorReduction;
            s.color.b /= colorReduction;
            normals[0] = s.getNormal();
            //normals[0].origin -= s.getDirection();
        }
        {
            Segment s(t.transformPoint(boxes[0].getPoint(1)), t.transformPoint(boxes[0].getPoint(2)), boxes[0].getFillColor());
            s.color.r /= colorReduction;
            s.color.g /= colorReduction;
            s.color.b /= colorReduction;
            normals[1] = s.getNormal();
            //normals[1].origin -= s.getDirection();
        }
    }

    {
        sf::Transform t = boxes[1].getTransform();
        {
            Segment s(t.transformPoint(boxes[1].getPoint(0)), t.transformPoint(boxes[1].getPoint(1)), boxes[1].getFillColor());
            s.color.r /= colorReduction;
            s.color.g /= colorReduction;
            s.color.b /= colorReduction;
            normals[2] = s.getNormal();
            //normals[2].origin += s.getDirection();
        }
        {
            Segment s(t.transformPoint(boxes[1].getPoint(1)), t.transformPoint(boxes[1].getPoint(2)), boxes[1].getFillColor());
            s.color.r /= colorReduction;
            s.color.g /= colorReduction;
            s.color.b /= colorReduction;
            normals[3] = s.getNormal();
            //normals[3].origin += s.getDirection();
        }
    }
}

//returns distance from axis center to point
float project(const sf::Vector2f& v, const Axis& axis)
{
    return dot(v-axis.origin, axis.direction);
}

ProjectedSegment project(const sf::RectangleShape& box, const Axis& axis)
{
    //std::cout << "Axis : " << axis.origin << ">" << axis.direction << std::endl;
    const sf::Transform& t = box.getTransform();
    std::array<float, 4> projections;
    for (size_t i = 0; i < 4; ++i)
    {
        projections[i] = project(t.transformPoint(box.getPoint(i)), axis);
        //std::cout << "projections[" << t.transformPoint(box.getPoint(i)) << "] = " << projections[i] << std::endl;
    }

    float mini = projections[0], maxi = projections[0];
    for (size_t i = 1; i < 4; ++i)
    {
        if (projections[i] < mini)
        {
            mini = projections[i];
        }
        else if(projections[i] > maxi)
        {
            maxi = projections[i];
        }
    }

    //std::cout << "***********" << std::endl;
    /*std::cout << "Projection of box " 
        << box.getPoint(0) << ">" << box.getPoint(1) 
        << "on axis " << axis.origin << " > " << axis.direction
        << " = " << mini << ">" << maxi         
        << std::endl;*/

    return ProjectedSegment(axis, mini, maxi, sf::Color::White);
}

void calcProjections(std::array<ProjectedSegment, 8>& projections, std::array<sf::RectangleShape, 2>& boxes, std::array<Axis, 4>& normals)
{
    for (size_t b = 0; b < boxes.size(); ++b)
    {
        for (size_t n = 0; n < normals.size(); ++n)
        {
            projections[b * 4 + n] = project(boxes[b], normals[n]);
            projections[b * 4 + n].color = boxes[b].getFillColor() + sf::Color(50, 50, 50);;
        }
    }
}

int main(int argc, char** argv)
{
    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SAT test");

    std::array<sf::RectangleShape, 2> boxes;
    boxes[0].setFillColor(sf::Color::Blue);
    boxes[0].setPosition(150, 150);
    boxes[0].setSize({ 150, 50 });
    //boxes[0].setRotation(30);

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

    //SAT related stuff
    std::array<Axis, 4> normals;
    std::array<ProjectedSegment, 8> projections;
    std::array<ProjectedSegment, 4> collisions;

    calcNormals(boxes, normals);
    calcProjections(projections, boxes, normals);
    ProjectedSegment collisionVector;
    collisionVector.color = sf::Color::Black;

    sf::Clock fpsTest;
    size_t frames = 0;

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
                        boxes[affectedBox].setRotation(angle * 180.0f / (float)PI);
                    }
                    calcNormals(boxes, normals);
                    calcProjections(projections, boxes, normals);
                    for (size_t i = 0; i < collisions.size(); ++i)
                    {
                        collisions[i] = projections[i].collides(projections[i+normals.size()]);
                    }
                }
            }
        }

        //check collisions
        size_t minCollision = 0;
        float l = 0.0f;
        for (size_t i = 1; i < collisions.size(); ++i)
        {
            if (collisions[i].length() < collisions[minCollision].length())
            {
                minCollision = i;
            }
        }

        if (collisions[minCollision].length() != 0.0f)
        {
            l = collisions[minCollision].length();
        }
        collisionVector = ProjectedSegment(collisions[minCollision].axis, 0, l, sf::Color(255,127,15));
        collisionVector.axis.origin = boxes[0].getPosition();

        window.clear({ 127, 127, 127 });
        for (size_t i = 0; i < boxes.size(); ++i)
        {
            drawBox(window, boxes[i]);
        }
        /*for (size_t i = 0; i < normals.size(); ++i)
        {
            window.draw(normals[i]);
        }
        for (size_t i = 0; i < projections.size(); ++i)
        {
            window.draw((Segment)projections[i]);
        }
        for (size_t i = 0; i < collisions.size(); ++i)
        {
            window.draw((Segment)collisions[i]);
        }*/
        window.draw((Segment)collisionVector);
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

void Axis::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Vertex v[2];
    v[0] = { origin - direction * 20000.0f, color };
    v[1] = { origin + direction * 20000.0f, color };

    target.draw(v, 2, sf::Lines, states);
}

void Segment::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Vertex v[2];
    v[0] = { points[0], color };
    v[1] = { points[1], color };

    target.draw(v, 2, sf::Lines, states);
}