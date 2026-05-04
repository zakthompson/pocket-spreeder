#ifndef POCKET_SPREEDER_TYPES_H
#define POCKET_SPREEDER_TYPES_H

namespace core {

struct Token {
    const char* raw;
    int rawLen;
    const char* clean;
    int cleanLen;
    int orpIndex;
    float pauseMultiplier;
    bool isParagraphBreak;
};

} // namespace core

#endif
