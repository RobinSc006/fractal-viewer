#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <math.h>
#include <iostream>
#include <string>

float map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

int main()
{
    sf::Font font;
    if (!font.loadFromFile("Roboto-Thin.ttf"))
        std::cout << "unable to load font\n";

    const int font_size = 13;
    const int font_outline_thiccness = 1;

    const int WIDTH = 500;
    const int HEIGHT = 500;

    float zoom = 1.0;
    int iterations = 35;
    const float zoom_increment = 0.0005;
    const float zoom_big_increment = 0.005;
    float imag_x_pos = 0.0;
    float imag_y_pos = 0.0;
    const float pos_increment = 0.05;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fractal generator");

    sf::Shader shader;
    if (!shader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment))
        std::cout << "failed to load shader\n";

    shader.setUniform("window_width", WIDTH);
    shader.setUniform("window_height", HEIGHT);
    shader.setUniform("iterations", 100);

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
        imag_xpos_text.setString("Complex X: " + std::to_string(imag_x_pos));
        imag_ypos_text.setString("Complex Y: " + std::to_string(imag_y_pos));
        zoom_text.setString("Detail: " + std::to_string(zoom));
        iterations_text.setString("Iterations: " + std::to_string(iterations));

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                imag_y_pos -= pos_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                imag_y_pos += pos_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                imag_x_pos += pos_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                imag_x_pos -= pos_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            {
                iterations -= 5;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                iterations += 5;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                zoom += zoom_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                zoom -= zoom_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                zoom += zoom_big_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                zoom -= zoom_big_increment;
            }
        }

        shader.setUniform("x_translation", imag_x_pos);
        shader.setUniform("y_translation", imag_y_pos);
        shader.setUniform("zoom", zoom);
        shader.setUniform("iterations", iterations);

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