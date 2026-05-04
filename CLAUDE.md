# Pocket Spreeder

A **tiny** ESP32-based RSVP (rapid serial visual presentation) e-reader. Uses ORP (optimal recognition point) highlighting to keep the reader's eye fixed on a specific point.

## Prototype Hardware

We have a **[LILYGO T-DisplayS3 AMOLED V2.0](https://lilygo.cc/en-ca/products/t-display-s3-amoled?variant=43506902368437)** that we're using for prototyping. The display is **held in landscape orientation** (536×240 pixels). We also have a generic rotary encoder which will be used for controls.

## Development Environment

Abstraction is key. We are prototyping - final hardware may change, and rapid iteration is critical. As best as possible, write everything modularly enough that it could be reused even if the underlying environment changes.

## Project Architecture

### Design Goal

The core business logic (ORP calculation, text tokenization, pause/timing, reading state machine) is written **once in C++** and compiled to two targets:

- **ESP32** — compiled natively via PlatformIO, linked against hardware drivers
- **Web simulator** — compiled to WebAssembly via Emscripten, driven by a thin JS shell with an HTML canvas and keyboard/mouse input

This lets us iterate on ORP rendering, timing, and UX in a browser without hardware attached, while guaranteeing the same logic runs on the device. The platform-specific code (display drivers, input handling, storage) diverges intentionally behind a Hardware Abstraction Layer — it is not duplicated.

### Layer Structure

```
┌──────────────────────────────┐
│          Core (C++)          │
│  ORP engine, text tokenizer, │
│  timing, reading state, WPM  │
└──────────────┬───────────────┘
               │
       ┌───────┴───────┐
       │  Platform HAL  │
       │ (abstract API) │
       └───────┬───────┘
               │
    ┌──────────┴──────────┐
    │                     │
┌───┴────┐         ┌─────┴─────┐
│ ESP32  │         │ Web/WASM  │
│ AMOLED │         │ <canvas>  │
│ GPIO   │         │ keyboard  │
│ SPIFFS │         │ localStorage │
└────────┘         └───────────┘
```

### Rules for Maintaining This Boundary

- **Core C++ must not include any platform headers** (no `<Arduino.h>`, no `<emscripten.h>`). It calls only the HAL interface.
- **The HAL interface is intentionally small**: `drawText()`, `clearScreen()`, `getInputEvent()`, `saveState()`, `loadState()`, etc. Resist expanding it without good reason.
- **Platform adapters implement the HAL** for their target. ESP32 adapter talks to TFT_eSPI / LovyanGFX and GPIO. Web adapter talks to JS via Emscripten bindings.
- **When adding new features**, write the logic in core, expose any new platform needs through the HAL, then implement both adapters. Never put business logic in an adapter.

## Planning Documents

Spec and planning documents (in `docs/superpowers/`) are **local reference only** — do not commit them to the repository. They are gitignored.

## ORP Spec

Whenever you are looking for details regarding the ORP spec for implementation, read @ORP.md
