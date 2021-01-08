// For use in ICS4U 2019-20
// Brandon "minokah" Luu
// Game: DESOCRETE

// EaseMove is unused and will probably be removed along with the QuadEaseOut function

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "engine.h"

using namespace std;
using namespace sf;
using namespace Engine;
using namespace UI;

#ifndef ENGINE_HEADER
#define ENGINE_HEADER

// Game Engine - Self
namespace Engine {
    Font Roboto;
    RenderWindow GameWindow;
    Event GameEvent;
}

// Settings menu options
ResolutionPair::ResolutionPair(int xx, int yy) {
    x = xx;
    y = yy;
}

Settings::Settings() {
    Settings::WindowResolutions.push_back(ResolutionPair(1280, 768));
    Settings::WindowResolutions.push_back(ResolutionPair(1280, 1024));
    Settings::WindowResolutions.push_back(ResolutionPair(1366, 768));
    Settings::WindowResolutions.push_back(ResolutionPair(1440, 900));
    Settings::WindowResolutions.push_back(ResolutionPair(1600, 900));
    Settings::WindowResolutions.push_back(ResolutionPair(1920, 1080));
    Settings::WindowMode.push_back("Windowed");
    Settings::WindowMode.push_back("Borderless");
    Settings::WindowMode.push_back("Fullscreen");
    Settings::ToggleMode.push_back("On");
    Settings::ToggleMode.push_back("Off");
}

/*
    Game Engine - Console
    Allows parsing and output of commands
*/

Console::Console() {
    Roboto.loadFromFile("Roboto.ttf");
    Console::StatText.SetColour(Color::Black);
    Console::StatOwOText.SetColour(Color::Black);
    Console::StatOwOText.SetTransparency(40);
}

void Console::Print(string text, string type, Color colour) {
    string prefix;
    if (type == "time") prefix = "[" + to_string(Console::GameRuntime.getElapsedTime().asSeconds()) + "s]";
    else if (type == "self") prefix = "[!]";
    else if (type == "help") prefix = "[?]";
    else if (type == "output" || type == "none") prefix = "";
    TextLabel line(prefix + " " + text, Roboto, 15);
    line.SetColour(colour);
    line.UpdateResolution(Console::WindowSize.x - line.Text.getGlobalBounds().width - 35, Console::Text.size() * 18 + 135);
    Console::Text.push_back(line);
}

void Console::Draw() {
    if (Console::Toggled) {
        Console::StatUptimeText.SetText(to_string(Console::GameRuntime.getElapsedTime().asSeconds()) + "s");
        Console::StatPanel.Draw();
        Console::StatSecondaryPanel.Draw();
        Console::ClearButton.Draw();
        Console::InputButton.Draw();
        Console::StatText.Draw();
        Console::StatOwOText.Draw();
        Console::StatUptimeText.Draw();
        Console::UptimeImage.Draw();
        Console::ClearImage.Draw();
        Console::InputImage.Draw();
        for (auto i : Console::Text) i.Draw();
    }
}

void Console::UpdateResolution(float x, float y) {
    Console::StatPanel.UpdateResolution(x - 330, 30);
    Console::StatSecondaryPanel.UpdateResolution(x - 330, 55);
    Console::ClearButton.UpdateResolution(x - 320, 90);
    Console::InputButton.UpdateResolution(x - 320, 115);
    Console::StatText.UpdateResolution(x - 310, 33);
    Console::StatOwOText.UpdateResolution(x - 75, 33);
    Console::StatUptimeText.UpdateResolution(x - 310, 58);
    Console::UptimeImage.UpdateResolution(x - 60, 58);
    Console::ClearImage.UpdateResolution(x - 60, 83);
    Console::InputImage.UpdateResolution(x - 60, 108);
    Console::WindowSize = Vector2f(x, y);
}

void Console::Toggle() {
    Console::Toggled = !Console::Toggled;
    Console::ToggleClock.restart();
}

