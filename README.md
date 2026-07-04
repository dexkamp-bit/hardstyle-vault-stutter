# Hardstyle Vault Stutter VST3/AU Plugin

A professional-grade audio effect plugin for hardstyle music production, featuring advanced stutter/glitch effects with tempo-synced repetition, pitch modulation, dynamic filtering, and gate envelope control.

## Features

### Core Stutter Effect
- **Tempo-Synced Stutter Rate**: 1/4, 1/8, 1/16, 1/32 notes + triplet divisions
- **Free Mode Stutter**: Independent millisecond-based timing
- **Configurable Repeats**: 1–64 repetitions per trigger
- **Crossfade Smoothness**: 0% (glitchy) to 100% (seamless blending)

### Pitch & Modulation
- **Pitch Drift**: ±24 semitones per repeat
- **Pitch Direction**: Ramp up, ramp down, or randomized
- **Reverse Playback**: Optional backward repeats for creative effects

### Filtering & Dynamics
- **Dynamic Filter Sweep**: Low-pass, high-pass, or band-pass cutoff automation
- **Adjustable Gate Envelope**: Square (on/off), triangle (ramp), or sine curves
- **Randomization Control**: Add controlled chaos to slice parameters

### Triggering & Mix
- **Automation Parameter**: MIDI/host-based stutter trigger
- **MIDI Trigger Mode**: Note-on/note-off stutter control
- **Wet/Dry Mix**: 0–100% effect blending
- **Output Gain**: ±24 dB makeup gain

### UI/UX
- **Dark/Neon Aesthetic**: Hardstyle-inspired visual design with magenta, cyan, and acid green accents
- **Organized Parameter Sections**: Grouped controls for timing, pitch, filter, envelope, and mix
- **Rotary Knobs**: Smooth, intuitive parameter manipulation
- **Real-time Sync**: Host BPM integration for tempo-locked effects

## Project Structure

```
hardstyle-vault-stutter/
├── CMakeLists.txt              # JUCE CMake build configuration
├── src/
│   ├── Parameters.h            # Centralized parameter definitions & APVTS layout
│   ├── StutterEngine.h         # DSP engine header (circular buffer, effects)
│   ├── StutterEngine.cpp       # DSP implementation
│   ├── CustomLookAndFeel.h     # Dark/neon UI theme
│   ├── CustomLookAndFeel.cpp   # Theme implementation
│   ├── PluginProcessor.h       # Main audio processor
│   ├── PluginProcessor.cpp     # Processor implementation
│   ├── PluginEditor.h          # GUI editor
│   └── PluginEditor.cpp        # Editor implementation
├── JUCE/                       # JUCE framework (git submodule)
└── README.md                   # This file
```

## Build Instructions

### Prerequisites
- CMake 3.21+
- C++17 compiler
- JUCE framework (included as git submodule)

### Clone & Initialize
```bash
git clone https://github.com/dexkamp-bit/hardstyle-vault-stutter.git
cd hardstyle-vault-stutter
git submodule update --init --recursive
```

### Build (macOS/Linux)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Build (Windows)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Parameter Reference

| Parameter | Range | Default | Notes |
|-----------|-------|---------|-------|
| **Stutter Trigger** | On/Off | Off | Rising edge triggers effect |
| **Stutter Rate** | 1/4–1/32T | 1/16 | Synced to host tempo if enabled |
| **Stutter Rate Sync** | On/Off | On | Tempo-lock mode |
| **Stutter Length** | 10–500 ms | 100 ms | Duration per repeat |
| **Stutter Length Sync** | On/Off | On | Sync to tempo |
| **Repeat Count** | 1–64 | 4 | Number of repeats per trigger |
| **Smoothness** | 0–100% | 50% | Crossfade amount |
| **Pitch Drift** | ±24 st | 0 | Max pitch shift range |
| **Pitch Direction** | Up/Down/Random | Up | Pitch modulation pattern |
| **Reverse Mode** | On/Off | Off | Play repeats backward |
| **Filter Type** | LP/HP/BP | LP | Dynamic filter mode |
| **Filter Start** | 20–20 kHz | 8 kHz | Initial cutoff |
| **Filter End** | 20–20 kHz | 200 Hz | Final cutoff |
| **Randomize Amount** | 0–100% | 0% | Parameter variation |
| **Gate Shape** | Square/Triangle/Sine | Square | Envelope curve |
| **Wet/Dry Mix** | 0–100% | 100% | Effect blend |
| **Output Gain** | ±24 dB | 0 dB | Makeup gain |
| **MIDI Trigger Mode** | On/Off | Off | Use MIDI notes to trigger |

## DSP Architecture

### Circular Buffer
- Records incoming audio continuously (max 2 seconds)
- Lock-free design using atomics for real-time safety
- Efficient sample interpolation for smooth playback

### Stutter Engine
- Captures audio slice on trigger
- Repeats slice N times with configurable pitch shifts
- Applies dynamic filter sweep across repeats
- Blends repeats via equal-power crossfading
- Gate envelope applied per repeat

### Parameter Binding
- APVTS (AudioProcessorValueTreeState) for automation & presets
- Real-time safe parameter reads via atomic loads
- Edge detection for trigger events

## Audio Processing Pipeline

1. **Capture Phase**: On trigger, record audio slice to circular buffer
2. **Repeat Phase**: Loop captured slice N times, advancing read position
3. **Pitch Shift**: Apply per-repeat pitch modulation
4. **Filter**: Dynamic filter sweep from start to end cutoff
5. **Gate**: Apply envelope shape (square, triangle, sine)
6. **Crossfade**: Seamless loop blending
7. **Wet/Dry Mix**: Blend stuttered audio with dry signal
8. **Output Gain**: Apply makeup gain

## Audio Formats

- **VST3** (Windows, macOS, Linux)
- **AU** (macOS)
- **Standalone** (for testing)

## Development Notes

### Real-Time Safety
- All DSP runs in the audio thread
- Parameter reads use atomic loads (lock-free)
- Circular buffer supports concurrent read/write
- No dynamic allocations during processing

### Future Enhancements
- Granular synthesis mode (variable grain size)
- Phase vocoder pitch shifting (higher quality)
- Multi-tap delay integration
- Preset system with factory banks
- Visualization/scope display
- Sidechain modulation
- MIDI learn for parameter control

## License

[Insert your license here]

## Author

**Dexkamp** – hardstyle audio plugin development

---

**Note**: This plugin is optimized for hardstyle, rawstyle, and hardcore music production. Use the stutter effect to create signature DJ transitions, breakbeat fills, and glitch textures!
