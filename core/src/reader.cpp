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
    , state_(ReaderState::PAUSED)
    , elapsed_(0)
    , currentDuration_(0)
    , dirty_(true) {}

void Reader::loadTokens(const Token* tokens, int count) {
    tokens_ = tokens;
    tokenCount_ = count;
    tokenIndex_ = 0;
    elapsed_ = 0;
    state_ = ReaderState::PAUSED;
    dirty_ = true;
}

void Reader::restoreState() {
    ReaderSaveData data;
    if (storage_.load("reader", &data, sizeof(data))) {
        if (data.tokenIndex >= 0 && data.tokenIndex < tokenCount_) {
            tokenIndex_ = data.tokenIndex;
        }
        if (data.wpm >= ReaderConfig::MIN_WPM && data.wpm <= ReaderConfig::MAX_WPM) {
            wpm_ = data.wpm;
        }
        dirty_ = true;
    }
}

void Reader::tick(unsigned long deltaMicros) {
    handleInput();

    if (state_ == ReaderState::DISPLAYING_WORD || state_ == ReaderState::BLANK_FRAME) {
        elapsed_ += deltaMicros;
        if (elapsed_ >= currentDuration_) {
            elapsed_ = 0;
            advanceWord();
        }
    }

    if (dirty_) {
        render();
        dirty_ = false;
    }
}

void Reader::handleInput() {
    int delta = input_.getEncoderDelta();
    bool button = input_.getButtonPress();

    if (button) {
        if (state_ == ReaderState::PAUSED) {
            if (tokens_ && tokenCount_ > 0) {
                state_ = ReaderState::DISPLAYING_WORD;
                elapsed_ = 0;
                currentDuration_ = computeDuration();
                dirty_ = true;
            }
        } else {
            state_ = ReaderState::PAUSED;
            elapsed_ = 0;
            dirty_ = true;
            saveState();
        }
    }

    if (delta != 0) {
        if (state_ == ReaderState::PAUSED) {
            int target = findNextDisplayable(tokenIndex_, delta > 0 ? 1 : -1);
            if (target != tokenIndex_) {
                tokenIndex_ = target;
                dirty_ = true;
            }
        } else {
            int newWpm = wpm_ + delta * ReaderConfig::WPM_STEP;
            if (newWpm < ReaderConfig::MIN_WPM) newWpm = ReaderConfig::MIN_WPM;
            if (newWpm > ReaderConfig::MAX_WPM) newWpm = ReaderConfig::MAX_WPM;
            if (newWpm != wpm_) {
                wpm_ = newWpm;
                currentDuration_ = computeDuration();
                dirty_ = true;
            }
        }
    }
}

void Reader::advanceWord() {
    if (state_ == ReaderState::BLANK_FRAME) {
        int next = findNextDisplayable(tokenIndex_, 1);
        if (next != tokenIndex_) {
            tokenIndex_ = next;
            state_ = ReaderState::DISPLAYING_WORD;
            currentDuration_ = computeDuration();
        } else {
            state_ = ReaderState::PAUSED;
        }
        dirty_ = true;
        return;
    }

    int next = tokenIndex_ + 1;
    if (next >= tokenCount_) {
        state_ = ReaderState::PAUSED;
        dirty_ = true;
        return;
    }

    if (tokens_[next].isParagraphBreak) {
        tokenIndex_ = next;
        state_ = ReaderState::BLANK_FRAME;
        currentDuration_ = 60000000UL / wpm_;
    } else {
        tokenIndex_ = next;
        currentDuration_ = computeDuration();
    }
    dirty_ = true;
}

void Reader::render() {
    display_.clear();

    int centerY = display_.getHeight() / 2;

    display_.drawText(ReaderConfig::ORP_ANCHOR_X, centerY - ReaderConfig::RETICLE_OFFSET_ABOVE,
                      "|", 1, ReaderConfig::COLOR_RETICLE);
    display_.drawText(ReaderConfig::ORP_ANCHOR_X, centerY + ReaderConfig::RETICLE_OFFSET_BELOW,
                      "|", 1, ReaderConfig::COLOR_RETICLE);

    if (state_ != ReaderState::BLANK_FRAME && tokens_ && tokenIndex_ < tokenCount_) {
        const Token& tok = tokens_[tokenIndex_];
        if (tok.raw && tok.rawLen > 0 && !tok.isParagraphBreak) {
            int leadingOffset = static_cast<int>(tok.clean - tok.raw);
            int rawOrpIndex = tok.orpIndex + leadingOffset;

            int leftWidth = display_.measureText(tok.raw, rawOrpIndex);
            int wordStartX = ReaderConfig::ORP_ANCHOR_X - leftWidth;

            if (rawOrpIndex > 0) {
                display_.drawText(wordStartX, centerY, tok.raw, rawOrpIndex,
                                  ReaderConfig::COLOR_WHITE);
            }

            display_.drawText(wordStartX + leftWidth, centerY,
                              tok.raw + rawOrpIndex, 1, ReaderConfig::COLOR_RED);

            int orpCharWidth = display_.measureText(tok.raw + rawOrpIndex, 1);
            int rightLen = tok.rawLen - rawOrpIndex - 1;
            if (rightLen > 0) {
                display_.drawText(wordStartX + leftWidth + orpCharWidth, centerY,
                                  tok.raw + rawOrpIndex + 1, rightLen,
                                  ReaderConfig::COLOR_WHITE);
            }
        }
    }

    display_.present();
}

void Reader::saveState() {
    ReaderSaveData data;
    data.tokenIndex = tokenIndex_;
    data.wpm = wpm_;
    storage_.save("reader", &data, sizeof(data));
}

unsigned long Reader::computeDuration() const {
    unsigned long base = 60000000UL / wpm_;
    if (tokens_ && tokenIndex_ < tokenCount_ && !tokens_[tokenIndex_].isParagraphBreak) {
        return static_cast<unsigned long>(base * tokens_[tokenIndex_].pauseMultiplier);
    }
    return base;
}

int Reader::findNextDisplayable(int from, int direction) const {
    int candidate = from + direction;
    while (candidate >= 0 && candidate < tokenCount_) {
        if (!tokens_[candidate].isParagraphBreak) {
            return candidate;
        }
        candidate += direction;
    }
    return from;
}

int Reader::getWpm() const { return wpm_; }
bool Reader::isPlaying() const {
    return state_ == ReaderState::DISPLAYING_WORD || state_ == ReaderState::BLANK_FRAME;
}
int Reader::getTokenIndex() const { return tokenIndex_; }
ReaderState Reader::getState() const { return state_; }

} // namespace core
