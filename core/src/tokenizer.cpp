#include "tokenizer.h"
#include "orp.h"

static bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

namespace core {

Tokenizer::Tokenizer() : tokens_(nullptr), count_(0), capacity_(0) {}

Tokenizer::~Tokenizer() {
    delete[] tokens_;
}

void Tokenizer::tokenize(const char* text, int textLen) {
    count_ = 0;

    int i = 0;
    while (i < textLen) {
        if (text[i] == '\n') {
            int newlineCount = 0;
            int j = i;
            while (j < textLen && (text[j] == '\n' || text[j] == '\r')) {
                if (text[j] == '\n') newlineCount++;
                j++;
            }
            if (newlineCount >= 2) {
                addParagraphBreak();
                i = j;
            } else {
                i = j;
            }
        } else if (isWhitespace(text[i])) {
            i++;
        } else {
            int start = i;
            while (i < textLen && !isWhitespace(text[i])) {
                i++;
            }
            addToken(text + start, i - start);
        }
    }
}

const Token* Tokenizer::getTokens() const {
    return tokens_;
}

int Tokenizer::getTokenCount() const {
    return count_;
}

void Tokenizer::grow() {
    int newCap = capacity_ == 0 ? 256 : capacity_ * 2;
    Token* newBuf = new Token[newCap];
    for (int i = 0; i < count_; i++) {
        newBuf[i] = tokens_[i];
    }
    delete[] tokens_;
    tokens_ = newBuf;
    capacity_ = newCap;
}

void Tokenizer::addToken(const char* raw, int rawLen) {
    if (count_ >= capacity_) grow();

    Token& t = tokens_[count_++];
    t.raw = raw;
    t.rawLen = rawLen;
    t.cleanLen = stripPunctuation(raw, rawLen, &t.clean);
    t.orpIndex = getOrpIndex(t.clean, t.cleanLen);
    t.pauseMultiplier = getPauseMultiplier(raw, rawLen);
    t.isParagraphBreak = false;
}

void Tokenizer::addParagraphBreak() {
    if (count_ >= capacity_) grow();

    Token& t = tokens_[count_++];
    t.raw = nullptr;
    t.rawLen = 0;
    t.clean = nullptr;
    t.cleanLen = 0;
    t.orpIndex = 0;
    t.pauseMultiplier = 1.0f;
    t.isParagraphBreak = true;
}

} // namespace core
