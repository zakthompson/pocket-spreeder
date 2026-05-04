#include "reader.h"

namespace core {

Reader::Reader(hal::Display& display, hal::Input& input, hal::Storage& storage)
    : display_(display)
    , input_(input)
    , storage_(storage)
    , tokens_(nullptr)
    , tokenCount_(0)
    , tokenIndex_(0)
    , wpm_(ReaderConfig::DEFAULT_WPM)
    , playing_(false)
    , elapsed_(0) {}

void Reader::loadTokens(const Token* tokens, int count) {
    tokens_ = tokens;
    tokenCount_ = count;
    tokenIndex_ = 0;
    elapsed_ = 0;
}

void Reader::tick(unsigned long) {
    // TODO: implement — poll input, advance timing, render word via display
}

int Reader::getWpm() const { return wpm_; }
bool Reader::isPlaying() const { return playing_; }
int Reader::getTokenIndex() const { return tokenIndex_; }

} // namespace core
