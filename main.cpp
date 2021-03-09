#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>

#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

float map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

bool set_waypoint(double x_pos, double y_pos, double zoom, int key, sf::Sound &sound)
{
    std::ofstream file("waypoints/" + std::to_string(key) + ".wp");
    if (file.is_open())
    {
        file << std::setprecision(15);
        file << x_pos << std::endl;
        file << y_pos << std::endl;
        file << zoom;
        file.close();

        sound.play();
        return true;
    }
    else
    {
        return false;
    }
    // ? Life's goooood. Man, this is my absolute favorite project ever!
}

bool load_waypoint(int key, double &x_trans, double &y_trans, double &zoom)
{
    std::string line;
    std::ifstream file("waypoints/" + std::to_string(key) + ".wp");

    int counter = 0;

    if (file.is_open())
    {
        file >> std::setprecision(15);
        while (getline(file, line))
        {
            if (counter == 0)
                x_trans = std::stod(line);
            else if (counter == 1)
                y_trans = std::stod(line);
            else if (counter == 2)
                zoom = std::stod(line);
            counter++;
        }
        file.close();
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

    const int WIDTH = 550;
    const int HEIGHT = 550;

    const double MAX_ZOOM = 5.2146e-07;
    const double MIN_ZOOM = 5.0;
    const double DEFAULT_ZOOM = 2.5;
    const double DEFAULT_X_POS = 0;
    const double DEFAULT_Y_POS = 0;
    const int DEFAULT_MAX_ITERATIONS = 455;
    const double MIN_POS_INCREMENT = 0.00003;

    const double DEEP_LAYER = 0.00000015;
    const double SURFACE_LAYER = 1.0;

    const double POSITION_INCREMENT = 0.0055;
    const double POSITION_INCREMENT_LARGE = 0.1;
    //const float ZOOM_INCREMENT = 0.0001;
    //const float ZOOM_INCREMENT_LARGE = 0.005;
    const float ZOOM_INCREMENT = 0.002;
    const float ZOOM_INCREMENT_LARGE = 0.02;

    int max_iterations = DEFAULT_MAX_ITERATIONS;

    double zoom = DEFAULT_ZOOM;
    double compX = DEFAULT_X_POS;
    double compY = DEFAULT_Y_POS;

    std::ostringstream compX_sstream;
    std::ostringstream compY_sstream;
    std::ostringstream zoom_sstream;
    compX_sstream << std::fixed;
    compY_sstream << std::fixed;
    zoom_sstream << std::fixed;
    compX_sstream << std::setprecision(15);
    compY_sstream << std::setprecision(15);
    zoom_sstream << std::setprecision(15);
    

    bool auto_iterations = false;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fractal viewer");

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

    /* Audio */
    sf::SoundBuffer deep_layer_enter_sound_buffer;
    sf::Sound deep_layer_enter_sound;
    if (!deep_layer_enter_sound_buffer.loadFromFile("sound/deep_layer_enter.wav")) std::cout << "Failed loading sound\n"; 
    deep_layer_enter_sound.setBuffer(deep_layer_enter_sound_buffer);

    sf::SoundBuffer set_waypoint_sound_buffer;
    sf::Sound set_waypoint_sound;
    if (!set_waypoint_sound_buffer.loadFromFile("sound/set_waypoint.wav")) std::cout << "Failed loading sound\n"; 
    set_waypoint_sound.setBuffer(set_waypoint_sound_buffer);

    bool is_deep_layer = false;
    bool is_surface_layer = false;
    bool play_deep_layer_enter = true;


    while (window.isOpen())
    {
        if (auto_iterations)
        {
            if (fps >= 100)
                max_iterations += 1;
            else
                max_iterations -= 1;
        }

        if (zoom * zoom <= DEEP_LAYER) is_deep_layer = true;
        else is_deep_layer = false;
        if (zoom * zoom >= SURFACE_LAYER) is_surface_layer = true;
        else is_surface_layer = false;

        if (is_surface_layer) play_deep_layer_enter = true;
        if ((play_deep_layer_enter) && (is_deep_layer)) {deep_layer_enter_sound.play();play_deep_layer_enter = false;}


        compX_sstream.str("");
        compY_sstream.str("");
        zoom_sstream.str("");
        compX_sstream.clear();
        compY_sstream.clear();
        zoom_sstream.clear();
       
        compX_sstream << compX;
        compY_sstream << compY;
        zoom_sstream << (zoom * zoom);

        imag_xpos_text.setString("Complex X: " + compX_sstream.str());
        imag_ypos_text.setString("Complex Y: " + compY_sstream.str());
        zoom_text.setString("Detail: " + zoom_sstream.str());
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
                    compY -= POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);
                }
                else
                {
                    compY -= POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compY += POSITION_INCREMENT_LARGE * zoom * zoom  + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);
                }
                else
                {
                    compY += POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compX -= POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);
                }
                else
                {
                    compX -= POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    compX += POSITION_INCREMENT_LARGE * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);;
                }
                else
                {
                    compX += POSITION_INCREMENT * zoom * zoom + (MIN_POS_INCREMENT * MIN_POS_INCREMENT * MIN_POS_INCREMENT/2);;
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
                    set_waypoint(compX, compY, zoom, 0, set_waypoint_sound);
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
                    set_waypoint(compX, compY, zoom, 1, set_waypoint_sound);
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
                    set_waypoint(compX, compY, zoom, 2, set_waypoint_sound);
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

        shader.setUniform("x_translation", ((float)(compX * 100000)));
        shader.setUniform("y_translation", ((float)(compY * 100000)));
        shader.setUniform("zoom", ((float)(zoom)) * 1000000);
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