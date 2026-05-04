#ifndef POCKET_SPREEDER_HAL_H
#define POCKET_SPREEDER_HAL_H

#include <cstddef>
#include <cstdint>

namespace hal {

class Display {
public:
    virtual ~Display() = default;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual int measureText(const char* text, int len) const = 0;
    virtual void drawText(int x, int y, const char* text, int len, uint32_t color) = 0;
    virtual void clear() = 0;
    virtual void present() = 0;
};

class Input {
public:
    virtual ~Input() = default;
    virtual int getEncoderDelta() = 0;
    virtual bool getButtonPress() = 0;
};

class Storage {
public:
    virtual ~Storage() = default;
    virtual bool save(const char* key, const void* data, size_t len) = 0;
    virtual bool load(const char* key, void* data, size_t maxLen) = 0;
};

} // namespace hal

#endif
