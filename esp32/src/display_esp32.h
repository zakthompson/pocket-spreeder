#ifndef POCKET_SPREEDER_DISPLAY_ESP32_H
#define POCKET_SPREEDER_DISPLAY_ESP32_H

#include "hal.h"

class DisplayESP32 : public hal::Display {
public:
    void init();

    int getWidth() const override;
    int getHeight() const override;
    int measureText(const char* text, int len) const override;
    void drawText(int x, int y, const char* text, int len, uint32_t color) override;
    void clear() override;
    void present() override;
};

#endif
