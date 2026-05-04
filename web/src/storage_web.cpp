#include "storage_web.h"
#include <emscripten.h>
#include <cstring>

EM_JS(int, js_storage_save, (const char* key, const void* data, int len), {
    try {
        var k = UTF8ToString(key);
        var bytes = new Uint8Array(Module.HEAPU8.buffer, data, len);
        var str = "";
        for (var i = 0; i < bytes.length; i++) {
            str += String.fromCharCode(bytes[i]);
        }
        localStorage.setItem("spreeder_" + k, btoa(str));
        return 1;
    } catch (e) {
        return 0;
    }
});

EM_JS(int, js_storage_load, (const char* key, void* data, int maxLen), {
    try {
        var k = UTF8ToString(key);
        var encoded = localStorage.getItem("spreeder_" + k);
        if (!encoded) return 0;
        var str = atob(encoded);
        var len = Math.min(str.length, maxLen);
        var bytes = new Uint8Array(Module.HEAPU8.buffer, data, len);
        for (var i = 0; i < len; i++) {
            bytes[i] = str.charCodeAt(i);
        }
        return len;
    } catch (e) {
        return 0;
    }
});

bool StorageWeb::save(const char* key, const void* data, size_t len) {
    return js_storage_save(key, data, static_cast<int>(len)) != 0;
}

bool StorageWeb::load(const char* key, void* data, size_t maxLen) {
    return js_storage_load(key, data, static_cast<int>(maxLen)) > 0;
}
