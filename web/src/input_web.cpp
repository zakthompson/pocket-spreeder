#include "input_web.h"
#include <emscripten.h>

EM_JS(int, js_get_encoder_delta, (), {
    var delta = Module.encoderDelta || 0;
    Module.encoderDelta = 0;
    return delta;
});

EM_JS(int, js_get_button_press, (), {
    var pressed = Module.buttonPressed || false;
    Module.buttonPressed = false;
    return pressed ? 1 : 0;
});

int InputWeb::getEncoderDelta() {
    return js_get_encoder_delta();
}

bool InputWeb::getButtonPress() {
    return js_get_button_press() != 0;
}
