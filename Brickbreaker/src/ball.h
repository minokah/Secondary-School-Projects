#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include "paddle.h"

using namespace sf;
using namespace std;

#ifndef BALL_HEADER
#define BALL_HEADER

struct Ball : public CircleShape {
    Ball(b2World &world, float x, float y, float r, b2BodyType type = b2_dynamicBody) {
        b2BodyDef bodyDef;
        bodyDef.position.Set(x/32, y/32);
        bodyDef.type = type;
        bodyDef.linearDamping = 0;
        b2CircleShape b2shape;

        b2shape.m_radius = r/32;

        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.0;
        fixtureDef.friction = 0;
        fixtureDef.restitution = 1.0;
        fixtureDef.shape = &b2shape;

        body = world.CreateBody(&bodyDef);
        body->CreateFixture(&fixtureDef);

        body->SetLinearVelocity(b2Vec2(speed*cos(angle), speed*sin(angle)));

        this->setRadius(r);
        this->setPosition(x, y);
        this->setOrigin(r, r);
    }

    void updatePos() {
        this->setPosition(body->GetPosition().x*32, body->GetPosition().y*32);
        this->setRotation(body->GetAngle()*180/3.14);

        b2Vec2 velocity = body->GetLinearVelocity();
        body->SetLinearVelocity(speed/velocity.Length()*velocity);
    }

    b2Body* body;
    float speed = 300/32;
    float angle = 45/(180/3.14);
    bool active = false;
};

#endif // BALL_HEADER
