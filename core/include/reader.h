#ifndef POCKET_SPREEDER_READER_H
#define POCKET_SPREEDER_READER_H

#include "hal.h"
#include "types.h"

namespace core {

class Tokenizer;

struct ReaderConfig {
    static constexpr int DEFAULT_WPM = 300;
    static constexpr int MIN_WPM = 200;
    static constexpr int MAX_WPM = 600;
    static constexpr int WPM_STEP = 25;
    static constexpr int ORP_ANCHOR_X = 230;
    static constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
    static constexpr uint32_t COLOR_RED = 0xFF0000;
};

class Reader {
public:
    Reader(hal::Display& display, hal::Input& input, hal::Storage& storage);

    void loadTokens(const Token* tokens, int count);
    void tick(unsigned long deltaMicros);

    int getWpm() const;
    bool isPlaying() const;
    int getTokenIndex() const;

private:
    hal::Display& display_;
    hal::Input& input_;
    hal::Storage& storage_;

    const Token* tokens_;
    int tokenCount_;
    int tokenIndex_;
    int wpm_;
    bool playing_;
    unsigned long elapsed_;
};

} // namespace core

#endif
