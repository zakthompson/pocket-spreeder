#ifndef POCKET_SPREEDER_INPUT_WEB_H
#define POCKET_SPREEDER_INPUT_WEB_H

#include "hal.h"

class InputWeb : public hal::Input {
public:
    int getEncoderDelta() override;
    bool getButtonPress() override;
};

#endif
