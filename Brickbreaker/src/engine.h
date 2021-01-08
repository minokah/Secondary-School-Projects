// For use in ICS4U 2019-20
// Brandon "minokah" Luu
// Game: DESOCRETE

// EaseMove is unused and will probably be removed along with the QuadEaseOut function

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;

// Game Engine - UI
// I should've known about derived classes before I made these UI elements :(
// Oh well

namespace UI {
    class Frame {
    public:
        Frame(float x, float y, sf::Color colour);
        void EaseMove(string direction, float initial, float goal, float time);
        void UpdateResolution(float x, float y);
        void Draw();
        sf::RectangleShape Panel;
        sf::Clock EaseClock;
    };

    class TextLabel {
    public:
        TextLabel(string text, sf::Font &font, int fontsize);
        void SetText(string text);
        void EaseMove(string direction, float initial, float goal, float time);
        void UpdateResolution(float x, float y);
        void SetTransparency(float a);
        void SetColour(sf::Color colour);
        void Fade(string type, float goal, float rate);
        void Draw();
        sf::Text Text;
        sf::Clock EaseClock;
    };

    class TextButton {
    public:
        TextButton(string text, float x, float y, sf::Font &font, int fontsize);
        void SetText(string text);
        bool Hover(sf::RectangleShape &cursor);
        bool Clicked();
        void EaseMove(string direction, float initial, float goal, float time);
        void Draw();
        void UpdateResolution(float x, float y);
        void SetAlignment(string type);
        sf::RectangleShape Button;
        sf::Text Text;
        sf::Clock EaseClock, ClickedClock;
        string Alignment = "left";
        sf::Color InactiveColour = sf::Color::White;
        sf::Color ActiveColour = sf::Color::Black;
        sf::Color FrameColour = sf::Color(253, 220, 104);
    };

    class Selector {
    public:
        Selector(float x, float y, sf::Font &font, int fontsize, int highest);
        void EaseMove(string direction, float initial, float goal, float time);
        void Draw();
        bool Hover(sf::RectangleShape &button, sf::RectangleShape &cursor);
        bool LeftClicked(sf::RectangleShape &cursor);
        bool RightClicked(sf::RectangleShape &cursor);
        void SetText(string text);
        void UpdateResolution(float x, float y);
        sf::RectangleShape Frame, Left, Right;
        sf::Text Text, LeftArrow, RightArrow;
        sf::Clock EaseClock, ClickedClock;
        int Iterator = 0, HighestIterator;
    };

    class ImageLabel {
    public:
        ImageLabel(string directory, string name = "default");
        void Draw();
        void UpdateResolution(float x, float y);
        void EaseMove(string direction, float initial, float goal, float time);
        void SetColour(sf::Color colour);
        void Scale(float x, float y);
        void Fade(string type, float goal, float rate);
        void LoadTexture(string directory, string name);
        void SetTexture(string name);
        sf::Color GetColour();
        sf::Sprite Image;
        sf::Texture Texture;
        sf::Clock EaseClock;
        map<string, sf::Texture> Textures;
    };

    class ProgressBar {
    public:
        ProgressBar(float x, float y, sf::Color colour, sf::Font &font, string text);
        void UpdateResolution(float x, float y);
        void Draw();
        void SetPercentage(float num, float den, bool update = false);
        sf::Text Text;
        sf::RectangleShape Bar, BarAmount;
        sf::CircleShape LeftCircle, RightCircle;
    };

    class RoundedFrame {
    public:
        RoundedFrame(float x, float y, sf::Color colour);
        void UpdateResolution(float x, float y);
        void Draw();
        sf::RectangleShape Panel;
        sf::CircleShape LeftCircle, RightCircle;
    };
}

// Game Engine - Console
namespace Engine {
    extern sf::Font Roboto;
    extern sf::RenderWindow GameWindow;

    struct ResolutionPair {
        ResolutionPair(int xx, int yy);
        int x, y;
    };

    class Settings {
    public:
        Settings();
        vector<ResolutionPair> WindowResolutions;
        vector<string> WindowMode;
        vector<string> ToggleMode;
    };

    class Console {
    public:
        Console();
        vector<UI::TextLabel> Text;
        void Print(string text, string type, sf::Color colour);
        void Draw();
        void UpdateResolution(float x, float y);
        void Toggle();
        void Routine(sf::RectangleShape &cursor);
        void Clear();
        void Cancel();
        bool Toggled = true;
        bool Inputting = false;
        bool ProcessCommand = false;
        string Command, Value;
        sf::Clock GameRuntime, ToggleClock;
        UI::Frame StatPanel = UI::Frame(300, 25, sf::Color(253, 220, 104));
        UI::Frame StatSecondaryPanel = UI::Frame(300, 75, sf::Color(31, 31, 31));
        UI::TextButton ClearButton = UI::TextButton("CLEAR CONSOLE", 280, 5, Roboto, 15);
        UI::TextButton InputButton = UI::TextButton("INPUT COMMAND", 280, 5, Roboto, 15);
        UI::TextLabel StatText = UI::TextLabel("ENGINE", Roboto, 15);
        UI::TextLabel StatOwOText = UI::TextLabel("OwO", Roboto, 15);
        UI::TextLabel StatUptimeText = UI::TextLabel("hi", Roboto, 15);
        UI::ImageLabel UptimeImage = UI::ImageLabel("clock.png", "clock");
        UI::ImageLabel ClearImage = UI::ImageLabel("clear.png", "clear");
        UI::ImageLabel InputImage = UI::ImageLabel("input.png", "input");
        sf::Vector2f WindowSize;
    };
    extern Console GameConsole;
}
