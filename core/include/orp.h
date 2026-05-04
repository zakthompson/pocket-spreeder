#ifndef POCKET_SPREEDER_ORP_H
#define POCKET_SPREEDER_ORP_H

namespace core {

int getOrpIndex(const char* cleanWord, int len);

int stripPunctuation(const char* raw, int rawLen, const char** cleanStart);

float getPauseMultiplier(const char* raw, int rawLen);

} // namespace core

#endif
