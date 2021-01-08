#ifndef BREAKER_UI
#define BREAKER_UI

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

struct RoundedFrame : public RectangleShape {
    RoundedFrame(float x, float y, Color colour) {
        this->setSize(Vector2f(x - y/2, y));
        this->setFillColor(colour);
        LeftCircle.setRadius(y/2);
        LeftCircle.setFillColor(colour);
        RightCircle.setRadius(y/2);
        RightCircle.setFillColor(colour);
        LeftCircle.setPointCount(360);
        RightCircle.setPointCount(360);

        Reposition(0, 0);
    }

    void Reposition(float x, float y) {
        float r = LeftCircle.getRadius();
        this->setPosition(x + r, y);
        LeftCircle.setPosition(x, y);
        RightCircle.setPosition(x + this->getSize().x, y);
    }

    void Resize(float x, float y) {
        this->setSize(Vector2f(x - y/2, y));
        Reposition(LeftCircle.getPosition().x, LeftCircle.getPosition().y);
    }

    void Draw(RenderWindow& window) {
        window.draw(LeftCircle);
        window.draw(RightCircle);
        window.draw(*this);
    }

    void Recolor(Color colour) {
        this->setFillColor(colour);
        LeftCircle.setFillColor(colour);
        RightCircle.setFillColor(colour);

    }

    CircleShape LeftCircle, RightCircle;
};

#endif // BREAKER_UI
