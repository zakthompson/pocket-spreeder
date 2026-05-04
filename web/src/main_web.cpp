#include <emscripten.h>
#include <cstring>
#include <cstdlib>
#include "reader.h"
#include "tokenizer.h"
#include "display_web.h"
#include "input_web.h"
#include "storage_web.h"

static DisplayWeb display;
static InputWeb input;
static StorageWeb storage;
static core::Reader reader(display, input, storage);
static core::Tokenizer tokenizer;

static unsigned long lastTime = 0;

static char* textBuffer = nullptr;

static const char* sampleText =
    "It was a bright cold day in April, and the clocks were striking thirteen. "
    "Winston Smith, his chin nuzzled into his breast in an effort to escape the "
    "vile wind, slipped quickly through the glass doors of Victory Mansions, "
    "though not quickly enough to prevent a swirl of gritty dust from entering "
    "along with him.\n\n"
    "The hallway smelt of boiled cabbage and old rag mats. At one end of it a "
    "coloured poster, too large for indoor display, had been tacked to the wall. "
    "It depicted simply an enormous face, more than a metre wide: the face of a "
    "man of about forty-five, with a heavy black moustache and ruggedly handsome "
    "features.\n\n"
    "Winston made for the stairs. It was no use trying the lift. Even at the best "
    "of times it was seldom working, and at present the electric current was cut "
    "off during daylight hours. It was part of the economy drive in preparation "
    "for Hate Week. The flat was seven flights up, and Winston, who was "
    "thirty-nine and had a varicose ulcer above his right ankle, went slowly, "
    "resting several times on the way.";

static void loadText(const char* text, int len) {
    free(textBuffer);
    textBuffer = static_cast<char*>(malloc(len));
    std::memcpy(textBuffer, text, len);
    tokenizer.tokenize(textBuffer, len);
    reader.loadTokens(tokenizer.getTokens(), tokenizer.getTokenCount());
    reader.fitFontToTokens();
}

extern "C" {
    void setReaderText(const char* text, int len) {
        loadText(text, len);
    }
}

EM_JS(void, js_update_status, (int wpm, int state), {
    document.getElementById('wpm').textContent = wpm;
    var stateStr = state === 0 ? 'Paused' : state === 1 ? 'Reading' : 'Paragraph';
    document.getElementById('state').textContent = stateStr;
});

void tick() {
    double now = emscripten_get_now() * 1000.0;
    unsigned long nowMicros = static_cast<unsigned long>(now);
    unsigned long delta = lastTime == 0 ? 0 : nowMicros - lastTime;
    lastTime = nowMicros;
    reader.tick(delta);
    js_update_status(reader.getWpm(), static_cast<int>(reader.getState()));
}

int main() {
    loadText(sampleText, std::strlen(sampleText));
    emscripten_set_main_loop(tick, 0, 1);
    return 0;
}
