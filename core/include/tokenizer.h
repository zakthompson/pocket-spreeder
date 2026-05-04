#ifndef POCKET_SPREEDER_TOKENIZER_H
#define POCKET_SPREEDER_TOKENIZER_H

#include "types.h"

namespace core {

class Tokenizer {
public:
    Tokenizer();
    ~Tokenizer();

    void tokenize(const char* text, int textLen);
    const Token* getTokens() const;
    int getTokenCount() const;

private:
    Token* tokens_;
    int count_;
    int capacity_;

    void grow();
    void addToken(const char* raw, int rawLen);
};

} // namespace core

#endif
