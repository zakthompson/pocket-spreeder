#include "display_esp32.h"

void DisplayESP32::init() {
    // TODO: initialize LovyanGFX with AMOLED panel config
}

int DisplayESP32::getWidth() const { return 536; }
int DisplayESP32::getHeight() const { return 240; }

void DisplayESP32::setFontSize(int) {
    // TODO: use LovyanGFX setTextSize() / setFont()
}

int DisplayESP32::measureText(const char*, int) const {
    // TODO: use LovyanGFX textWidth()
    return 0;
}

void DisplayESP32::drawText(int, int, const char*, int, uint32_t) {
    // TODO: use LovyanGFX drawString()
}

void DisplayESP32::clear() {
    // TODO: use LovyanGFX fillScreen(TFT_BLACK)
}

void DisplayESP32::present() {
    // TODO: push sprite/buffer to display if double-buffered
}
