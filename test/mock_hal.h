#ifndef POCKET_SPREEDER_MOCK_HAL_H
#define POCKET_SPREEDER_MOCK_HAL_H

#include "hal.h"
#include <cstring>

class MockDisplay : public hal::Display {
public:
    int width = 536;
    int height = 240;
    int charWidth = 10;

    int clearCount = 0;
    int presentCount = 0;
    int drawTextCount = 0;

    struct DrawCall {
        int x, y, len;
        uint32_t color;
        char text[64];
    };
    DrawCall draws[32];

    int fontSize = 24;

    int getWidth() const override { return width; }
    int getHeight() const override { return height; }

    void setFontSize(int size) override { fontSize = size; }

    int measureText(const char*, int len) const override {
        return len * charWidth;
    }

    void drawText(int x, int y, const char* text, int len, uint32_t color) override {
        if (drawTextCount < 32) {
            DrawCall& d = draws[drawTextCount];
            d.x = x;
            d.y = y;
            d.len = len;
            d.color = color;
            int copyLen = len < 63 ? len : 63;
            std::memcpy(d.text, text, copyLen);
            d.text[copyLen] = '\0';
        }
        drawTextCount++;
    }

    void clear() override { clearCount++; }
    void present() override { presentCount++; }

    void resetCounts() {
        clearCount = 0;
        presentCount = 0;
        drawTextCount = 0;
    }
};

class MockInput : public hal::Input {
public:
    int encoderDelta = 0;
    bool buttonPressed = false;

    int getEncoderDelta() override {
        int d = encoderDelta;
        encoderDelta = 0;
        return d;
    }

    bool getButtonPress() override {
        bool b = buttonPressed;
        buttonPressed = false;
        return b;
    }
};

class MockStorage : public hal::Storage {
public:
    char savedKey[32] = {};
    char savedData[64] = {};
    size_t savedLen = 0;
    bool hasData = false;

    bool save(const char* key, const void* data, size_t len) override {
        std::strncpy(savedKey, key, 31);
        std::memcpy(savedData, data, len < 64 ? len : 64);
        savedLen = len;
        hasData = true;
        return true;
    }

    bool load(const char* key, void* data, size_t maxLen) override {
        if (!hasData) return false;
        if (std::strncmp(key, savedKey, 31) != 0) return false;
        size_t copyLen = savedLen < maxLen ? savedLen : maxLen;
        std::memcpy(data, savedData, copyLen);
        return true;
    }
};

#endif
