#ifndef POCKET_SPREEDER_DISPLAY_WEB_H
#define POCKET_SPREEDER_DISPLAY_WEB_H

#include "hal.h"

class DisplayWeb : public hal::Display {
public:
    int getWidth() const override;
    int getHeight() const override;
    void setFontSize(int size) override;
    int measureText(const char* text, int len) const override;
    void drawText(int x, int y, const char* text, int len, uint32_t color) override;
    void clear() override;
    void present() override;
};

#endif
