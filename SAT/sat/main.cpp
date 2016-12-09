#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

#define WIDTH   640
#define HEIGHT  480

#define M_PI 3.14159265359

int main(int argc, char** argv)
{

    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SAT test");

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

        window.clear({ 127, 127, 127 });

        window.display();

        sf::sleep(sf::milliseconds(16));
    }


    return 0;
}
