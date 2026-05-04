#include <cstdio>
#include <cstring>
#include "tokenizer.h"

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

#define ASSERT_TRUE(cond, msg) do { \
    tests_run++; \
    if (!(cond)) { \
        tests_failed++; \
        std::printf("FAIL: %s\n", (msg)); \
    } \
} while (0)

#define ASSERT_FALSE(cond, msg) do { \
    tests_run++; \
    if ((cond)) { \
        tests_failed++; \
        std::printf("FAIL: %s\n", (msg)); \
    } \
} while (0)

void test_basic_splitting() {
    core::Tokenizer tok;
    const char* text = "hello world";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(2, tok.getTokenCount(), "basic split: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("hello", tokens[0].raw, tokens[0].rawLen, "basic split: token 0 raw");
    ASSERT_STR_EQ("world", tokens[1].raw, tokens[1].rawLen, "basic split: token 1 raw");
}

void test_multiple_spaces() {
    core::Tokenizer tok;
    const char* text = "hello   world";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(2, tok.getTokenCount(), "multi-space: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("hello", tokens[0].raw, tokens[0].rawLen, "multi-space: token 0");
    ASSERT_STR_EQ("world", tokens[1].raw, tokens[1].rawLen, "multi-space: token 1");
}

void test_leading_trailing_whitespace() {
    core::Tokenizer tok;
    const char* text = "  hello world  ";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(2, tok.getTokenCount(), "leading/trailing ws: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("hello", tokens[0].raw, tokens[0].rawLen, "leading/trailing ws: token 0");
    ASSERT_STR_EQ("world", tokens[1].raw, tokens[1].rawLen, "leading/trailing ws: token 1");
}

void test_single_word() {
    core::Tokenizer tok;
    const char* text = "hello";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(1, tok.getTokenCount(), "single word: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("hello", tokens[0].raw, tokens[0].rawLen, "single word: raw");
}

void test_empty_input() {
    core::Tokenizer tok;
    tok.tokenize("", 0);
    ASSERT_EQ(0, tok.getTokenCount(), "empty: count");

    core::Tokenizer tok2;
    tok2.tokenize("   ", 3);
    ASSERT_EQ(0, tok2.getTokenCount(), "whitespace-only: count");
}

void test_clean_word_populated() {
    core::Tokenizer tok;
    const char* text = "\"Hello,\" (quietly) don't,";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(3, tok.getTokenCount(), "clean word: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("Hello", tokens[0].clean, tokens[0].cleanLen, "clean: \"Hello,\"");
    ASSERT_STR_EQ("quietly", tokens[1].clean, tokens[1].cleanLen, "clean: (quietly)");
    ASSERT_STR_EQ("don't", tokens[2].clean, tokens[2].cleanLen, "clean: don't,");
}

void test_orp_index_populated() {
    core::Tokenizer tok;
    const char* text = "I the from window understand extraordinary";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(6, tok.getTokenCount(), "orp index: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_EQ(0, tokens[0].orpIndex, "orp: I");
    ASSERT_EQ(0, tokens[1].orpIndex, "orp: the");
    ASSERT_EQ(1, tokens[2].orpIndex, "orp: from");
    ASSERT_EQ(2, tokens[3].orpIndex, "orp: window");
    ASSERT_EQ(3, tokens[4].orpIndex, "orp: understand");
    ASSERT_EQ(4, tokens[5].orpIndex, "orp: extraordinary");
}

void test_orp_index_with_punctuation() {
    core::Tokenizer tok;
    const char* text = "(quietly), \"Hello,\" well-known $47.50";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(4, tok.getTokenCount(), "orp+punct: count");

    const core::Token* tokens = tok.getTokens();
    // "quietly" = 7 letters → ORP index 2
    ASSERT_EQ(2, tokens[0].orpIndex, "orp: (quietly),");
    // "Hello" = 5 letters → ORP index 1
    ASSERT_EQ(1, tokens[1].orpIndex, "orp: \"Hello,\"");
    // "well-known" = 10 letters (with hyphen) → ORP index 3
    ASSERT_EQ(3, tokens[2].orpIndex, "orp: well-known");
    // "$47.50" = 6 chars → ORP index 2
    ASSERT_EQ(2, tokens[3].orpIndex, "orp: $47.50");
}

void test_pause_multiplier_populated() {
    core::Tokenizer tok;
    const char* text = "hello hello, hello; hello. wait...";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(5, tok.getTokenCount(), "pause: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_FLOAT_EQ(1.0f, tokens[0].pauseMultiplier, "pause: hello");
    ASSERT_FLOAT_EQ(1.5f, tokens[1].pauseMultiplier, "pause: hello,");
    ASSERT_FLOAT_EQ(1.8f, tokens[2].pauseMultiplier, "pause: hello;");
    ASSERT_FLOAT_EQ(2.5f, tokens[3].pauseMultiplier, "pause: hello.");
    ASSERT_FLOAT_EQ(3.0f, tokens[4].pauseMultiplier, "pause: wait...");
}

void test_paragraph_break() {
    core::Tokenizer tok;
    const char* text = "end.\n\nstart";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(3, tok.getTokenCount(), "para break: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("end.", tokens[0].raw, tokens[0].rawLen, "para break: token 0");
    ASSERT_TRUE(tokens[1].isParagraphBreak, "para break: token 1 is paragraph break");
    ASSERT_EQ(0, tokens[1].rawLen, "para break: token 1 rawLen");
    ASSERT_STR_EQ("start", tokens[2].raw, tokens[2].rawLen, "para break: token 2");
}

void test_multiple_paragraph_breaks() {
    core::Tokenizer tok;
    const char* text = "one.\n\ntwo.\n\n\nthree";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(5, tok.getTokenCount(), "multi para: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("one.", tokens[0].raw, tokens[0].rawLen, "multi para: token 0");
    ASSERT_TRUE(tokens[1].isParagraphBreak, "multi para: break 1");
    ASSERT_STR_EQ("two.", tokens[2].raw, tokens[2].rawLen, "multi para: token 2");
    ASSERT_TRUE(tokens[3].isParagraphBreak, "multi para: break 2");
    ASSERT_STR_EQ("three", tokens[4].raw, tokens[4].rawLen, "multi para: token 4");
}

void test_single_newline_is_space() {
    core::Tokenizer tok;
    const char* text = "hello\nworld";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(2, tok.getTokenCount(), "single newline: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("hello", tokens[0].raw, tokens[0].rawLen, "single newline: token 0");
    ASSERT_FALSE(tokens[0].isParagraphBreak, "single newline: not para break");
    ASSERT_STR_EQ("world", tokens[1].raw, tokens[1].rawLen, "single newline: token 1");
}

void test_tabs_as_whitespace() {
    core::Tokenizer tok;
    const char* text = "hello\tworld";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(2, tok.getTokenCount(), "tab split: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("hello", tokens[0].raw, tokens[0].rawLen, "tab split: token 0");
    ASSERT_STR_EQ("world", tokens[1].raw, tokens[1].rawLen, "tab split: token 1");
}

void test_non_paragraph_tokens_not_flagged() {
    core::Tokenizer tok;
    const char* text = "normal words here";
    tok.tokenize(text, std::strlen(text));

    const core::Token* tokens = tok.getTokens();
    for (int i = 0; i < tok.getTokenCount(); i++) {
        ASSERT_FALSE(tokens[i].isParagraphBreak, "normal token not para break");
    }
}

void test_spec_test_vectors() {
    core::Tokenizer tok;
    const char* text = "I to the from every window quickly different understand immediately extraordinary";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(11, tok.getTokenCount(), "spec vectors: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_EQ(0, tokens[0].orpIndex, "spec: I");
    ASSERT_EQ(0, tokens[1].orpIndex, "spec: to");
    ASSERT_EQ(0, tokens[2].orpIndex, "spec: the");
    ASSERT_EQ(1, tokens[3].orpIndex, "spec: from");
    ASSERT_EQ(1, tokens[4].orpIndex, "spec: every");
    ASSERT_EQ(2, tokens[5].orpIndex, "spec: window");
    ASSERT_EQ(2, tokens[6].orpIndex, "spec: quickly");
    ASSERT_EQ(2, tokens[7].orpIndex, "spec: different");
    ASSERT_EQ(3, tokens[8].orpIndex, "spec: understand");
    ASSERT_EQ(3, tokens[9].orpIndex, "spec: immediately");
    ASSERT_EQ(4, tokens[10].orpIndex, "spec: extraordinary");
}

void test_spec_punctuation_vectors() {
    core::Tokenizer tok;
    const char* text = "(quietly), \"Hello,\" don't, well-known $47.50";
    tok.tokenize(text, std::strlen(text));

    ASSERT_EQ(5, tok.getTokenCount(), "spec punct: count");

    const core::Token* tokens = tok.getTokens();

    // "(quietly)," → clean "quietly", ORP index 2
    ASSERT_STR_EQ("quietly", tokens[0].clean, tokens[0].cleanLen, "spec: (quietly), clean");
    ASSERT_EQ(2, tokens[0].orpIndex, "spec: (quietly), orp");

    // "\"Hello,\"" → clean "Hello", ORP index 1
    ASSERT_STR_EQ("Hello", tokens[1].clean, tokens[1].cleanLen, "spec: \"Hello,\" clean");
    ASSERT_EQ(1, tokens[1].orpIndex, "spec: \"Hello,\" orp");

    // "don't," → clean "don't", ORP index 1
    ASSERT_STR_EQ("don't", tokens[2].clean, tokens[2].cleanLen, "spec: don't, clean");
    ASSERT_EQ(1, tokens[2].orpIndex, "spec: don't, orp");

    // "well-known" → clean "well-known", ORP index 3
    ASSERT_STR_EQ("well-known", tokens[3].clean, tokens[3].cleanLen, "spec: well-known clean");
    ASSERT_EQ(3, tokens[3].orpIndex, "spec: well-known orp");

    // "$47.50" → clean "$47.50", ORP index 2
    ASSERT_STR_EQ("$47.50", tokens[4].clean, tokens[4].cleanLen, "spec: $47.50 clean");
    ASSERT_EQ(2, tokens[4].orpIndex, "spec: $47.50 orp");
}

void test_retokenize_resets() {
    core::Tokenizer tok;
    const char* text1 = "one two three";
    tok.tokenize(text1, std::strlen(text1));
    ASSERT_EQ(3, tok.getTokenCount(), "first tokenize: count");

    const char* text2 = "four five";
    tok.tokenize(text2, std::strlen(text2));
    ASSERT_EQ(2, tok.getTokenCount(), "second tokenize: count");

    const core::Token* tokens = tok.getTokens();
    ASSERT_STR_EQ("four", tokens[0].raw, tokens[0].rawLen, "retokenize: token 0");
    ASSERT_STR_EQ("five", tokens[1].raw, tokens[1].rawLen, "retokenize: token 1");
}

int main() {
    test_basic_splitting();
    test_multiple_spaces();
    test_leading_trailing_whitespace();
    test_single_word();
    test_empty_input();
    test_clean_word_populated();
    test_orp_index_populated();
    test_orp_index_with_punctuation();
    test_pause_multiplier_populated();
    test_paragraph_break();
    test_multiple_paragraph_breaks();
    test_single_newline_is_space();
    test_tabs_as_whitespace();
    test_non_paragraph_tokens_not_flagged();
    test_spec_test_vectors();
    test_spec_punctuation_vectors();
    test_retokenize_resets();

    std::printf("\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}
