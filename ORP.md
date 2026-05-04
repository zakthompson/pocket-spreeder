Pleebz
Pleebz
pleebz
Cemu

Pleebz
just slid into the server. — 2026-04-14, 8:38 PM

Wave to say hi!
Pleebz — 2026-04-14, 8:38 PM
Spreeder
Huynh — 2026-04-14, 8:38 PM
im a spreeder
Pleebz — 2026-04-14, 8:38 PM
Man-spreeder
Huynh — 2026-04-14, 8:39 PM
:PES_Laugh:
Huynh — 2026-04-14, 8:48 PM
since colour and refresh rate matters, i think we move away from e-ink displays
the esp32 AMOLED 1.91" looks pretty appealing since its already packaged together well PLUS amoled black pixels are switched off so less overall power draw compared to other non e-ink displays
Pleebz — 2026-04-14, 9:04 PM
I'll never say no to an OLED
Pleebz — 2026-04-14, 10:19 PM
Mind giving admin perms? Gunna set up some stuff
Huynh — 2026-04-14, 10:19 PM
yeah one sec
done
i'm not hard set on anything in here yet so change to your hearts desire
Huynh — 2026-04-15, 4:00 PM
what hardware do you have for initial testing @Pleebz ?
Pleebz — 2026-04-15, 4:03 PM
I've got a bunch of UNO R3s around, which have different pin outs but if I abstract that I can test code on them. Breadboards, wires, and a few rotary encoders all on hand. Random other things I'm not sure we'll need - USB serial adapter, bluetooth board, etc. The one thing I don't have on hand is a small display
Huynh — 2026-04-15, 4:03 PM
yeah, let me order a display then
maybe i'll restock on ESP32s
Pleebz — 2026-04-15, 4:04 PM
If you just wanna order two of the ESP32 screen kits, I don't mind springing for one
Huynh — 2026-04-15, 4:04 PM
i
yeah, i was in the middle of typing that out
can def find a use for it outside of this project if they dont work out for us
Pleebz — 2026-04-15, 4:05 PM
For sure
Huynh — 2026-04-15, 6:58 PM
ordered 2, hopefully they dont take too long to get here
Pleebz — 2026-04-15, 7:56 PM
Sweet
Huynh — 2026-04-15, 9:41 PM
Proposed MVP Acceptance Criteria

A 50,000-word novel can be loaded and read in full without crashes
ORP letter is correctly calculated for all word lengths and rendered in red
ORP letter x-position remains fixed across all word lengths (verified visually)
Speed can be adjusted from 200 to 600 WPM in 25 WPM increments
Punctuation pauses are perceptible and improve comprehension (user test)
Reading position is correctly restored after power cycle
Battery life exceeds 10 hours in active reading conditions
Boot to reading mode completes in under 3 seconds
Device fits in standard jeans front pocket
Pleebz — 2026-04-15, 9:42 PM
Alright now attach the spec sheet for what constitutes "standard jeans"
Huynh — 2026-04-15, 9:42 PM
smh
Pleebz
pinned a message to this channel. See all pinned messages. — 2026-04-15, 9:42 PM
Huynh — 2026-04-15, 9:49 PM
ORP - Claude dump (unverified)

## ORP Implementation Requirements

---

### What ORP Actually Is

message.txt
10 KB
Pleebz — 2026-04-27, 1:42 AM
Where dat hardware at
Think it'll be in your hands by D&D?
Huynh — 2026-04-27, 2:08 AM
let me check the shipping
its in canada
Image
Pleebz — 2026-04-27, 2:44 AM
Nice, seems promising
Huynh — 2026-04-27, 10:26 AM
nice, even more promising
Image
released by customs
Huynh — 2026-04-29, 11:46 AM
Image
Pleebz — 2026-04-29, 12:26 PM
Ayyy
Huynh — 2026-04-29, 3:33 PM
Image
will bring sunday
Pleebz — 2026-04-29, 3:34 PM
Sick, I should have bandwidth next week to start scaffolding out the code too
Huynh — 2026-04-29, 3:41 PM
yeah, i'll keep sourcing parts for testing

## ORP Implementation Requirements

---

### What ORP Actually Is

The Optimal Recognition Point is the specific letter position where the eye's foveal vision (the high-resolution center) naturally anchors when recognizing a word. It's not the center of the word — it's offset left of center, because the brain begins processing a word before the eye reaches its physical midpoint.

The key insight for RSVP: instead of centering the word on screen, you pin the ORP letter to a fixed pixel position and let the rest of the word extend left and right from it. The eye never moves. Every word, regardless of length, presents its recognition anchor at exactly the same spot.

