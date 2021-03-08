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

sf::Image gen_mandelbrot(int actX, int actY, float zoom, int iterations, float x_translation, float y_translation)
{
    float diversion_threshold = 4.0;

    sf::Image img_plane;
    img_plane.create(actX, actY);

    for (int x = 0; x < actX; x++)
    {
        for (int y = 0; y < actY; y++)
        {
            float z_real = map((float)x, 0.0, (float)actX, (-2.0 - x_translation) * zoom, (2.0 - x_translation) * zoom);
            float z_imaginary = map((float)y, 0.0, (float)actY, (-2.0 + y_translation) * zoom, (2.0 + y_translation) * zoom);

            float z_real_original = z_real;
            float z_imaginary_original = z_imaginary;

            float brightness = 0.0;

            for (int n = 0; n <= iterations; n++)
            {
                float z_real_temp = z_real * z_real - z_imaginary * z_imaginary;
                float z_imaginary_temp = 2.0 * z_real * z_imaginary;

                z_real = z_real_temp + z_real_original;
                z_imaginary = z_imaginary_temp + z_imaginary_original;

                brightness = map((float)n, 0.0, (float)iterations, 0.0, 1.0);
                brightness = map(sqrt(brightness), 0.0, 1.0, 0.0, 200.0);

                if ((z_real * z_real + z_imaginary * z_imaginary) > diversion_threshold)
                {
                    break;
                }
            }

            img_plane.setPixel(x, y, sf::Color(brightness, (int)(brightness * 0.67), (int)(brightness * 0.80), 255));
        }
    }

    return img_plane;
    /* Brooo that took like 7 minutes */
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
    int iterations = 50;
    const float zoom_increment = 0.0005;
    const float zoom_big_increment = 0.005;
    float imag_x_pos = 0.0;
    float imag_y_pos = 0.0;
    const float pos_increment = 0.05;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fractal generator");

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Image img = gen_mandelbrot(WIDTH, HEIGHT, zoom, iterations, imag_x_pos, imag_y_pos);

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
        imag_xpos_text.setString("Imaginary X: " + std::to_string(imag_x_pos));
        imag_ypos_text.setString("Imaginary Y: " + std::to_string(imag_y_pos));
        zoom_text.setString("Detail: " + std::to_string(zoom));
        iterations_text.setString("Iterations: " + std::to_string(iterations));

        img = gen_mandelbrot(WIDTH, HEIGHT, zoom, iterations, imag_x_pos, imag_y_pos);
        texture.loadFromImage(img);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                imag_y_pos -= pos_increment;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
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

        window.clear();

        window.draw(sprite);

        window.draw(imag_xpos_text);
        window.draw(imag_ypos_text);
        window.draw(zoom_text);
        window.draw(iterations_text);

        window.display();
    }

    return 0;
}
