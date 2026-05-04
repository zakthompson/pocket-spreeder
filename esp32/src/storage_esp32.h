#ifndef POCKET_SPREEDER_STORAGE_ESP32_H
#define POCKET_SPREEDER_STORAGE_ESP32_H

#include "hal.h"

class StorageESP32 : public hal::Storage {
public:
    void init();

    bool save(const char* key, const void* data, size_t len) override;
    bool load(const char* key, void* data, size_t maxLen) override;
};

#endif