---

### ORP Position Rules

| Word length   | ORP letter (1-indexed) | Rationale                                                              |
| ------------- | ---------------------- | ---------------------------------------------------------------------- |
| 1 letter      | 1st                    | No choice; the whole word is the ORP                                   |
| 2–3 letters   | 1st                    | Short words recognized as a unit from the start                        |
| 4–5 letters   | 2nd                    | First internal letter; left of center                                  |
| 6–9 letters   | 3rd                    | Standard offset for common word lengths                                |
| 10–13 letters | 4th                    | Longer words shift anchor right but still left of center               |
| 14+ letters   | 5th                    | Very long words; anchor stabilizes at 5th regardless of further length |

---

### What Must Be Programmed

#### 1. Word cleaning before ORP calculation

The ORP position is calculated on the **semantic word** — not the raw token including punctuation. You must strip leading and trailing punctuation before counting letters, then restore it for display.

```
raw token:    "don't,"
clean word:   "don't"    ← ORP calculated on this (5 letters → ORP = 2nd)
displayed:    "don't,"   ← punctuation restored for display
```

Rules for stripping:

- Strip from the right: `. , ! ? : ; " ' ) ] }`
- Strip from the left: `" ' ( [ {`
- Do **not** strip internal punctuation: apostrophes in contractions (`don't`, `it's`), hyphens in hyphenated words
- Do **not** strip from the right if it's an ellipsis `...` — treat as sentence-end pause trigger

Special cases that need explicit handling:

- `"Hello,"` → clean = `Hello`, ORP = 3rd letter (`l`)
- `(quietly)` → clean = `quietly`, ORP = 3rd letter (`i`)
- `it's` → clean = `it's`, length = 4, ORP = 2nd letter (`t`)
- `well-known` → treat as single word, length = 9, ORP = 3rd letter (`l`)
- `$47.50` → treat as single token, length = 6, ORP = 3rd character (`4`)
- `...` or `—` alone → not a word; trigger a pause, skip display

---

#### 2. ORP pixel position (the fixed anchor)

Choose an absolute x-coordinate on the 240px-wide display and never deviate from it. This is the single most important implementation decision.

The ORP anchor should sit slightly left of horizontal center — roughly at x = 100–110px on a 240px-wide display. The exact value should be tuned visually during testing.

```
ORP_ANCHOR_X = 105  // pixels from left edge — tune this
```

Once set, every word is positioned by solving:

```
// Calculate where the word string starts
// such that the ORP letter falls exactly at ORP_ANCHOR_X

orp_index = get_orp_index(clean_word)  // 0-indexed

// Width of characters to the left of ORP letter
left_of_orp = character_width_sum(clean_word, 0, orp_index)

// Word rendering start x
word_start_x = ORP_ANCHOR_X - left_of_orp
```

This means a 3-letter word like "the" (ORP = 1st letter, index 0) renders with its first letter at ORP_ANCHOR_X, extending entirely to the right. A 14-letter word renders with 4 letters to the left of the anchor and 10 to the right.

---

#### 3. Character width handling

This is where most RSVP implementations get it wrong. You cannot assume monospaced rendering. With a proportional font (which you should use — it reads more naturally), each character has a different pixel width.

You need either:

**Option A — Measure at runtime:** Use the font library's `getTextBounds()` or character advance functions to measure actual pixel widths per character. Slower but accurate for any font.

**Option B — Pre-compute width table:** For your chosen font at your chosen size, pre-compute the pixel width of every printable ASCII character and store it as a lookup table. Fast, deterministic, but must be recalculated if you change font or size.

For RSVP at 600 WPM you have ~100ms per word — plenty of time for runtime measurement, but pre-computation is cleaner.

Special rendering requirement: **the ORP letter must be visually distinct from adjacent letters** without shifting its position. Options:

- Render ORP letter in red, all others in white — simplest and correct
- Do not change size of ORP letter — this would break pixel positioning
- Do not bold the ORP letter unless the entire font is already bold — inconsistent weight changes character advance width

---

#### 4. Vertical centering

The word should be vertically centered on the display. With a 536px-tall portrait display, the baseline of the word text should sit at approximately y = 268px (center). Use the font's ascent/descent metrics to center the cap-height, not the baseline — otherwise tall letters feel low.

The ORP anchor should be a visual reticle: a subtle fixed marker (a small dot or tick mark, 2–3px) at position `(ORP_ANCHOR_X, 268)` that remains on screen between word flashes. This trains the eye to hold position and reduces the startup cost of each new word.

---

