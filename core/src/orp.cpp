#include "orp.h"

static bool isLeadingPunct(char c) {
    return c == '"' || c == '\'' || c == '(' || c == '[' || c == '{';
}

static bool isTrailingPunct(char c) {
    return c == '.' || c == ',' || c == '!' || c == '?' || c == ':' ||
           c == ';' || c == '"' || c == '\'' || c == ')' || c == ']' ||
           c == '}';
}

namespace core {

int getOrpIndex(const char*, int len) {
    if (len <= 3) return 0;
    if (len <= 5) return 1;
    if (len <= 9) return 2;
    if (len <= 12) return 3;
    return 4;
}

int stripPunctuation(const char* raw, int rawLen, const char** cleanStart) {
    int start = 0;
    while (start < rawLen && isLeadingPunct(raw[start])) {
        start++;
    }

    int end = rawLen;
    while (end > start && isTrailingPunct(raw[end - 1])) {
        if (raw[end - 1] == '.' && end >= 3 &&
            raw[end - 2] == '.' && raw[end - 3] == '.') {
            break;
        }
        end--;
    }

    *cleanStart = raw + start;
    return end - start;
}

float getPauseMultiplier(const char* raw, int rawLen) {
    if (rawLen == 0) return 1.0f;

    if (rawLen >= 3 &&
        raw[rawLen - 1] == '.' && raw[rawLen - 2] == '.' && raw[rawLen - 3] == '.') {
        return 3.0f;
    }

    char last = raw[rawLen - 1];
    if (last == '.' || last == '!' || last == '?') return 2.5f;
    if (last == ';' || last == ':') return 1.8f;
    if (last == ',') return 1.5f;

    return 1.0f;
}

} // namespace core
