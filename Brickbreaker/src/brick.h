#include <SFML/Graphics.hpp>
#include <iostream>
#include "paddle.h"

using namespace sf;
using namespace std;

#ifndef BRICK_HEADER
#define BRICK_HEADER

struct Brick : public Paddle {
    Brick(b2World &world, float x, float y, float width, float height, Color colour = Color::Red) : Paddle(world, x, y, width, height) {
        this->setFillColor(Color::Red);
        this->setPosition(body->GetPosition().x*32, body->GetPosition().y*32);
        this->setRotation(body->GetAngle()*180/3.14);
        this->setFillColor(colour);
        blockColour = colour;
    }

    void checkDie(b2World &world, Ball &ball, int& score, float& streak) {
        for (b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next) {
            if (edge->other == ball.body && !die) {
                ball.setFillColor(this->getFillColor());
                float columns = this->getSize().x/10;
                float rows = this->getSize().y/10;
                score++;
                streak++;

                for (int i = 0; i != rows; i++) {
                    for (int x = 0; x != columns; x++) {
                        deathBlocks.push_back(Paddle(world, this->getPosition().x - 50 + x * 10, this->getPosition().y - 15 + i * 10, 10, 10));
                        float angle = (rand() % 361) * 180/3.14;
                        deathBlocks.back().body->SetLinearVelocity(b2Vec2(5 * cos(angle), 5 * sin(angle)));
                        deathBlocks.back().setFillColor(blockColour);
                        this->setFillColor(Color::Transparent);
                        deathBlocks.back().body->GetFixtureList()->SetSensor(true);
                        die = true;
                    }
                }
            }
        }

        for (int i = 0; i < deathBlocks.size(); i++) {
            deathBlocks[i].updatePos();

            if (die) {
                if (gravityTimer.getElapsedTime().asSeconds() > 0.1) {
                    this->body->GetFixtureList()->SetSensor(true);
                    deathBlocks[i].body->SetLinearVelocity(b2Vec2(deathBlocks[i].body->GetLinearVelocity().x, gravityTimer.getElapsedTime().asSeconds() * 9.8));
                    if (deathBlocks[i].getFillColor().a > 3) {
                        deathBlocks[i].setFillColor(Color(blockColour.r, blockColour.g, blockColour.b, deathBlocks[i].getFillColor().a - 3));
                    }
                    else {
                        physics::deleteBlock(world, deathBlocks[i].body);
                        deathBlocks.erase(deathBlocks.begin() + i);
                        dead = true;
                    }
                }
            }
        }

        if (!die) {
            gravityTimer.restart();
        }
    }

    bool die = false;
    bool dead = false;
    vector<Paddle> deathBlocks;
    Clock gravityTimer;
    Color blockColour;
};

#endif // BRICK_HEADER
