#include <SFML/Graphics.hpp>
#include <iostream>

#ifndef PADDLE_HEADER
#define PADDLE_HEADER

using namespace sf;
using namespace std;

struct Paddle : public RectangleShape {
    Paddle(b2World &world, float x, float y, float width, float height) {
        b2BodyDef bodyDef;
        bodyDef.position.Set((x + width/2.0)/32, (y + height/2.0)/32);
        bodyDef.type = b2_kinematicBody;
        bodyDef.linearDamping = 0;
        b2PolygonShape b2shape;
        b2shape.SetAsBox(width/32/2.0, height/32/2.0);
        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.0;
        fixtureDef.friction = 0;
        fixtureDef.restitution = 1;
        fixtureDef.shape = &b2shape;

        trans = b2Vec2((x + width/2.0)/32, (y + height/2.0)/32);

        body = world.CreateBody(&bodyDef);
        body->CreateFixture(&fixtureDef);

        this->setSize(Vector2f(width, height));
        this->setPosition(x, y);
        this->setOrigin(width/2, height/2);
    }

    void updatePos() {
        this->setPosition(body->GetPosition().x*32, body->GetPosition().y*32);
        this->setRotation(body->GetAngle()*180/3.14);
    }

    void keyMove() {
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            body->SetLinearVelocity(b2Vec2(speed, 0));
        }
        else if (Keyboard::isKeyPressed(Keyboard::A)) {
            body->SetLinearVelocity(b2Vec2(-speed, 0));
        }
        else body->SetLinearVelocity(b2Vec2(0, 0));
    }

    void Resize(b2World& world, float x, float y) {
        physics::deleteBlock(world, body);
        b2Vec2 position = body->GetPosition();
        b2BodyDef bodyDef;
        bodyDef.position.Set(position.x, position.y);
        bodyDef.type = b2_kinematicBody;
        bodyDef.linearDamping = 0;
        b2PolygonShape b2shape;
        b2shape.SetAsBox(x/32/2.0, y/32/2.0);
        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.0;
        fixtureDef.friction = 0;
        fixtureDef.restitution = 1;
        fixtureDef.shape = &b2shape;

        body = world.CreateBody(&bodyDef);
        body->CreateFixture(&fixtureDef);
        this->setSize(Vector2f(x, y));
    }

    b2Body* body;
    float speed = 10;
    b2Vec2 trans;
};

#endif // PADDLE_HEADER
