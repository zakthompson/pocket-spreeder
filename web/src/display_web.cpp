#include "display_web.h"
#include <emscripten.h>

EM_JS(int, js_measure_text, (const char* text, int len), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    var str = UTF8ToString(text, len);
    return ctx.measureText(str).width;
});

EM_JS(void, js_draw_text, (int x, int y, const char* text, int len, unsigned int color), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    var str = UTF8ToString(text, len);
    var r = (color >> 16) & 0xFF;
    var g = (color >> 8) & 0xFF;
    var b = color & 0xFF;
    ctx.fillStyle = 'rgb(' + r + ',' + g + ',' + b + ')';
    ctx.fillText(str, x, y);
});

EM_JS(void, js_clear, (), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    ctx.fillStyle = '#000000';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
});

EM_JS(void, js_set_font_size, (int size), {
    var canvas = document.getElementById('display');
    var ctx = canvas.getContext('2d');
    ctx.font = size + 'px system-ui, -apple-system, sans-serif';
    ctx.textBaseline = 'middle';
});

int DisplayWeb::getWidth() const { return 536; }
int DisplayWeb::getHeight() const { return 240; }

void DisplayWeb::setFontSize(int size) {
    js_set_font_size(size);
}

int DisplayWeb::measureText(const char* text, int len) const {
    return js_measure_text(text, len);
}

void DisplayWeb::drawText(int x, int y, const char* text, int len, uint32_t color) {
    js_draw_text(x, y, text, len, color);
}

void DisplayWeb::clear() {
    js_clear();
}

void DisplayWeb::present() {
    // Canvas is immediate-mode; nothing to flush
}
