#ifndef FADING_HEADER
#define FADING_HEADER

#include <SFML/Graphics.hpp>
using namespace sf;

struct rgbProfile {
    rgbProfile() {
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;

        int rd = rand() % 2;
        int gd = rand() % 2;
        int bd = rand() % 2;

        if (rd == 0) rD = "in";
        else rD = "out";

        if (gd == 0) gD = "in";
        else gD = "out";

        if (bd == 0) bD = "in";
        else bD = "out";
    }

    void Routine() {
        if (rD == "in") r += rand() % 5;
        else r -= rand() % 5;

        if (gD == "in") g += rand() % 5;
        else g -= rand() % 5;

        if (bD == "in") b += rand() % 5;
        else b -= rand() % 5;

        // red
        if (r > 255) {
            r = 255;
            rD = "out";
        }
        else if (r < 0) {
            r = 0;
            rD = "in";
        }

        // green
        if (g > 255) {
            g = 255;
            gD = "out";
        }
        else if (g < 0) {
            g = 0;
            gD = "in";
        }

        // blue
        if (b > 255) {
            b = 255;
            bD = "out";
        }
        else if (b < 0) {
            b = 0;
            bD = "in";
        }
    }

    Color GetColour() {
        return Color(r, g, b);
    }

    int r, g, b;
    string rD, gD, bD;
};

#endif // FADING_HEADER
