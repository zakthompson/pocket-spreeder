#include <emscripten.h>
#include <cstring>
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

EM_JS(void, js_begin_font_sizing, (), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    Module._fontSizeRef = 100;
    ctx.font = Module._fontSizeRef + 'px system-ui, -apple-system, sans-serif';
    Module._worstScale = Infinity;
});

EM_JS(void, js_measure_token_for_sizing, (const char* raw, int rawLen, int rawOrpIndex), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    var str = UTF8ToString(raw, rawLen);
    var left = str.substring(0, rawOrpIndex);
    var right = str.substring(rawOrpIndex);
    var orpAnchorX = 230;
    var availLeft = orpAnchorX - 10;
    var availRight = canvas.width - orpAnchorX - 10;
    var scale = Infinity;
    if (left.length > 0) {
        var leftWidth = ctx.measureText(left).width;
        if (leftWidth > 0) scale = Math.min(scale, availLeft / leftWidth);
    }
    if (right.length > 0) {
        var rightWidth = ctx.measureText(right).width;
        if (rightWidth > 0) scale = Math.min(scale, availRight / rightWidth);
    }
    if (scale < Module._worstScale) Module._worstScale = scale;
});

EM_JS(void, js_apply_font_size, (), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    var fontSize = Math.floor(Module._fontSizeRef * Module._worstScale);
    ctx.font = fontSize + 'px system-ui, -apple-system, sans-serif';
    ctx.textBaseline = 'middle';
});

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
    tokenizer.tokenize(sampleText, std::strlen(sampleText));
    reader.loadTokens(tokenizer.getTokens(), tokenizer.getTokenCount());

    const core::Token* tokens = tokenizer.getTokens();
    int count = tokenizer.getTokenCount();
    js_begin_font_sizing();
    for (int i = 0; i < count; i++) {
        if (!tokens[i].isParagraphBreak && tokens[i].rawLen > 0) {
            int leadingOffset = static_cast<int>(tokens[i].clean - tokens[i].raw);
            int rawOrpIndex = tokens[i].orpIndex + leadingOffset;
            js_measure_token_for_sizing(tokens[i].raw, tokens[i].rawLen, rawOrpIndex);
        }
    }
    js_apply_font_size();

    emscripten_set_main_loop(tick, 0, 1);
    return 0;
}