// Allows console functionality
// Must be ran in the game loop using Console.Routine(cursor);
void Console::Routine(RectangleShape &cursor) {
    if (Console::ToggleClock.getElapsedTime().asSeconds() > 0.35 && Keyboard::isKeyPressed(Keyboard::Tilde)) {
        Console::Toggle();
    }

    if (Console::ClearButton.Hover(cursor) && Console::ClearButton.Clicked()) {
        Console::Clear();
    }

    if (Console::InputButton.Hover(cursor) && Console::InputButton.Clicked()) {
        Console::StatText.SetText("ESC/RETURN TO CANCEL");
        Console::InputButton.SetText("");
        GameWindow.setMouseCursorVisible(false);
        Console::Inputting = true;
    }

    while (GameWindow.pollEvent(GameEvent)) {
        if (GameEvent.type == Event::Closed) GameWindow.close();

        if (Console::Inputting) {
            if (GameEvent.type == Event::TextEntered) {
                int unicode = GameEvent.text.unicode;
                string text = Console::InputButton.Text.getString();
                if (unicode == 8) {
                    if (text != "") Console::InputButton.SetText(text.erase(text.size() - 1));
                }
                else if (unicode < 128) Console::InputButton.SetText(text + string(1, unicode));
            }

            if (Keyboard::isKeyPressed(Keyboard::Return)) {
                string RemoveLetter = Console::InputButton.Text.getString();

                if (RemoveLetter == "") Console::Cancel();
                else {
                    for (int i = 0; i != RemoveLetter.length(); i++) {
                        if (RemoveLetter[i] == ' ') {
                            Console::Value = RemoveLetter.substr(i + 1);
                            break;
                        }
                        else Console::Command += RemoveLetter[i];
                    }

                    Console::ProcessCommand = true;
                    Console::Cancel();
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                Console::Cancel();
            }
        }
    }
}

void Console::Cancel() {
    GameWindow.setMouseCursorVisible(true);
    Console::Inputting = false;
    Console::StatText.SetText("ENGINE");
    Console::InputButton.SetText("INPUT COMMAND");
}

void Console::Clear() {
    Console::Text.clear();
}

/*
    Game Engine - UI
    Interface elements to make life a bit easier
*/

// I should've known about derived classes before I made these UI elements :(
// Oh well

Color PrimaryButton = Color(253, 220, 104);
Color TransparentColor = Color(0, 0, 0, 0);

// Easing style Quad (Out)
// EaseMove option is deprecated
float QuadEaseOut(float t,float b , float c, float d) {
	return -c *(t/=d)*(t-2) + b;
}

/*
    Frame
    It's a box

    x, y        - Size of the frame
    colour      - Rainbows are pretty
*/

Frame::Frame(float x, float y, Color colour) {
    Frame::Panel.setSize(Vector2f(x, y));
    Frame::Panel.setFillColor(colour);
}

void Frame::EaseMove(string direction, float initial, float goal, float time) {
    float EaseTime = Frame::EaseClock.getElapsedTime().asSeconds();
    if (EaseTime <= time) {
        float Value = QuadEaseOut(EaseTime, initial, goal, time);
        if (direction == "horizontal") Frame::Panel.setPosition(Value, Frame::Panel.getPosition().y);
        else if (direction == "vertical") Frame::Panel.setPosition(Frame::Panel.getPosition().x, Value);
    }
}

void Frame::UpdateResolution(float x, float y) {
    Frame::Panel.setPosition(x, y);
}

void Frame::Draw() {
    GameWindow.draw(Frame::Panel);
}

/*
    TextLabel
    Words on your screen

    text        - What it'll say
    font        - Font used for the selection text
    fontsize    - No need to explain
*/

TextLabel::TextLabel(string text, Font &font, int fontsize) {
    TextLabel::Text.setString(text);
    TextLabel::Text.setFont(font);
    TextLabel::Text.setCharacterSize(fontsize);
}

void TextLabel::SetText(string text) {
    TextLabel::Text.setString(text);
}

void TextLabel::EaseMove(string direction, float initial, float goal, float time) {
    float EaseTime = TextLabel::EaseClock.getElapsedTime().asSeconds();
    if (EaseTime <= time) {
        float Value = QuadEaseOut(EaseTime, initial, goal, time);
        if (direction == "horizontal") TextLabel::Text.setPosition(Value, TextLabel::Text.getPosition().y);
        else if (direction == "vertical") TextLabel::Text.setPosition(TextLabel::Text.getPosition().x, Value);
    }
}

void TextLabel::UpdateResolution(float x, float y) {
    TextLabel::Text.setPosition(x, y);
}

void TextLabel::SetTransparency(float a) {
    Color colour = TextLabel::Text.getColor();
    TextLabel::Text.setFillColor(Color(colour.r, colour.g, colour.b , a));
}

void TextLabel::SetColour(Color colour) {
    TextLabel::Text.setFillColor(colour);
}

void TextLabel::Fade(string type, float goal, float rate) {
    Color colour = TextLabel::Text.getFillColor();
    if (type == "in") {
        if (colour.a < goal - 1) TextLabel::Text.setFillColor(Color(colour.r, colour.g, colour.b, colour.a + rate));
    }
    else if (type == "out") {
        if (colour.a > goal + 1) TextLabel::Text.setFillColor(Color(colour.r, colour.g, colour.b, colour.a - rate));
    }
}

void TextLabel::Draw() {
    GameWindow.draw(TextLabel::Text);
}

/*
    Button
    You click it

    text        - What it'll say
    x, y        - Size of the frame
    font        - Font used for the selection text
    fontsize    - No need to explain
*/

TextButton::TextButton(string text, float x, float y, Font &font, int fontsize) {
    TextButton::Button.setSize(Vector2f(x, y));
    TextButton::Button.setFillColor(TextButton::FrameColour);
    TextButton::Button.setOutlineColor(TextButton::FrameColour);
    //TextButton::Button.setOutlineThickness(10); // Commented to test button position

    TextButton::Text.setString(text);
    TextButton::Text.setFont(font);
    TextButton::Text.setCharacterSize(fontsize);
}

void TextButton::SetText(string text) {
    TextButton::Text.setString(text);
}

bool TextButton::Hover(RectangleShape &cursor) {
    Color ButtonColour = TextButton::FrameColour;
    float ButtonOutline = TextButton::Button.getOutlineThickness();
    if (TextButton::Button.getGlobalBounds().intersects(cursor.getGlobalBounds())) {
        TextButton::Button.setFillColor(ButtonColour);
        TextButton::Button.setOutlineColor(ButtonColour);
        TextButton::Text.setFillColor(TextButton::ActiveColour);
        if (ButtonOutline < 10) TextButton::Button.setOutlineThickness(ButtonOutline + 2);
        return true;
    }
    else {
       TextButton::Button.setFillColor(sf::Color(ButtonColour.r, ButtonColour.g, ButtonColour.b, 0));
       TextButton::Text.setFillColor(TextButton::InactiveColour);
       TextButton::Button.setOutlineThickness(0);
       return false;
    }
}

bool TextButton::Clicked() {
    if (Mouse::isButtonPressed(Mouse::Left) && TextButton::ClickedClock.getElapsedTime().asSeconds() > 0.35) {
        Color ButtonColour = TextButton::FrameColour;
        TextButton::Button.setFillColor(sf::Color(ButtonColour.r, ButtonColour.g, ButtonColour.b, 0));
        TextButton::Text.setFillColor(TextButton::InactiveColour);
        TextButton::Button.setOutlineThickness(0);
        TextButton::ClickedClock.restart();
        return true;
    }
    else return false;
}

void TextButton::Draw() {
    GameWindow.draw(TextButton::Button);
    GameWindow.draw(TextButton::Text);
}

void TextButton::EaseMove(string direction, float initial, float goal, float time) {
    float EaseTime = TextButton::EaseClock.getElapsedTime().asSeconds();
    Vector2f ButtonPos = TextButton::Button.getPosition();
    if (EaseTime <= time) {
        float Value = QuadEaseOut(EaseTime, initial, goal, time);
        if (direction == "horizontal") {
            TextButton::Button.setPosition(Value, TextButton::Button.getPosition().y);
            TextButton::Text.setPosition(Value + 10, TextButton::Button.getPosition().y + 5);
        }
        else if (direction == "vertical") {
            TextButton::Button.setPosition(TextButton::Button.getPosition().x, Value);
            TextButton::Text.setPosition(TextButton::Button.getPosition().x + 10, Value + 5);
        }
    }
}

void TextButton::UpdateResolution(float x, float y) {
    TextButton::Button.setPosition(x, y);
    float PosX;
    if (TextButton::Alignment == "left") PosX = x + 10;
    else if (TextButton::Alignment == "center") PosX = x + TextButton::Button.getSize().x/2 - TextButton::Text.getGlobalBounds().width/2;
    else if (TextButton::Alignment == "right") PosX = x + TextButton::Button.getSize().x - TextButton::Text.getGlobalBounds().width - 10;
    TextButton::Text.setPosition(PosX, (int)(y + (TextButton::Button.getSize().y/2 - TextButton::Text.getGlobalBounds().height/1.25)));
}

void TextButton::SetAlignment(string type) {
    TextButton::Alignment = type;
}

/*
    Selector
    Left/Right option selector

    x, y        - Size of the frame
    font        - Font used for the selection text
    fontsize    - No need to explain
    highest     - Vector size of the selection options
*/

Selector::Selector(float x, float y, Font &font, int fontsize, int highest) {
    Selector::Frame.setSize(Vector2f(x, y));
    Selector::Frame.setFillColor(Color(50, 50, 50));
    Selector::Left.setSize(Vector2f(y, y));
    Selector::Left.setFillColor(TransparentColor);
    Selector::Right.setSize(Vector2f(y, y));
    Selector::Right.setFillColor(TransparentColor);
    Selector::LeftArrow.setString("<");
    Selector::LeftArrow.setFont(font);
    Selector::LeftArrow.setFillColor(Color::White);
    Selector::LeftArrow.setCharacterSize(25);
    Selector::RightArrow.setString(">");
    Selector::RightArrow.setFont(font);
    Selector::RightArrow.setFillColor(Color::White);
    Selector::RightArrow.setCharacterSize(25);

    Selector::Text.setCharacterSize(fontsize);
    Selector::Text.setFont(font);

    Selector::HighestIterator = highest - 1;
}

void Selector::EaseMove(string direction, float initial, float goal, float time) {
    float EaseTime = Selector::EaseClock.getElapsedTime().asSeconds();
    Vector2f FrameSize = Selector::Frame.getSize();
    if (EaseTime <= time) {
        float Value = QuadEaseOut(EaseTime, initial, goal, time);
        if (direction == "horizontal") {
            Selector::Frame.setPosition(Value, Selector::Frame.getPosition().y);
            Selector::Left.setPosition(Value, Selector::Frame.getPosition().y);
            Selector::LeftArrow.setPosition(Value + 10, Selector::Frame.getPosition().y + 2);
            Selector::Right.setPosition(Value + FrameSize.x - FrameSize.y, Selector::Frame.getPosition().y);
            Selector::RightArrow.setPosition(Value + FrameSize.x - FrameSize.y + 10, Selector::Frame.getPosition().y + 2);
            Selector::Text.setPosition(Value + Selector::Frame.getSize().x/2 - Selector::Text.getGlobalBounds().width/2, Selector::Frame.getPosition().y + 5);
        }
        else if (direction == "vertical") {
            Selector::Frame.setPosition(Selector::Frame.getPosition().x, Value);
            Selector::Left.setPosition(Selector::Frame.getPosition().x, Value);
            Selector::LeftArrow.setPosition(Selector::Frame.getPosition().x + 10, Value + 2);
            Selector::Right.setPosition(Selector::Frame.getPosition().x + FrameSize.x - FrameSize.y, Value);
            Selector::RightArrow.setPosition(Selector::Frame.getPosition().x + FrameSize.x - FrameSize.y + 10, Value + 2);
            Selector::Text.setPosition(Selector::Frame.getPosition().x + Selector::Frame.getSize().x/2 - Selector::Text.getGlobalBounds().width/2, Value + 5);
        }
    }
}

void Selector::Draw() {
    GameWindow.draw(Selector::Frame);
    GameWindow.draw(Selector::Left);
    GameWindow.draw(Selector::Right);
    GameWindow.draw(Selector::LeftArrow);
    GameWindow.draw(Selector::RightArrow);
    GameWindow.draw(Selector::Text);
}

bool Selector::Hover(RectangleShape &button, RectangleShape &cursor) {
    if (button.getGlobalBounds().intersects(cursor.getGlobalBounds())) return true;
    else return false;
}

void Selector::SetText(string text) {
    Vector2f Position = Selector::Frame.getPosition();
    Selector::Text.setString(text);
    Selector::Text.setPosition((int)(Position.x + Selector::Frame.getSize().x/2 - Selector::Text.getGlobalBounds().width/2), (int)(Position.y + 5));
}

bool Selector::LeftClicked(RectangleShape &cursor) {
    if (Selector::Hover(Selector::Left, cursor)) {
        Selector::LeftArrow.setFillColor(PrimaryButton);
        if (Mouse::isButtonPressed(Mouse::Left) && Selector::ClickedClock.getElapsedTime().asSeconds() > 0.35) {
            Selector::Iterator--;
            if (Selector::Iterator < 0) Selector::Iterator = Selector::HighestIterator;
            Selector:ClickedClock.restart();
            return true;
        }
        else return false;
    }
    else {
        Selector::LeftArrow.setFillColor(Color::White);
        return false;
    }
}

bool Selector::RightClicked(RectangleShape &cursor) {
    if (Selector::Hover(Selector::Right, cursor)) {
        Selector::RightArrow.setFillColor(PrimaryButton);
        if (Mouse::isButtonPressed(Mouse::Left) && Selector::ClickedClock.getElapsedTime().asSeconds() > 0.35) {
            Selector::Iterator++;
            if (Selector::Iterator > Selector::HighestIterator) Selector::Iterator = 0;
            Selector::ClickedClock.restart();
            return true;
        }
        else return false;
    }
    else {
        Selector::RightArrow.setFillColor(Color::White);
        return false;
    }
}

void Selector::UpdateResolution(float x, float y) {
    Vector2f FrameSize = Selector::Frame.getSize();
    Selector::Frame.setPosition(x, y);
    Selector::Left.setPosition(x, y);
    Selector::LeftArrow.setPosition(x + 10, y + 2);
    Selector::Right.setPosition(x + FrameSize.x - FrameSize.y, y);
    Selector::RightArrow.setPosition(x + FrameSize.x - FrameSize.y + 10, y + 2);
    Selector::Text.setPosition((int)(Selector::Frame.getPosition().x + FrameSize.x/2 - Selector::Text.getGlobalBounds().width/2), (int)(y + 5));
}

/*
    ImageLabel
    Pictures paint a thousand words

    directory   - Location of image
*/

ImageLabel::ImageLabel(string directory, string name) {
    ImageLabel::Textures[name].loadFromFile(directory);
    ImageLabel::Image.setTexture(ImageLabel::Textures[name]);
}

void ImageLabel::EaseMove(string direction, float initial, float goal, float time) {
    float EaseTime = ImageLabel::EaseClock.getElapsedTime().asSeconds();
    if (EaseTime <= time) {
        float Value = QuadEaseOut(EaseTime, initial, goal, time);
        if (direction == "horizontal") ImageLabel::Image.setPosition(Value, ImageLabel::Image.getPosition().y);
        else if (direction == "vertical") ImageLabel::Image.setPosition(ImageLabel::Image.getPosition().x, Value);
    }
}


void ImageLabel::Draw() {
    GameWindow.draw(ImageLabel::Image);
}

void ImageLabel::UpdateResolution(float x, float y) {
    ImageLabel::Image.setPosition(x, y);
}

void ImageLabel::SetColour(Color colour) {
    ImageLabel::Image.setColor(colour);
}

void ImageLabel::Scale(float x, float y) {
    ImageLabel::Image.setScale(x, y);
}

sf::Color ImageLabel::GetColour() {
    return ImageLabel::Image.getColor();
}

void ImageLabel::Fade(string type, float goal, float rate) {
    Color colour = ImageLabel::Image.getColor();
    if (type == "in") {
        if (colour.a < goal - 1) ImageLabel::Image.setColor(Color(colour.r, colour.g, colour.b, colour.a + rate));
    }
    else if (type == "out") {
        if (colour.a > goal + 1) ImageLabel::Image.setColor(Color(colour.r, colour.g, colour.b, colour.a - rate));
    }
}

void ImageLabel::LoadTexture(string directory, string name) {
    ImageLabel::Textures[name].loadFromFile(directory);
}

void ImageLabel::SetTexture(string name) {
    ImageLabel::Image.setTexture(ImageLabel::Textures[name]);
}

/*
    ProgressBar
    Is the thingy done yet?
*/

ProgressBar::ProgressBar(float x, float y, Color colour, Font &font, string text) {
    ProgressBar::Bar.setSize(Vector2f(x - y/2, y));
    ProgressBar::Bar.setFillColor(Color(100, 100, 100));
    ProgressBar::BarAmount.setSize(Vector2f(0, y));
    ProgressBar::BarAmount.setFillColor(colour);
    ProgressBar::LeftCircle.setRadius(y/2);
    ProgressBar::LeftCircle.setFillColor(Color(100, 100, 100));
    ProgressBar::RightCircle.setRadius(y/2);
    ProgressBar::RightCircle.setFillColor(Color(100, 100, 100));
    ProgressBar::Text.setString(text);
    ProgressBar::Text.setFont(font);
    ProgressBar::Text.setCharacterSize(y - 4);
    ProgressBar::Text.setFillColor(Color::White);
    ProgressBar::Text.setOutlineThickness(1);
    ProgressBar::Text.setOutlineColor(Color(0, 0, 0, 70));
    ProgressBar::LeftCircle.setPointCount(360);
    ProgressBar::RightCircle.setPointCount(360);
}

void ProgressBar::UpdateResolution(float x, float y) {
    float r = ProgressBar::LeftCircle.getRadius();
    Vector2f barSize = ProgressBar::Bar.getSize();
    ProgressBar::Bar.setPosition(x + r, y);
    ProgressBar::BarAmount.setPosition(x + r, y);
    ProgressBar::LeftCircle.setPosition(x, y);
    ProgressBar::RightCircle.setPosition(x + ProgressBar::Bar.getSize().x, y);
    ProgressBar::Text.setPosition(x + barSize.x/2 - ProgressBar::Text.getGlobalBounds().width/2, y + barSize.y/2 - ProgressBar::Text.getGlobalBounds().height/1.5); // holy shit
}

void ProgressBar::Draw() {
    GameWindow.draw(ProgressBar::LeftCircle);
    GameWindow.draw(ProgressBar::RightCircle);
    GameWindow.draw(ProgressBar::Bar);
    GameWindow.draw(ProgressBar::BarAmount);
    GameWindow.draw(ProgressBar::Text);
}

void ProgressBar::SetPercentage(float num, float den, bool update) {
    float percent = num/den;
    float xsize = percent * (ProgressBar::Bar.getSize().x);
    ProgressBar::BarAmount.setSize(Vector2f(xsize, ProgressBar::Bar.getSize().y));

    if (update) {
        ProgressBar::Text.setString(to_string((int)num) + "/" + to_string((int)den));
        Vector2f pos = ProgressBar::LeftCircle.getPosition();
        Vector2f barSize = ProgressBar::Bar.getSize();
        ProgressBar::Text.setPosition(pos.x + barSize.x/2 - ProgressBar::Text.getGlobalBounds().width/2, pos.y + barSize.y/2 - ProgressBar::Text.getGlobalBounds().height/1.5);
    }

    if (percent <= 0) {
        ProgressBar::LeftCircle.setFillColor(Color(100, 100, 100));
        ProgressBar::RightCircle.setFillColor(Color(100, 100, 100));
    }
    else if (percent > 0) ProgressBar::LeftCircle.setFillColor(ProgressBar::BarAmount.getFillColor());

    if (percent >= 1) ProgressBar::RightCircle.setFillColor(ProgressBar::BarAmount.getFillColor());
    else ProgressBar::RightCircle.setFillColor(Color(100, 100, 100));
}

/*
    RoundedFrame
    Progressbar but not really
*/

RoundedFrame::RoundedFrame(float x, float y, Color colour) {
    RoundedFrame::Panel.setSize(Vector2f(x - y/2, y));
    RoundedFrame::Panel.setFillColor(colour);
    RoundedFrame::LeftCircle.setRadius(y/2);
    RoundedFrame::LeftCircle.setFillColor(colour);
    RoundedFrame::RightCircle.setRadius(y/2);
    RoundedFrame::RightCircle.setFillColor(colour);
    RoundedFrame::LeftCircle.setPointCount(360);
    RoundedFrame::RightCircle.setPointCount(360);
}

void RoundedFrame::UpdateResolution(float x, float y) {
    float r = RoundedFrame::LeftCircle.getRadius();
    RoundedFrame::Panel.setPosition(x + r, y);
    RoundedFrame::LeftCircle.setPosition(x, y);
    RoundedFrame::RightCircle.setPosition(x + RoundedFrame::Panel.getSize().x, y);
}

void RoundedFrame::Draw() {
    GameWindow.draw(RoundedFrame::LeftCircle);
    GameWindow.draw(RoundedFrame::RightCircle);
    GameWindow.draw(RoundedFrame::Panel);
}


#endif // ENGINE_HEADER
