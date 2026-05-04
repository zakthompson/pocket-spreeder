#include <emscripten.h>
#include "reader.h"
#include "display_web.h"
#include "input_web.h"
#include "storage_web.h"

static DisplayWeb display;
static InputWeb input;
static StorageWeb storage;
static core::Reader reader(display, input, storage);

static unsigned long lastTime = 0;

void tick() {
    double now = emscripten_get_now() * 1000.0;
    unsigned long nowMicros = static_cast<unsigned long>(now);
    unsigned long delta = lastTime == 0 ? 0 : nowMicros - lastTime;
    lastTime = nowMicros;
    reader.tick(delta);
}

int main() {
    emscripten_set_main_loop(tick, 0, 1);
    return 0;
}
