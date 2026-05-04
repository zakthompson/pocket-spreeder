#ifndef POCKET_SPREEDER_STORAGE_WEB_H
#define POCKET_SPREEDER_STORAGE_WEB_H

#include "hal.h"

class StorageWeb : public hal::Storage {
public:
    bool save(const char* key, const void* data, size_t len) override;
    bool load(const char* key, void* data, size_t maxLen) override;
};

#endif
