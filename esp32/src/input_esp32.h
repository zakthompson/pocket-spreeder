#ifndef POCKET_SPREEDER_INPUT_ESP32_H
#define POCKET_SPREEDER_INPUT_ESP32_H

#include "hal.h"

class InputESP32 : public hal::Input {
public:
    void init(int pinA, int pinB, int pinButton);

    int getEncoderDelta() override;
    bool getButtonPress() override;

private:
    int pinA_ = -1;
    int pinB_ = -1;
    int pinButton_ = -1;
    int lastEncoded_ = 0;
    int encoderValue_ = 0;
    int lastReportedValue_ = 0;
    bool lastButtonState_ = false;
};

#endif
