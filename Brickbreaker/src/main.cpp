#include "physics.h"
#include "ball.h"
#include "paddle.h"
#include "brick.h"
#include <vector>
#include <string>
#include "ui.hpp"
#include "fading.hpp"

using namespace std;
using namespace sf;

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(1024, 750),"Brickbreaker");
    window.setFramerateLimit(60);
    b2World world(b2Vec2(0, 0));
    //Block bottomWall = physics::createBox(world, 0, 750, 1024, 30, b2_staticBody); bottomWall->GetFixtureList()->SetRestitution(1);
    Block leftWall = physics::createBox(world, -30, 0, 30, 750, b2_staticBody); leftWall->GetFixtureList()->SetRestitution(1);
    Block rightWall = physics::createBox(world, 1024, 0, 30, 750, b2_staticBody); rightWall->GetFixtureList()->SetRestitution(1);
    Block topWall = physics::createBox(world, 0, 0, 1024, 0, b2_staticBody); topWall->GetFixtureList()->SetRestitution(1);

    Texture backTexture;
    backTexture.loadFromFile("background.png");
    Sprite background(backTexture);
    rgbProfile profile;

    Paddle paddle(world, 450, 630, 150, 30); // 450, 150

    vector<Color> colours;
    colours.push_back(Color(0, 168, 255));
    colours.push_back(Color(156, 136, 255));
    colours.push_back(Color(251, 197, 49));
    colours.push_back(Color(76, 209, 55));
    colours.push_back(Color(72, 126, 176));
    colours.push_back(Color(232, 65, 24));
    colours.push_back(Color(245, 246, 250));
    colours.push_back(Color(127, 143, 166));
    colours.push_back(Color(39, 60, 117));
    colours.push_back(Color(53, 59, 72));

    vector<Brick> bricks;
    vector<Ball> balls;

    Font barlowBold;
    barlowBold.loadFromFile("Barlow-Bold.ttf");

    RoundedFrame pointFrame(160, 30, Color(255, 107, 107));
    pointFrame.Reposition(-10, 720);

    int score = 0;
    Text pointText("SCORE: 0", barlowBold, 20);
    pointText.setPosition(15, 723);
    pointText.setOutlineThickness(1);
    pointText.setOutlineColor(Color(0, 0, 0, 100));

    RoundedFrame streakFrame(170, 30, Color(72, 219, 251));
    streakFrame.Reposition(125, 720);

    float streak = 0;
    Text streakText("STREAK: 0", barlowBold, 20);
    streakText.setPosition(140, 723);
    streakText.setOutlineThickness(1);
    streakText.setOutlineColor(Color(0, 0, 0, 100));

    RoundedFrame breakstrFrame(755, 30, Color(50, 50, 50));
    breakstrFrame.Reposition(265, 720);

    RoundedFrame breakstrAmtFrame(185, 30, Color(254, 202, 87));
    breakstrAmtFrame.Reposition(265, 720);

    Text breakstrText("BREAKERSTREAKS", barlowBold, 20);
    breakstrText.setPosition(280, 723);
    breakstrText.setOutlineThickness(1);
    breakstrText.setOutlineColor(Color(0, 0, 0, 100));

    RoundedFrame speedFrame(70, 30, Color(100, 100, 100));
    speedFrame.Reposition(600, 720);

    Text speedText("SPEED", barlowBold, 20);
    speedText.setPosition(615, 723);
    speedText.setOutlineThickness(1);
    speedText.setOutlineColor(Color(0, 0, 0, 100));

    bool secondBall = false;
    bool secondBallDestroyed = false;
    bool resized = false;
    RoundedFrame splitFrame(70, 30, Color(100, 100, 100));
    splitFrame.Reposition(780, 720);

    Text splitText("SPLIT", barlowBold, 20);
    splitText.setPosition(795, 723);
    splitText.setOutlineThickness(1);
    splitText.setOutlineColor(Color(0, 0, 0, 100));

    RoundedFrame sizeFrame(70, 30, Color(100, 100, 100));
    sizeFrame.Reposition(960, 720);

    Text sizeText("SIZE", barlowBold, 20);
    sizeText.setPosition(975, 723);
    sizeText.setOutlineThickness(1);
    sizeText.setOutlineColor(Color(0, 0, 0, 100));

    Clock brickGenTimer;

    RectangleShape countdownFrame(Vector2f(1024, 50));
    countdownFrame.setPosition(0, 450);
    countdownFrame.setFillColor(colours[rand() % 10]);

    Text countdownText("3", barlowBold, 50);
    countdownText.setPosition(512, 457);
    countdownText.setOutlineThickness(1);
    countdownText.setOutlineColor(Color(0, 0, 0, 100));

    RoundedFrame secondBallNotif(300, 30, Color(95, 39, 205));
    secondBallNotif.Reposition(724, 680);

    Text secondBallText("Get ready to play!", barlowBold, 20);
    secondBallText.setPosition(739, 683);
    secondBallText.setOutlineThickness(1);
    secondBallText.setOutlineColor(Color(0, 0, 0, 100));

    int countdownAmt = 3;
    Clock countdownTimer, secondBallTimer;
    bool countingDown = true;
    bool secondBallSpawning = false;

    bool firstRun = true;

    while (window.isOpen()&& !Keyboard::isKeyPressed(Keyboard::Escape)) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }

        for (int i = 0; i != balls.size(); i++) {
            balls[i].updatePos();
        }

        if (countingDown && countdownAmt > -1) {
            if (countdownTimer.getElapsedTime().asSeconds() >= 1) {
                if (countdownAmt > 0) countdownAmt--;
                else {
                    countingDown = false;
                    balls.push_back(Ball(world, 512, 560, 20));
                }
                countdownText.setString(to_string(countdownAmt));
                countdownText.setCharacterSize(50);
                countdownTimer.restart();
            }

            if (countdownText.getCharacterSize() > 30) countdownText.setCharacterSize(countdownText.getCharacterSize() - 3);
        }

        paddle.updatePos();

        if (bricks.size() == 0) {
            if (firstRun ||  balls.size() > 0 && balls[0].getPosition().y > 500) {
                bool alternating = false;
                for (int x = 0; x != 8; x++) {
                    if (!alternating) {
                        for (int i = 0 ; i != 10; i++) {
                            bricks.push_back(Brick(world, -35 + i * 110, 10 + x * 50, 100, 40, colours[rand() % 10]));
                        }
                    }
                    else {
                        for (int i = 0 ; i != 9; i++) {
                            bricks.push_back(Brick(world, 25 + i * 110, 10 + x * 50, 100, 40, colours[rand() % 10]));
                        }
                    }
                    alternating = !alternating;
                }
            }
            firstRun = false;
        }

        for (int i = 0; i < bricks.size(); i++) {
            for (int x = 0; x != balls.size(); x++) {
                bricks[i].checkDie(world, balls[x], score, streak);
            }

            if (bricks[i].dead) {
                physics::deleteBlock(world, bricks[i].body);
                bricks.erase(bricks.begin() + i);
            }
        }

        if (!countingDown) {
            if (streak >= 30) {
                speedFrame.Recolor(Color(50, 255, 126));
                for (auto i : balls) {
                    i.speed = 500/32;
                }
                paddle.speed = 15;
            }
            else {
                speedFrame.Recolor(Color(100, 100, 100));
                paddle.speed = 10;
                balls[1].speed = 300/32;
            }

            if (streak >= 60) {
                splitFrame.Recolor(Color(50, 255, 126));
                if (!secondBall && !secondBallDestroyed) {
                    if (secondBallTimer.getElapsedTime().asSeconds() <= 3) {
                        secondBallSpawning = true;
                        secondBallText.setString("Second ball spawning in " + to_string(3 - (int)secondBallTimer.getElapsedTime().asSeconds()) + "...");
                    }
                    else if (!secondBall) {
                        balls.push_back(Ball(world, 512, 480, 20));
                        balls[0].setOutlineThickness(3);
                        secondBall = true;
                        secondBallSpawning = false;
                    }
                }
            }
            else {
                splitFrame.Recolor(Color(100, 100, 100));
                if (balls.size() > 1 && !secondBallDestroyed) {
                    physics::deleteBlock(world, balls[1].body);
                    balls.erase(balls.begin() + 1);
                }
                balls[0].setOutlineThickness(0);
                balls[0].setFillColor(Color::White);
                secondBall = false;
                secondBallDestroyed = false;
                secondBallTimer.restart();
            }

            if (balls[0].getPosition().y > 750) {
                paddle.body->SetTransform(paddle.trans, 0);
                paddle.body->SetLinearVelocity(b2Vec2(0, 0));
                physics::deleteBlock(world, balls[0].body);
                if (secondBall) physics::deleteBlock(world, balls[1].body);
                balls.clear();
                secondBallDestroyed = false;
                secondBall = false;
                countingDown = true;
                countdownAmt = 3;
                countdownText.setString("3");
                countdownFrame.setFillColor(colours[rand() % 10]);
                countdownTimer.restart();
                secondBallText.setString("Breakerstreak(s) lost!");
                streak = 0;
            }

            if (secondBall && !secondBallDestroyed) {
                if (balls[1].getPosition().y > 750) {
                    secondBallDestroyed = true;
                    physics::deleteBlock(world, balls[1].body);
                    balls.erase(balls.begin() + 1);
                    balls[0].setOutlineThickness(0);
                    secondBallText.setString("Second ball lost, careful now!");
                }
                else secondBallTimer.restart();
            }

            if (streak >= 90) {
                sizeFrame.Recolor(Color(50, 255, 126));secondBallNotif.Reposition(800, 680);
                if (!resized) {
                    paddle.Resize(world, 200, 30);
                    resized = true;
                }
            }
            else {
                sizeFrame.Recolor(Color(100, 100, 100));
                if (resized) {
                    paddle.Resize(world, 150, 30);
                    resized = false;
                }
            }
        }

        pointText.setString("SCORE: " + to_string(score));
        streakText.setString("STREAK: " + to_string((int)streak));
        float sizething = streak/90;
        breakstrAmtFrame.Resize(185 + (sizething * 570), 30);

        background.setColor(Color(profile.r, profile.g, profile.b));
        profile.Routine();
        if (!countingDown) paddle.keyMove();

        window.clear();
        world.Step((float32)1/60, 8, 8);
        window.draw(background);
        for (auto i : balls) {
            window.draw(i);
        }
        window.draw(paddle);
        pointFrame.Draw(window);
        window.draw(pointText);
        streakFrame.Draw(window);
        window.draw(streakText);
        breakstrFrame.Draw(window);
        breakstrAmtFrame.Draw(window);
        window.draw(breakstrText);

        speedFrame.Draw(window);
        window.draw(speedText);

        splitFrame.Draw(window);
        window.draw(splitText);

        sizeFrame.Draw(window);
        window.draw(sizeText);

        for (int i = 0; i != bricks.size(); i++) {
            window.draw(bricks[i]);
            for (int x = 0; x != bricks[i].deathBlocks.size(); x++) {
                window.draw(bricks[i].deathBlocks[x]);
            }
        }

        if (countingDown) {
            window.draw(countdownFrame);
            window.draw(countdownText);
        }

        if (secondBallSpawning || countingDown || secondBallDestroyed && secondBallTimer.getElapsedTime().asSeconds() < 3) {
            secondBallNotif.Draw(window);
            window.draw(secondBallText);
        }

        window.display();
    }

    return 0;
}
