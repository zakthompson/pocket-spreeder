#ifndef POCKET_SPREEDER_READER_H
#define POCKET_SPREEDER_READER_H

#include "hal.h"
#include "types.h"

namespace core {

enum class ReaderState { PAUSED, DISPLAYING_WORD, BLANK_FRAME };

struct ReaderSaveData {
    int tokenIndex;
    int wpm;
};

struct ReaderConfig {
    static constexpr int DEFAULT_WPM = 300;
    static constexpr int MIN_WPM = 200;
    static constexpr int MAX_WPM = 600;
    static constexpr int WPM_STEP = 25;
    static constexpr int ORP_ANCHOR_X = 230;
    static constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
    static constexpr uint32_t COLOR_RED = 0xFF0000;
    static constexpr uint32_t COLOR_RETICLE = 0x444444;
    static constexpr int RETICLE_OFFSET_ABOVE = 20;
    static constexpr int RETICLE_OFFSET_BELOW = 14;
};

class Reader {
public:
    Reader(hal::Display& display, hal::Input& input, hal::Storage& storage);

    void loadTokens(const Token* tokens, int count);
    void restoreState();
    void tick(unsigned long deltaMicros);

    int getWpm() const;
    bool isPlaying() const;
    int getTokenIndex() const;
    ReaderState getState() const;

private:
    hal::Display& display_;
    hal::Input& input_;
    hal::Storage& storage_;

    const Token* tokens_;
    int tokenCount_;
    int tokenIndex_;
    int wpm_;
    ReaderState state_;
    unsigned long elapsed_;
    unsigned long currentDuration_;
    bool dirty_;

    void handleInput();
    void advanceWord();
    void render();
    void saveState();
    unsigned long computeDuration() const;
    int findNextDisplayable(int from, int direction) const;
};

} // namespace core

#endif
