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

void test_word_advances_after_duration() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello world foo";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start playback
    input.buttonPressed = true;
    reader.tick(0);
    ASSERT_EQ(0, reader.getTokenIndex(), "starts at token 0");

    // At 300 WPM, base interval = 200,000 microseconds
    // tick just under — should not advance
    reader.tick(199999);
    ASSERT_EQ(0, reader.getTokenIndex(), "no advance before duration");

    // tick the remaining 1 microsecond — should advance
    reader.tick(1);
    ASSERT_EQ(1, reader.getTokenIndex(), "advances after duration");
}

void test_pause_multiplier_affects_duration() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello, world";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start playback
    input.buttonPressed = true;
    reader.tick(0);

    // "hello," has pause multiplier 1.5, so duration = 300,000 micros at 300 WPM
    reader.tick(299999);
    ASSERT_EQ(0, reader.getTokenIndex(), "comma pause: no advance at 299999");

    reader.tick(1);
    ASSERT_EQ(1, reader.getTokenIndex(), "comma pause: advances at 300000");
}

void test_pauses_at_end_of_text() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "only";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start
    input.buttonPressed = true;
    reader.tick(0);
    ASSERT_EQ((int)core::ReaderState::DISPLAYING_WORD, (int)reader.getState(), "playing single word");

    // Expire the word
    reader.tick(200000);
    ASSERT_EQ((int)core::ReaderState::PAUSED, (int)reader.getState(), "pauses at end of text");
}

void test_paragraph_break_triggers_blank_frame() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "end.\n\nstart";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start
    input.buttonPressed = true;
    reader.tick(0);
    ASSERT_EQ(0, reader.getTokenIndex(), "at 'end.'");

    // Expire "end." (pause multiplier 2.5 -> 500,000 micros)
    reader.tick(500000);
    ASSERT_EQ((int)core::ReaderState::BLANK_FRAME, (int)reader.getState(), "enters blank frame");

    // Blank frame lasts one base interval (200,000 micros)
    reader.tick(199999);
    ASSERT_EQ((int)core::ReaderState::BLANK_FRAME, (int)reader.getState(), "still in blank frame");

    reader.tick(1);
    ASSERT_EQ((int)core::ReaderState::DISPLAYING_WORD, (int)reader.getState(), "exits blank frame");
    ASSERT_EQ(2, reader.getTokenIndex(), "now at 'start'");
}

void test_encoder_scrubs_while_paused() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "one two three";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    ASSERT_EQ(0, reader.getTokenIndex(), "starts at 0");

    // Scrub forward
    input.encoderDelta = 1;
    reader.tick(0);
    ASSERT_EQ(1, reader.getTokenIndex(), "scrub forward to 1");

    // Scrub forward again
    input.encoderDelta = 1;
    reader.tick(0);
    ASSERT_EQ(2, reader.getTokenIndex(), "scrub forward to 2");

    // Scrub past end -- stays put
    input.encoderDelta = 1;
    reader.tick(0);
    ASSERT_EQ(2, reader.getTokenIndex(), "clamped at end");

    // Scrub backward
    input.encoderDelta = -1;
    reader.tick(0);
    ASSERT_EQ(1, reader.getTokenIndex(), "scrub backward to 1");

    // Scrub past beginning -- stays put
    input.encoderDelta = -2;
    reader.tick(0);
    ASSERT_EQ(0, reader.getTokenIndex(), "clamped at start");
}

void test_scrub_skips_paragraph_breaks() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "before\n\nafter";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Token 0 = "before", token 1 = paragraph break, token 2 = "after"
    ASSERT_EQ(0, reader.getTokenIndex(), "at 'before'");

    // Scrub forward -- should skip paragraph break
    input.encoderDelta = 1;
    reader.tick(0);
    ASSERT_EQ(2, reader.getTokenIndex(), "skipped para break, at 'after'");

    // Scrub backward -- should skip paragraph break
    input.encoderDelta = -1;
    reader.tick(0);
    ASSERT_EQ(0, reader.getTokenIndex(), "skipped para break back, at 'before'");
}

void test_encoder_adjusts_wpm_while_playing() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello world foo";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    ASSERT_EQ(300, reader.getWpm(), "default WPM");

    // Start
    input.buttonPressed = true;
    reader.tick(0);

    // Increase WPM
    input.encoderDelta = 2;
    reader.tick(0);
    ASSERT_EQ(350, reader.getWpm(), "WPM increased by 2 steps");

    // Decrease WPM
    input.encoderDelta = -1;
    reader.tick(0);
    ASSERT_EQ(325, reader.getWpm(), "WPM decreased by 1 step");
}

