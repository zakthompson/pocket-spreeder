#include "tokenizer.h"
#include "orp.h"

namespace core {

Tokenizer::Tokenizer() : tokens_(nullptr), count_(0), capacity_(0) {}

Tokenizer::~Tokenizer() {
    delete[] tokens_;
}

void Tokenizer::tokenize(const char*, int) {
    // TODO: implement — split on whitespace, call stripPunctuation/getOrpIndex/getPauseMultiplier
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

void Tokenizer::addToken(const char*, int) {
    // TODO: implement
}

} // namespace core
