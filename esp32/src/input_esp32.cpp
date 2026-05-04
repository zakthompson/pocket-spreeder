#include "input_esp32.h"

void InputESP32::init(int pinA, int pinB, int pinButton) {
    pinA_ = pinA;
    pinB_ = pinB;
    pinButton_ = pinButton;
    // TODO: pinMode, attachInterrupt for encoder
}

int InputESP32::getEncoderDelta() {
    // TODO: read encoder position, return delta since last call
    return 0;
}

bool InputESP32::getButtonPress() {
    // TODO: read button GPIO, detect press edge
    return false;
}
