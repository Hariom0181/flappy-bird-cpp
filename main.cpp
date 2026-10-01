#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

// ========================================
// PIPE STRUCTURE
// ========================================

struct PipePair
{
    sf::RectangleShape top;
    sf::RectangleShape bottom;
    bool scored = false;
};

// ========================================
// MAIN
// ========================================

int main()
{
    // ========================================
    // WINDOW
    // ========================================

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Flappy Bird"
    );

    window.setFramerateLimit(60);

    // ========================================
    // BIRD
    // ========================================

    sf::CircleShape bird(25);

    bird.setFillColor(sf::Color::Yellow);

    bird.setPosition({200, 250});

    // Bird physics
    float gravity = 1000.0f;
    float velocity = 0.0f;
    float flapStrength = -400.0f;

    // ========================================
    // FONT
    // ========================================

    sf::Font font;

    if (!font.openFromFile("assets/font.ttf"))
    {
        std::cout << "ERROR: Could not load font!"
                  << std::endl;

        return 1;
    }

    // ========================================
    // SCORE TEXT
    // ========================================

    sf::Text scoreText(font);

    scoreText.setCharacterSize(50);

    scoreText.setFillColor(sf::Color::White);

    scoreText.setPosition({20, 20});

    // ========================================
    // SCORE
    // ========================================

    int score = 0;

    // ========================================
    // PIPES
    // ========================================

    std::vector<PipePair> pipes;

    float pipeSpeed = 250.0f;

    float pipeWidth = 80.0f;

    float gapSize = 180.0f;

    float pipeSpawnTimer = 0.0f;

    float pipeSpawnInterval = 2.0f;

    // Random numbers
    std::srand(
        static_cast<unsigned>(std::time(nullptr))
    );

    // ========================================
    // GROUND
    // ========================================

    sf::RectangleShape ground({800, 50});

    ground.setPosition({0, 550});

    ground.setFillColor(
        sf::Color(139, 69, 19)
    );

    // ========================================
    // CLOCK
    // ========================================

    sf::Clock clock;

    // ========================================
    // GAME STATE
    // ========================================

    bool gameOver = false;

    // ========================================
    // GAME LOOP
    // ========================================

    while (window.isOpen())
    {
        // ====================================
        // DELTA TIME
        // ====================================

        float deltaTime =
            clock.restart().asSeconds();

        // ====================================
        // EVENTS
        // ====================================

        while (const std::optional event =
                   window.pollEvent())
        {
            // Close window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Keyboard
            if (const auto* keyPressed =
                    event->getIf<
                        sf::Event::KeyPressed>())
            {
                // ====================================
                // SPACE = FLAP
                // ====================================

                if (keyPressed->code ==
                    sf::Keyboard::Key::Space)
                {
                    if (!gameOver)
                    {
                        velocity = flapStrength;
                    }
                }

                // ====================================
                // R = RESTART
                // ====================================

                if (keyPressed->code ==
                    sf::Keyboard::Key::R)
                {
                    bird.setPosition({
                        200,
                        250
                    });

                    velocity = 0.0f;

                    pipes.clear();

                    score = 0;

                    pipeSpawnTimer = 0.0f;

                    gameOver = false;
                }
            }
        }

        // ========================================
        // GAME UPDATE
        // ========================================

        if (!gameOver)
        {
            // ====================================
            // BIRD GRAVITY
            // ====================================

            velocity +=
                gravity * deltaTime;

            bird.move({
                0,
                velocity * deltaTime
            });

            // ====================================
            // PIPE SPAWNING
            // ====================================

            pipeSpawnTimer += deltaTime;

            if (pipeSpawnTimer >=
                pipeSpawnInterval)
            {
                pipeSpawnTimer = 0.0f;

                // Random gap center
                float gapY =
                    100.0f +
                    static_cast<float>(
                        std::rand() % 300
                    );

                PipePair newPipe;

                // ====================================
                // TOP PIPE
                // ====================================

                newPipe.top.setSize({
                    pipeWidth,
                    gapY - gapSize / 2
                });

                newPipe.top.setPosition({
                    800,
                    0
                });

                newPipe.top.setFillColor(
                    sf::Color::Green
                );

                // ====================================
                // BOTTOM PIPE
                // ====================================

                newPipe.bottom.setSize({
                    pipeWidth,
                    600 -
                    (gapY + gapSize / 2)
                });

                newPipe.bottom.setPosition({
                    800,
                    gapY + gapSize / 2
                });

                newPipe.bottom.setFillColor(
                    sf::Color::Green
                );

                // New pipe hasn't been scored
                newPipe.scored = false;

                pipes.push_back(newPipe);
            }

            // ====================================
            // MOVE PIPES
            // ====================================

            for (auto& pipe : pipes)
            {
                pipe.top.move({
                    -pipeSpeed * deltaTime,
                    0
                });

                pipe.bottom.move({
                    -pipeSpeed * deltaTime,
                    0
                });
            }

            // ====================================
            // SCORE
            // ====================================

            float birdX =
                bird.getPosition().x;

            for (auto& pipe : pipes)
            {
                float pipeRight =
                    pipe.top.getPosition().x +
                    pipeWidth;

                // Pipe has passed the bird
                if (!pipe.scored &&
                    pipeRight < birdX)
                {
                    score++;

                    pipe.scored = true;

                    std::cout
                        << "Score: "
                        << score
                        << std::endl;
                }
            }

            // ====================================
            // COLLISION WITH PIPES
            // ====================================

            for (auto& pipe : pipes)
            {
                if (bird.getGlobalBounds()
                        .findIntersection(
                            pipe.top.getGlobalBounds()))
                {
                    gameOver = true;
                }

                if (bird.getGlobalBounds()
                        .findIntersection(
                            pipe.bottom.getGlobalBounds()))
                {
                    gameOver = true;
                }
            }

            // ====================================
            // GROUND COLLISION
            // ====================================

            if (bird.getPosition().y + 50 >= 550)
            {
                gameOver = true;
            }

            // ====================================
            // CEILING COLLISION
            // ====================================

            if (bird.getPosition().y <= 0)
            {
                gameOver = true;
            }

            // ====================================
            // REMOVE OLD PIPES
            // ====================================

            pipes.erase(
                std::remove_if(
                    pipes.begin(),
                    pipes.end(),

                    [](const PipePair& pipe)
                    {
                        return
                            pipe.top.getPosition().x +
                            80 < 0;
                    }
                ),

                pipes.end()
            );
        }

        // ========================================
        // UPDATE SCORE TEXT
        // ========================================

        scoreText.setString(
            "Score: " +
            std::to_string(score)
        );

        // ========================================
        // DRAW
        // ========================================

        window.clear(
            sf::Color::Cyan
        );

        // Draw bird
        window.draw(bird);

        // Draw pipes
        for (auto& pipe : pipes)
        {
            window.draw(pipe.top);

            window.draw(pipe.bottom);
        }

        // Draw ground
        window.draw(ground);

        // Draw score
        window.draw(scoreText);

        // Display everything
        window.display();
    }

    return 0;
}