#### 5. Hyphenated words and contractions

| Token                                      | Treatment              | Rationale                                                                        |
| ------------------------------------------ | ---------------------- | -------------------------------------------------------------------------------- |
| `don't`                                    | Single word, length 5  | Apostrophe is internal; word is a single unit                                    |
| `well-known`                               | Single word, length 9  | Hyphen is internal; treat as compound                                            |
| `San Francisco`                            | Two separate words     | Space is the word boundary                                                       |
| `twenty-three`                             | Single word, length 11 | Hyphenated number; single unit                                                   |
| `re-enter`                                 | Single word, length 7  | Prefixed word                                                                    |
| Very long hyphenated compounds (14+ chars) | Split at hyphen        | e.g., `counter-revolutionary` → display as two words at sentence-adjacent timing |

---

#### 6. Numbers and special tokens

| Token type    | Example          | Treatment                                                  |
| ------------- | ---------------- | ---------------------------------------------------------- |
| Integer       | `2026`           | Single word, length 4, ORP = 2nd                           |
| Decimal       | `3.14`           | Single word, length 4, strip trailing punctuation normally |
| Currency      | `$47.50`         | Single word — do not strip the `$`                         |
| Percentage    | `98%`            | Single word                                                |
| Ordinal       | `3rd`            | Single word, length 3, ORP = 1st                           |
| Roman numeral | `XIV`            | Single word, length 3, ORP = 1st                           |
| Initialism    | `NASA`           | Single word, length 4, ORP = 2nd                           |
| URL           | `www.google.com` | Extremely long — either skip or split at `.` boundaries    |
| Email address | `user@email.com` | Skip or display domain only                                |

---

#### 7. Pause triggers (not ORP but must coexist with it)

Every pause decision must be evaluated on the **raw token**, before punctuation stripping, after the ORP letter has been identified. Order of operations:

```
1. Read raw token from word buffer
2. Check for skip conditions (empty, URL, standalone punctuation)
3. Strip punctuation → clean word
4. Calculate ORP index on clean word
5. Render word with ORP letter in red
6. Determine display duration:
   a. Base interval from WPM setting
   b. Apply length multiplier if word.length ≥ 6
   c. Apply punctuation pause if raw token ends with pause-trigger character
7. After display duration, advance to next token
```

Punctuation that triggers a pause (check the raw token's trailing characters):

| Punctuation               | Pause multiplier             |
| ------------------------- | ---------------------------- |
| `,`                       | 1.5×                         |
| `;` `:`                   | 1.8×                         |
| `.` `!` `?`               | 2.5×                         |
| `...` (ellipsis)          | 3.0×                         |
| `—` (em-dash, standalone) | 1.8×                         |
| Chapter/section boundary  | 4.0× + display chapter title |

---

#### 8. Edge cases that will definitely break a naive implementation

- **All-caps words** (`STOP`, `NATO`) — ORP rule applies normally; don't treat differently
- **Single character tokens** (`I`, `a`) — ORP = 1st letter (index 0), word_start_x = ORP_ANCHOR_X
- **Possessives** (`James's`) — treat as single word, length 7, ORP = 3rd letter
- **Words with leading numbers** (`3D`, `4K`) — single word, ORP on length including number
- **Quoted words** (`"Yes"`) — strip outer quotes before ORP calc; display includes quotes
- **Words ending in multiple punctuation** (`Really?!`) — strip all trailing, pause = sentence-end
- **Paragraph breaks** — treat as sentence-end pause + one blank flash (empty display) to signal structure
- **Chapter headers** — display the header text for 2 seconds (static, not RSVP), then resume

---

### The Test Suite You Need

Before calling the engine done, run every word in this list through your ORP calculator and verify position and display manually:

```
"I"           → ORP index 0, letter "I"
"to"          → ORP index 0, letter "t"
"the"         → ORP index 0, letter "t"
"from"        → ORP index 1, letter "r"
"every"       → ORP index 1, letter "v"
"window"      → ORP index 2, letter "n"
"quickly"     → ORP index 2, letter "i"
"different"   → ORP index 2, letter "f"
"understand"  → ORP index 3, letter "e"
"immediately" → ORP index 3, letter "e"
"extraordinary" → ORP index 4, letter "a"
"(quietly),"  → strip to "quietly", ORP index 2, display "(quietly),"
"don't,"      → strip to "don't", ORP index 1, display "don't,"
"well-known"  → ORP index 2 on "well-known", letter "l"
"$47.50"      → ORP index 2 on "$47.50", letter "4"
```

Get all of these right and you'll get everything else right.
message.txt
10 KB
