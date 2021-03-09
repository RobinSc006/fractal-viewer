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

bool set_waypoint(float x_pos, float y_pos, double zoom, int key)
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
    // ? Life's goooood. Man, this is my absolute favorite project ever!
}

bool load_waypoint(int key, float &x_trans, float &y_trans, double &zoom)
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
                zoom = std::stod(line);
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

    const int font_size = 16;
    const int font_outline_thiccness = 1;

    const int WIDTH = 888;
    const int HEIGHT = 888;

    const double MAX_ZOOM = 0.000001;
    const float MIN_ZOOM = 5.0;
    const double DEFAULT_ZOOM = 2.5;
    const float DEFAULT_X_POS = 0;
    const float DEFAULT_Y_POS = 0;
    const int DEFAULT_MAX_ITERATIONS = 455;
    const float MIN_POS_INCREMENT = 0.00004;

    const float POSITION_INCREMENT = 0.0025;
    const float POSITION_INCREMENT_LARGE = 0.1;
    const float ZOOM_INCREMENT = 0.0001;
    const float ZOOM_INCREMENT_LARGE = 0.005;

    int max_iterations = DEFAULT_MAX_ITERATIONS;

    double zoom = DEFAULT_ZOOM;
    float compX = DEFAULT_X_POS;
    float compY = DEFAULT_Y_POS;

    bool auto_iterations = false;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fractal generator");

    float fps;
    sf::Clock clock = sf::Clock();
    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;

    sf::Shader shader;
    if (!shader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment))
        std::cout << "failed to load shader\n";

    shader.setUniform("window_width", WIDTH);
    shader.setUniform("window_height", HEIGHT);
    shader.setUniform("iterations", 100);
    shader.setUniform("color", 5);

    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape img;
    img.setSize(sf::Vector2f(WIDTH, HEIGHT));

    //img.create(WIDTH, HEIGHT, sf::Color::Black);

    sf::CircleShape crosshair(1.5);
    crosshair.setFillColor(sf::Color::White);
    crosshair.setPosition(WIDTH / 2, HEIGHT / 2);
    bool draw_crosshair = true;

    sf::Text imag_xpos_text;
    sf::Text imag_ypos_text;
    sf::Text zoom_text;
    sf::Text iterations_text;
    sf::Text fps_text;

    imag_xpos_text.setFont(font);
    imag_ypos_text.setFont(font);
    zoom_text.setFont(font);
    iterations_text.setFont(font);
    fps_text.setFont(font);

    imag_xpos_text.setCharacterSize(font_size);
    imag_ypos_text.setCharacterSize(font_size);
    zoom_text.setCharacterSize(font_size);
    iterations_text.setCharacterSize(font_size);
    fps_text.setCharacterSize(font_size);

    imag_xpos_text.setFillColor(sf::Color::White);
    imag_ypos_text.setFillColor(sf::Color::White);
    zoom_text.setFillColor(sf::Color::White);
    iterations_text.setFillColor(sf::Color::White);
    fps_text.setFillColor(sf::Color::White);

    imag_xpos_text.setOutlineColor(sf::Color::Black);
    imag_ypos_text.setOutlineColor(sf::Color::Black);
    zoom_text.setOutlineColor(sf::Color::Black);
    iterations_text.setOutlineColor(sf::Color::Black);
    fps_text.setOutlineColor(sf::Color::Black);

    imag_xpos_text.setOutlineThickness(font_outline_thiccness);
    imag_ypos_text.setOutlineThickness(font_outline_thiccness);
    zoom_text.setOutlineThickness(font_outline_thiccness);
    iterations_text.setOutlineThickness(font_outline_thiccness);
    fps_text.setOutlineThickness(font_outline_thiccness);

    imag_xpos_text.setPosition(10, 20);
    imag_ypos_text.setPosition(10, 40);
    zoom_text.setPosition(10, 60);
    iterations_text.setPosition(10, 80);
    fps_text.setPosition(10, 100);

    while (window.isOpen())
    {
        if (auto_iterations)
        {
            if (fps >= 100)
                max_iterations += 1;
            else
                max_iterations -= 1;
        }

        imag_xpos_text.setString("Complex X: " + std::to_string(compX));
        imag_ypos_text.setString("Complex Y: " + std::to_string(compY));
        zoom_text.setString("Detail: " + std::to_string(zoom));
        iterations_text.setString("Iterations: " + std::to_string(max_iterations));
        fps_text.setString("Fps: " + std::to_string(int(fps)));

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compY -= POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
                else
                {
                    compY -= POSITION_INCREMENT * zoom * zoom  + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compY += POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
                else
                {
                    compY += POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compX -= POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
                else
                {
                    compX -= POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compX += POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
                else
                {
                    compX += POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT);
                }
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
                crosshair.setFillColor(sf::Color::Cyan);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
            {
                shader.setUniform("color", 1);
                crosshair.setFillColor(sf::Color::White);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
            {
                shader.setUniform("color", 2);
                crosshair.setFillColor(sf::Color::White);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
            {
                shader.setUniform("color", 3);
                crosshair.setFillColor(sf::Color::White);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
            {
                shader.setUniform("color", 4);
                crosshair.setFillColor(sf::Color::White);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
            {
                shader.setUniform("color", 5);
                crosshair.setFillColor(sf::Color::White);
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

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
            {

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    draw_crosshair = false;
                }
                else
                {
                    draw_crosshair = true;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
            {
                shader.setUniform("color", 6);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            {

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    auto_iterations = false;
                    max_iterations = DEFAULT_MAX_ITERATIONS;
                }
                else
                {
                    auto_iterations = true;
                }
            }
        }

        shader.setUniform("x_translation", compX);
        shader.setUniform("y_translation", compY);
        shader.setUniform("zoom", ((float)(zoom * zoom)) * 10000);
        shader.setUniform("iterations", max_iterations);

        window.clear();

        window.draw(img, &shader);

        window.draw(imag_xpos_text);
        window.draw(imag_ypos_text);
        window.draw(zoom_text);
        window.draw(iterations_text);
        window.draw(fps_text);

        if (draw_crosshair)
            window.draw(crosshair);

        window.display();

        currentTime = clock.getElapsedTime();
        fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
        previousTime = currentTime;
    }

    return 0;
}