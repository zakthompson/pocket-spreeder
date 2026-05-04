#include "storage_esp32.h"

void StorageESP32::init() {
    // TODO: LittleFS.begin()
}

bool StorageESP32::save(const char*, const void*, size_t) {
    // TODO: write to LittleFS
    return false;
}

bool StorageESP32::load(const char*, void*, size_t) {
    // TODO: read from LittleFS
    return false;
}
