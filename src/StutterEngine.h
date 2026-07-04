#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <cstring>
#include "Parameters.h"

namespace HVS {

/**
 * @class StutterEngine
 * @brief Real-time safe circular buffer + capture/repeat/crossfade DSP logic.
 * 
 * This class handles:
 * - Real-time safe circular buffer that records incoming audio
 * - Capture and repeat logic with seamless crossfading
 * - Pitch shifting per repeat
 * - Reverse playback mode
 * - Filter sweep across repeats
 * - Gating envelope per repeat
 * 
 * All DSP is allocation-free and lock-free (uses atomics, no mutexes).
 */
class StutterEngine {
public:
    StutterEngine();
    ~StutterEngine();

    /**
     * Initialize the engine with sample rate and max buffer size.
     * Call this once during plugin initialization.
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * Process a block of audio.
     * Updates the circular buffer, handles repeat playback, crossfading, and DSP effects.
     */
    void process(juce::AudioBuffer<float>& buffer, const EngineParameters& params);

    /**
     * Trigger the stutter effect manually.
     * In real usage, this is called when the automation parameter changes or MIDI note arrives.
     */
    void triggerStutter();

    /**
     * Release the stutter effect (stop repeating).
     */
    void releaseStutter();

    /**
     * Reset internal state (buffer, repeat counter, etc.).
     * Call when the plugin is stopped or transport is reset.
     */
    void reset();

    /**
     * Set the current host BPM for tempo-synced stutter rate/length calculations.
     * Thread-safe via atomic.
     */
    void setHostBPM(double bpm);

    /**
     * Set the current host tempo info (for more precise sync).
     */
    void setHostTempo(const juce::AudioPlayHead::TempoAndTimeSigMetaData& tempo);

private:
    // Circular buffer
    static constexpr int MAX_BUFFER_SIZE_MS = 2000;
    float* circularBuffer = nullptr;
    int circularBufferSize = 0;
    int writeIndex = 0;

    // Capture/repeat state
    int captureStart = 0;
    int captureLength = 0;
    int repeatCount = 0;
    int currentRepeatIndex = 0;
    float currentReadIndex = 0.0f;
    bool isStuttering = false;

    // Crossfade state
    float crossfadePhase = 0.0f;  // 0.0 to 1.0
    int crossfadeSamples = 0;

    // Per-repeat pitch and other effects
    std::vector<float> pitchShifts;  // Precomputed pitch shifts for each repeat
    std::vector<float> filterCutoffs;  // Precomputed filter cutoffs for each repeat
    std::vector<bool> reverseFlags;  // Whether each repeat plays backward

    // Gate envelope state
    float gatePhase = 0.0f;
    int gateSamples = 0;

    // Engine parameters (cached from atomics)
    double sampleRate = 44100.0;
    int maxBlockSize = 512;
    std::atomic<double> hostBPM = 120.0;
    std::atomic<bool> pendingTrigger = false;
    std::atomic<bool> pendingRelease = false;

    // Helper functions (pure DSP, testable in isolation)

    /**
     * Calculate the length in samples of a stutter slice based on stutter rate.
     * Handles tempo-sync vs. free mode.
     */
    int calculateSliceLength(const EngineParameters& params) const;

    /**
     * Calculate total stutter duration in samples.
     * Handles tempo-sync vs. free mode.
     */
    int calculateTotalDuration(const EngineParameters& params) const;

    /**
     * Compute crossfade window (for seamless loop splicing).
     * At smoothness=0%, returns 0 (no crossfade, hard clicks).
     * At smoothness=100%, returns up to ~40ms crossfade.
     */
    int getCrossfadeLengthSamples(float smoothnessPercent) const;

    /**
     * Equal-power crossfade curve.
     * Takes a normalized phase (0.0 to 1.0) and returns fade-in gain [0..1].
     */
    float equalPowerCrossfade(float phase) const;

    /**
     * Gate envelope value at a given phase (0.0 to 1.0).
     * Shape determined by gateShape parameter (square, triangle, sine).
     */
    float getGateEnvelopeValue(float phase, GateShape shape) const;

    /**
     * Generate pitch shifts for each repeat based on pitch drift and direction.
     */
    void generatePitchShifts(const EngineParameters& params);

    /**
     * Generate filter cutoff values for each repeat based on filter sweep.
     */
    void generateFilterCutoffs(const EngineParameters& params);

    /**
     * Simple linear pitch-shift by resampling the audio.
     * (Production plugin might use granular or phase-vocoder approach,
     *  but this gives a basic pitch-shift for proof-of-concept.)
     */
    float pitchShiftedSample(float readIndex, float pitchShift, bool reverse) const;

    /**
     * Apply one-pole low-pass filter to a sample.
     */
    float applyLowPassFilter(float sample, float cutoffHz, float& filterState) const;

    /**
     * Linear interpolation for fractional sample reads.
     */
    float linearInterpolate(float index) const;
};

/**
 * @struct EngineParameters
 * @brief DSP parameters passed to process().
 * 
 * Decouples the engine from JUCE APVTS;
 * the PluginProcessor reads APVTS and constructs this struct each block.
 */
struct EngineParameters {
    float stutterRate;             // 0.0 = 1/4, 1.0 = 1/8, etc. (choice value)
    bool stutterRateSync;          // true = tempo-synced, false = free (Hz)
    float stutterLength;           // Duration in ms (or beats if synced)
    bool stutterLengthSync;
    float repeatCount;             // Number of repeats before release
    float smoothness;              // 0.0 = glitchy, 100.0 = smooth (%)
    float pitchDrift;              // Pitch shift range in semitones
    PitchDirection pitchDirection; // Up, Down, Random
    bool reverseMode;
    FilterType filterType;
    float filterStart;             // Start cutoff in Hz
    float filterEnd;               // End cutoff in Hz
    float randomizeAmount;         // % randomness applied to slice params
    GateShape gateShape;
    float wetDryMix;               // 0.0 = dry, 100.0 = wet (%)
    float outputGain;              // In dB
};

}  // namespace HVS
