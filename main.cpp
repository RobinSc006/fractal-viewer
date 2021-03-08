#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <math.h>
#include <iostream>
#include <string>
#include <fstream>

float map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

bool set_waypoint(float x_pos, float y_pos, float zoom, int key)
{
    std::string x_string = std::to_string(x_pos);
    std::string y_string = std::to_string(y_pos);

    std::ofstream file("waypoints/" + std::to_string(key) + ".wp");
    if (file.is_open())
    {
        file << x_string << std::endl;
        file << y_string << std::endl;
        file << zoom;
        file.close();
        return true;
    }
    else
    {
        return false;
    }
    /* Life's goooood. Man, this is my absolute favorite project ever!*/
}

bool load_waypoint(int key, float &x_trans, float &y_trans, float &zoom)
{
    std::string line;
    std::ifstream myfile("waypoints/" + std::to_string(key) + ".wp");

    int counter = 0;

    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if (counter == 0)
                x_trans = std::stof(line);
            else if (counter == 1)
                y_trans = std::stof(line);
            else if (counter == 2)
                zoom = std::stof(line);
            counter++;
        }
        myfile.close();
        return true;
    }
    else
        return false;
}

int main()
{
    sf::Font font;
    if (!font.loadFromFile("Roboto-Thin.ttf"))
        std::cout << "unable to load font\n";

    const int font_size = 15;
    const int font_outline_thiccness = 1;

    const int WIDTH = 700;
    const int HEIGHT = 700;

    const float MAX_ZOOM = 0.004;
    const float MIN_ZOOM = 5.0;
    const float DEFAULT_ZOOM = 2.5;
    const float DEFAULT_X_POS = 0;
    const float DEFAULT_Y_POS = 0;

    const float POSITION_INCREMENT = 0.05;
    const float ZOOM_INCREMENT = 0.0001;
    const float ZOOM_INCREMENT_LARGE = 0.005;

    int max_iterations = 445;

    float zoom = DEFAULT_ZOOM;
    float compX = DEFAULT_X_POS;
    float compY = DEFAULT_Y_POS;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fractal generator");

    sf::Shader shader;
    if (!shader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment))
        std::cout << "failed to load shader\n";

    shader.setUniform("window_width", WIDTH);
    shader.setUniform("window_height", HEIGHT);
    shader.setUniform("iterations", 100);
    shader.setUniform("color", 5);

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Image img;
    img.create(WIDTH, HEIGHT, sf::Color::Black);

    sf::Text imag_xpos_text;
    sf::Text imag_ypos_text;
    sf::Text zoom_text;
    sf::Text iterations_text;

    imag_xpos_text.setFont(font);
    imag_ypos_text.setFont(font);
    zoom_text.setFont(font);
    iterations_text.setFont(font);

    imag_xpos_text.setCharacterSize(font_size);
    imag_ypos_text.setCharacterSize(font_size);
    zoom_text.setCharacterSize(font_size);
    iterations_text.setCharacterSize(font_size);

    imag_xpos_text.setFillColor(sf::Color::White);
    imag_ypos_text.setFillColor(sf::Color::White);
    zoom_text.setFillColor(sf::Color::White);
    iterations_text.setFillColor(sf::Color::White);

    imag_xpos_text.setOutlineColor(sf::Color::Black);
    imag_ypos_text.setOutlineColor(sf::Color::Black);
    zoom_text.setOutlineColor(sf::Color::Black);
    iterations_text.setOutlineColor(sf::Color::Black);

    imag_xpos_text.setOutlineThickness(font_outline_thiccness);
    imag_ypos_text.setOutlineThickness(font_outline_thiccness);
    zoom_text.setOutlineThickness(font_outline_thiccness);
    iterations_text.setOutlineThickness(font_outline_thiccness);

    imag_xpos_text.setPosition(10, 20);
    imag_ypos_text.setPosition(10, 40);
    zoom_text.setPosition(10, 60);
    iterations_text.setPosition(10, 80);

    texture.create(WIDTH, HEIGHT);
    texture.loadFromImage(img);
    sprite.setTexture(texture);

    while (window.isOpen())
    {
        imag_xpos_text.setString("Complex X: " + std::to_string(compX));
        imag_ypos_text.setString("Complex Y: " + std::to_string(compY));
        zoom_text.setString("Detail: " + std::to_string(zoom));
        iterations_text.setString("Iterations: " + std::to_string(max_iterations));

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                compY -= POSITION_INCREMENT * zoom * zoom;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                compY += POSITION_INCREMENT * zoom * zoom;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                compX -= POSITION_INCREMENT * zoom * zoom;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                compX += POSITION_INCREMENT * zoom * zoom;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            {
                max_iterations -= 5;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                max_iterations += 5;
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A)) && (zoom <= MIN_ZOOM))
            {
                zoom += ZOOM_INCREMENT * zoom;
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D)) && (zoom >= MAX_ZOOM))
            {
                zoom -= ZOOM_INCREMENT * zoom;
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && (zoom <= MIN_ZOOM))
            {
                zoom += ZOOM_INCREMENT_LARGE * zoom / 2;
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && (zoom >= MAX_ZOOM))
            {
                zoom -= ZOOM_INCREMENT_LARGE * zoom / 2;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
            {
                shader.setUniform("color", 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
            {
                shader.setUniform("color", 1);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
            {
                shader.setUniform("color", 2);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
            {
                shader.setUniform("color", 3);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
            {
                shader.setUniform("color", 4);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
            {
                shader.setUniform("color", 5);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    set_waypoint(compX, compY, zoom, 0);
                }
                else
                {
                    load_waypoint(0, compX, compY, zoom);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    set_waypoint(compX, compY, zoom, 1);
                }
                else
                {
                    load_waypoint(1, compX, compY, zoom);
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    set_waypoint(compX, compY, zoom, 2);
                }
                else
                {
                    load_waypoint(2, compX, compY, zoom);
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            {
                compX = DEFAULT_X_POS;
                compY = DEFAULT_Y_POS;
                zoom = DEFAULT_ZOOM;
            }
        }

        shader.setUniform("x_translation", compX);
        shader.setUniform("y_translation", compY);
        shader.setUniform("zoom", zoom * zoom);
        shader.setUniform("iterations", max_iterations);

        window.clear();

        window.draw(sprite, &shader);

        window.draw(imag_xpos_text);
        window.draw(imag_ypos_text);
        window.draw(zoom_text);
        window.draw(iterations_text);

        window.display();
    }

    return 0;
}