void test_wpm_clamped_to_range() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello world";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start
    input.buttonPressed = true;
    reader.tick(0);

    // Try to exceed max
    input.encoderDelta = 20;
    reader.tick(0);
    ASSERT_EQ(600, reader.getWpm(), "WPM clamped at max");

    // Try to go below min
    input.encoderDelta = -20;
    reader.tick(0);
    ASSERT_EQ(200, reader.getWpm(), "WPM clamped at min");
}

void test_save_on_pause() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "one two three";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start, advance one word, then pause
    input.buttonPressed = true;
    reader.tick(0);
    reader.tick(200000); // advance to "two"

    input.buttonPressed = true;
    reader.tick(0); // pause -- should save

    ASSERT_TRUE(storage.hasData, "data was saved on pause");

    core::ReaderSaveData saved;
    std::memcpy(&saved, storage.savedData, sizeof(saved));
    ASSERT_EQ(1, saved.tokenIndex, "saved token index is 1");
    ASSERT_EQ(300, saved.wpm, "saved WPM is 300");
}

void test_restore_state() {
    reset();

    // Pre-populate storage
    core::ReaderSaveData preload;
    preload.tokenIndex = 2;
    preload.wpm = 400;
    storage.save("reader", &preload, sizeof(preload));

    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "one two three four";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());
    reader.restoreState();

    ASSERT_EQ(2, reader.getTokenIndex(), "restored token index");
    ASSERT_EQ(400, reader.getWpm(), "restored WPM");
}

void test_restore_invalid_data_keeps_defaults() {
    reset();

    // Pre-populate with out-of-range values
    core::ReaderSaveData bad;
    bad.tokenIndex = 999;
    bad.wpm = 9999;
    storage.save("reader", &bad, sizeof(bad));

    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "one two";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());
    reader.restoreState();

    ASSERT_EQ(0, reader.getTokenIndex(), "invalid index keeps default");
    ASSERT_EQ(300, reader.getWpm(), "invalid WPM keeps default");
}

void test_render_draws_reticle_and_word() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "window";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Initial render happens on first tick (dirty from loadTokens)
    reader.tick(0);

    // Should have: clear(1), present(1), drawText calls for reticle + word segments
    ASSERT_EQ(1, display.clearCount, "render: one clear");
    ASSERT_EQ(1, display.presentCount, "render: one present");

    // 2 reticle draws + up to 3 word segments (left, ORP, right)
    // "window": clean = "window", orpIndex = 2, leadingOffset = 0, rawOrpIndex = 2
    // left = "wi" (2 chars), ORP = "n" (1 char, RED), right = "dow" (3 chars)
    // Total drawText calls: 2 (reticle) + 3 (word) = 5
    ASSERT_EQ(5, display.drawTextCount, "render: 5 draw calls");

    // Verify ORP letter is drawn in red (3rd draw call after 2 reticle calls)
    ASSERT_EQ((int)core::ReaderConfig::COLOR_RED, (int)display.draws[3].color, "ORP letter is red");
    ASSERT_EQ(1, display.draws[3].len, "ORP letter is single char");
}

void test_render_only_when_dirty() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "hello world";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // First tick -- renders (dirty from loadTokens)
    reader.tick(0);
    ASSERT_EQ(1, display.clearCount, "first tick renders");

    // Second tick -- no state change, should not render
    reader.tick(1000);
    ASSERT_EQ(1, display.clearCount, "no re-render when not dirty");
}

void test_blank_frame_renders_only_reticle() {
    reset();
    core::Reader reader(display, input, storage);

    core::Tokenizer tok;
    const char* text = "end.\n\nstart";
    tok.tokenize(text, std::strlen(text));
    reader.loadTokens(tok.getTokens(), tok.getTokenCount());

    // Start
    input.buttonPressed = true;
    reader.tick(0);
    display.resetCounts();

    // Expire "end." to enter blank frame
    reader.tick(500000);

    // Should render: clear + 2 reticle draws + present (no word)
    ASSERT_EQ(1, display.clearCount, "blank frame: clear");
    ASSERT_EQ(2, display.drawTextCount, "blank frame: only reticle drawn");
    ASSERT_EQ(1, display.presentCount, "blank frame: present");
}

int main() {
    test_initial_state_is_paused();
    test_button_press_starts_playback();
    test_button_press_pauses_playback();
    test_no_playback_without_tokens();
    test_word_advances_after_duration();
    test_pause_multiplier_affects_duration();
    test_pauses_at_end_of_text();
    test_paragraph_break_triggers_blank_frame();
    test_encoder_scrubs_while_paused();
    test_scrub_skips_paragraph_breaks();
    test_encoder_adjusts_wpm_while_playing();
    test_wpm_clamped_to_range();
    test_save_on_pause();
    test_restore_state();
    test_restore_invalid_data_keeps_defaults();
    test_render_draws_reticle_and_word();
    test_render_only_when_dirty();
    test_blank_frame_renders_only_reticle();

    std::printf("\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}
