#include <cstdio>
#include <cstring>
#include "reader.h"
#include "tokenizer.h"
#include "mock_hal.h"

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

static MockDisplay display;
static MockInput input;
static MockStorage storage;

static void reset() {
    display = MockDisplay();
    input = MockInput();
    storage = MockStorage();
}

void test_initial_state_is_paused() {
    reset();
    core::Reader reader(display, input, storage);
    ASSERT_EQ((int)core::ReaderState::PAUSED, (int)reader.getState(), "initial state is PAUSED");
    ASSERT_FALSE(reader.isPlaying(), "not playing initially");
}

void test_button_press_starts_playback() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello world";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    input.buttonPressed = true;
    reader.tick(0);

    ASSERT_EQ((int)core::ReaderState::DISPLAYING_WORD, (int)reader.getState(), "button starts playback");
    ASSERT_TRUE(reader.isPlaying(), "is playing after button");
}

void test_button_press_pauses_playback() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello world";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start
    input.buttonPressed = true;
    reader.tick(0);

    // Pause
    input.buttonPressed = true;
    reader.tick(0);

    ASSERT_EQ((int)core::ReaderState::PAUSED, (int)reader.getState(), "button pauses");
    ASSERT_FALSE(reader.isPlaying(), "not playing after pause");
}

void test_no_playback_without_tokens() {
    reset();
    core::Reader reader(display, input, storage);

    input.buttonPressed = true;
    reader.tick(0);

    ASSERT_EQ((int)core::ReaderState::PAUSED, (int)reader.getState(), "no play without tokens");
}

int main() {
    test_initial_state_is_paused();
    test_button_press_starts_playback();
    test_button_press_pauses_playback();
    test_no_playback_without_tokens();

    std::printf("\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}
