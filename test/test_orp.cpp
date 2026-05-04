#include <cstdio>
#include <cstring>
#include "orp.h"

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT_EQ(expected, actual, msg) do { \
    tests_run++; \
    if ((expected) != (actual)) { \
        tests_failed++; \
        std::printf("FAIL: %s — expected %d, got %d\n", \
                    (msg), (int)(expected), (int)(actual)); \
    } \
} while (0)

#define ASSERT_FLOAT_EQ(expected, actual, msg) do { \
    tests_run++; \
    float diff = (expected) - (actual); \
    if (diff < -0.01f || diff > 0.01f) { \
        tests_failed++; \
        std::printf("FAIL: %s — expected %.2f, got %.2f\n", \
                    (msg), (float)(expected), (float)(actual)); \
    } \
} while (0)

#define ASSERT_STR_EQ(expected, actual, len, msg) do { \
    tests_run++; \
    if (std::strncmp((expected), (actual), (len)) != 0 || \
        (int)std::strlen(expected) != (len)) { \
        tests_failed++; \
        std::printf("FAIL: %s — expected \"%s\", got \"%.*s\"\n", \
                    (msg), (expected), (len), (actual)); \
    } \
} while (0)

void test_getOrpIndex() {
    ASSERT_EQ(0, core::getOrpIndex("I", 1),             "ORP 'I'");
    ASSERT_EQ(0, core::getOrpIndex("to", 2),            "ORP 'to'");
    ASSERT_EQ(0, core::getOrpIndex("the", 3),           "ORP 'the'");
    ASSERT_EQ(1, core::getOrpIndex("from", 4),          "ORP 'from'");
    ASSERT_EQ(1, core::getOrpIndex("every", 5),         "ORP 'every'");
    ASSERT_EQ(2, core::getOrpIndex("window", 6),        "ORP 'window'");
    ASSERT_EQ(2, core::getOrpIndex("quickly", 7),       "ORP 'quickly'");
    ASSERT_EQ(2, core::getOrpIndex("different", 9),     "ORP 'different'");
    ASSERT_EQ(3, core::getOrpIndex("understand", 10),   "ORP 'understand'");
    ASSERT_EQ(3, core::getOrpIndex("immediately", 11),  "ORP 'immediately'");
    ASSERT_EQ(4, core::getOrpIndex("extraordinary", 13), "ORP 'extraordinary'");
}

void test_stripPunctuation() {
    const char* clean;
    int cleanLen;

    cleanLen = core::stripPunctuation("Hello", 5, &clean);
    ASSERT_STR_EQ("Hello", clean, cleanLen, "strip 'Hello'");

    cleanLen = core::stripPunctuation("\"Hello,\"", 8, &clean);
    ASSERT_STR_EQ("Hello", clean, cleanLen, "strip '\"Hello,\"'");

    cleanLen = core::stripPunctuation("(quietly)", 9, &clean);
    ASSERT_STR_EQ("quietly", clean, cleanLen, "strip '(quietly)'");

    cleanLen = core::stripPunctuation("don't,", 6, &clean);
    ASSERT_STR_EQ("don't", clean, cleanLen, "strip \"don't,\"");

    cleanLen = core::stripPunctuation("well-known", 10, &clean);
    ASSERT_STR_EQ("well-known", clean, cleanLen, "strip 'well-known'");

    cleanLen = core::stripPunctuation("$47.50", 6, &clean);
    ASSERT_STR_EQ("$47.50", clean, cleanLen, "strip '$47.50'");

    cleanLen = core::stripPunctuation("Really?!", 8, &clean);
    ASSERT_STR_EQ("Really", clean, cleanLen, "strip 'Really?!'");

    cleanLen = core::stripPunctuation("[test]", 6, &clean);
    ASSERT_STR_EQ("test", clean, cleanLen, "strip '[test]'");
}

void test_getPauseMultiplier() {
    ASSERT_FLOAT_EQ(1.0f, core::getPauseMultiplier("hello", 5),   "pause 'hello'");
    ASSERT_FLOAT_EQ(1.5f, core::getPauseMultiplier("hello,", 6),  "pause 'hello,'");
    ASSERT_FLOAT_EQ(1.8f, core::getPauseMultiplier("hello;", 6),  "pause 'hello;'");
    ASSERT_FLOAT_EQ(1.8f, core::getPauseMultiplier("hello:", 6),  "pause 'hello:'");
    ASSERT_FLOAT_EQ(2.5f, core::getPauseMultiplier("hello.", 6),  "pause 'hello.'");
    ASSERT_FLOAT_EQ(2.5f, core::getPauseMultiplier("hello!", 6),  "pause 'hello!'");
    ASSERT_FLOAT_EQ(2.5f, core::getPauseMultiplier("hello?", 6),  "pause 'hello?'");
    ASSERT_FLOAT_EQ(3.0f, core::getPauseMultiplier("wait...", 7), "pause 'wait...'");
    ASSERT_FLOAT_EQ(2.5f, core::getPauseMultiplier("Really?!", 8), "pause 'Really?!'");
}

int main() {
    test_getOrpIndex();
    test_stripPunctuation();
    test_getPauseMultiplier();

    std::printf("\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}
