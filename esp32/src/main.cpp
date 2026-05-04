#include <Arduino.h>
#include "reader.h"
#include "display_esp32.h"
#include "input_esp32.h"
#include "storage_esp32.h"

static DisplayESP32 display;
static InputESP32 input;
static StorageESP32 storage;
static core::Reader reader(display, input, storage);

static unsigned long lastMicros = 0;

void setup() {
    Serial.begin(115200);
    display.init();
    input.init(-1, -1, -1);  // TODO: set real GPIO pins
    storage.init();
    lastMicros = micros();
}

void loop() {
    unsigned long now = micros();
    unsigned long delta = now - lastMicros;
    lastMicros = now;
    reader.tick(delta);
}
