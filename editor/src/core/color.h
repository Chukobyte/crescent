#pragma once

struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    void Normalize() {
        r = r / 255.0f;
        g = g / 255.0f;
        b = b / 255.0f;
        a = a / 255.0f;
    }

    static Color CreateNormalizedColor(float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f) {
        Color color = Color{ red, green, blue, alpha };
        color.Normalize();
        return color;
    }
};
