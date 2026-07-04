#include "StutterEngine.h"
#include <cmath>
#include <algorithm>

namespace HVS {

StutterEngine::StutterEngine() = default;

StutterEngine::~StutterEngine() {
    if (circularBuffer != nullptr) {
        delete[] circularBuffer;
    }
}

void StutterEngine::prepare(double sampleRate_, int maxBlockSize_) {
    sampleRate = sampleRate_;
    maxBlockSize = maxBlockSize_;

    // Allocate circular buffer (max 2 seconds)
    circularBufferSize = static_cast<int>(sampleRate * MAX_BUFFER_SIZE_MS / 1000.0);
    if (circularBuffer != nullptr) {
        delete[] circularBuffer;
    }
    circularBuffer = new float[circularBufferSize];
    std::fill(circularBuffer, circularBuffer + circularBufferSize, 0.0f);

    reset();
}

void StutterEngine::reset() {
    writeIndex = 0;
    captureStart = 0;
    captureLength = 0;
    repeatCount = 0;
    currentRepeatIndex = 0;
    currentReadIndex = 0.0f;
    isStuttering = false;
    crossfadePhase = 0.0f;
    crossfadeSamples = 0;
    gatePhase = 0.0f;
    gateSamples = 0;
    pendingTrigger = false;
    pendingRelease = false;
}

void StutterEngine::setHostBPM(double bpm) {
    hostBPM.store(bpm, std::memory_order_relaxed);
}

void StutterEngine::setHostTempo(const juce::AudioPlayHead::TempoAndTimeSigMetaData& tempo) {
    // Could store additional tempo info if needed
    if (tempo.bpm > 0.0) {
        setHostBPM(tempo.bpm);
    }
}

void StutterEngine::triggerStutter() {
    pendingTrigger.store(true, std::memory_order_release);
}

void StutterEngine::releaseStutter() {
    pendingRelease.store(true, std::memory_order_release);
}

int StutterEngine::calculateSliceLength(const EngineParameters& params) const {
    if (params.stutterRateSync) {
        // Convert note division to milliseconds based on host BPM
        double bpm = hostBPM.load(std::memory_order_relaxed);
        if (bpm <= 0.0) bpm = 120.0;  // Fallback

        double beatLengthMs = (60.0 / bpm) * 1000.0;
        double divisor = 4.0;  // 1/4 note

        switch (static_cast<StutterRateDivision>(static_cast<int>(params.stutterRate + 0.5f))) {
            case StutterRateDivision::Quarter: divisor = 1.0; break;
            case StutterRateDivision::Eighth: divisor = 2.0; break;
            case StutterRateDivision::Sixteenth: divisor = 4.0; break;
            case StutterRateDivision::ThirtySecond: divisor = 8.0; break;
            case StutterRateDivision::EighthTriplet: divisor = 3.0; break;
            case StutterRateDivision::SixteenthTriplet: divisor = 6.0; break;
            case StutterRateDivision::ThirtySecondTriplet: divisor = 12.0; break;
            default: divisor = 4.0;
        }

        double lengthMs = beatLengthMs / divisor;
        return static_cast<int>(lengthMs * sampleRate / 1000.0);
    } else {
        // Free mode: interpret as milliseconds
        return static_cast<int>(params.stutterLength * sampleRate / 1000.0);
    }
}

int StutterEngine::calculateTotalDuration(const EngineParameters& params) const {
    if (params.stutterLengthSync) {
        double bpm = hostBPM.load(std::memory_order_relaxed);
        if (bpm <= 0.0) bpm = 120.0;

        double beatLengthMs = (60.0 / bpm) * 1000.0;
        double divisor = 4.0;

        switch (static_cast<StutterRateDivision>(static_cast<int>(params.stutterLength + 0.5f))) {
            case StutterRateDivision::Quarter: divisor = 1.0; break;
            case StutterRateDivision::Eighth: divisor = 2.0; break;
            case StutterRateDivision::Sixteenth: divisor = 4.0; break;
            case StutterRateDivision::ThirtySecond: divisor = 8.0; break;
            case StutterRateDivision::EighthTriplet: divisor = 3.0; break;
            case StutterRateDivision::SixteenthTriplet: divisor = 6.0; break;
            case StutterRateDivision::ThirtySecondTriplet: divisor = 12.0; break;
            default: divisor = 4.0;
        }

        double lengthMs = beatLengthMs / divisor;
        return static_cast<int>(lengthMs * sampleRate / 1000.0);
    } else {
        return static_cast<int>(params.stutterLength * sampleRate / 1000.0);
    }
}

int StutterEngine::getCrossfadeLengthSamples(float smoothnessPercent) const {
    // 0% = 0ms (hard), 100% = 40ms (smooth)
    float crossfadeMs = (smoothnessPercent / 100.0f) * 40.0f;
    return std::max(0, static_cast<int>(crossfadeMs * sampleRate / 1000.0));
}

float StutterEngine::equalPowerCrossfade(float phase) const {
    // Equal-power crossfade: sqrt curve for fade-in
    return std::sqrt(phase);
}

float StutterEngine::getGateEnvelopeValue(float phase, GateShape shape) const {
    switch (shape) {
        case GateShape::Square:
            return 1.0f;  // Full volume throughout
        case GateShape::Triangle:
            if (phase < 0.5f) {
                return phase * 2.0f;  // Ramp up
            } else {
                return (1.0f - phase) * 2.0f;  // Ramp down
            }
        case GateShape::Sine:
            return std::sin(phase * 3.14159265359f);  // π
        default:
            return 1.0f;
    }
}

void StutterEngine::generatePitchShifts(const EngineParameters& params) {
    pitchShifts.clear();
    pitchShifts.resize(static_cast<size_t>(params.repeatCount));

    float maxPitch = params.pitchDrift;

    for (size_t i = 0; i < pitchShifts.size(); ++i) {
        float shift = 0.0f;

        if (params.pitchDirection == PitchDirection::Up) {
            // Ramp up from 0 to maxPitch
            shift = (maxPitch / std::max(1.0f, params.repeatCount - 1.0f)) * i;
        } else if (params.pitchDirection == PitchDirection::Down) {
            // Ramp down from 0 to -maxPitch
            shift = -(maxPitch / std::max(1.0f, params.repeatCount - 1.0f)) * i;
        } else if (params.pitchDirection == PitchDirection::Random) {
            // Random pitch shift within range
            shift = (std::rand() / (float)RAND_MAX) * maxPitch * 2.0f - maxPitch;
        }

        // Apply randomization
        if (params.randomizeAmount > 0.0f) {
            float randomFactor = (std::rand() / (float)RAND_MAX) * params.randomizeAmount / 100.0f;
            shift *= (1.0f + randomFactor);
        }

        pitchShifts[i] = shift;
    }
}

void StutterEngine::generateFilterCutoffs(const EngineParameters& params) {
    filterCutoffs.clear();
    filterCutoffs.resize(static_cast<size_t>(params.repeatCount));

    float startCutoff = params.filterStart;
    float endCutoff = params.filterEnd;

    for (size_t i = 0; i < filterCutoffs.size(); ++i) {
        // Linear interpolation between start and end
        float t = (params.repeatCount > 1) ? i / (params.repeatCount - 1.0f) : 0.0f;
        filterCutoffs[i] = startCutoff + (endCutoff - startCutoff) * t;

        // Apply randomization
        if (params.randomizeAmount > 0.0f) {
            float randomFactor = (std::rand() / (float)RAND_MAX) * params.randomizeAmount / 100.0f;
            filterCutoffs[i] *= (1.0f + randomFactor);
            filterCutoffs[i] = std::clamp(filterCutoffs[i], 20.0f, 20000.0f);
        }
    }
}

float StutterEngine::linearInterpolate(float index) const {
    int intPart = static_cast<int>(index);
    float fracPart = index - intPart;

    intPart = intPart % circularBufferSize;
    int nextIndex = (intPart + 1) % circularBufferSize;

    float s1 = circularBuffer[intPart];
    float s2 = circularBuffer[nextIndex];

    return s1 + (s2 - s1) * fracPart;
}

float StutterEngine::pitchShiftedSample(float readIndex, float pitchShift, bool reverse) const {
    // Simple pitch shift via playback rate adjustment
    float pitchFactor = std::pow(2.0f, pitchShift / 12.0f);  // Convert semitones to factor
    float adjustedIndex = readIndex;

    if (reverse) {
        adjustedIndex = -adjustedIndex;
    }

    return linearInterpolate(adjustedIndex * pitchFactor);
}

float StutterEngine::applyLowPassFilter(float sample, float cutoffHz, float& filterState) const {
    // One-pole low-pass filter
    float rc = 1.0f / (2.0f * 3.14159265359f * cutoffHz);
    float dt = 1.0f / static_cast<float>(sampleRate);
    float alpha = dt / (rc + dt);

    filterState = alpha * sample + (1.0f - alpha) * filterState;
    return filterState;
}

void StutterEngine::process(juce::AudioBuffer<float>& buffer, const EngineParameters& params) {
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // Check for pending trigger/release
    if (pendingTrigger.exchange(false, std::memory_order_acquire)) {
        isStuttering = true;
        captureLength = calculateSliceLength(params);
        captureStart = (writeIndex - captureLength + circularBufferSize) % circularBufferSize;
        repeatCount = static_cast<int>(params.repeatCount);
        currentRepeatIndex = 0;
        currentReadIndex = 0.0f;
        crossfadePhase = 0.0f;
        crossfadeSamples = getCrossfadeLengthSamples(params.smoothness);
        gatePhase = 0.0f;

        // Generate pitch shifts and filter cutoffs for this stutter
        generatePitchShifts(params);
        generateFilterCutoffs(params);
    }

    if (pendingRelease.exchange(false, std::memory_order_acquire)) {
        isStuttering = false;
    }

    // Process each sample
    for (int ch = 0; ch < numChannels; ++ch) {
        float* channelData = buffer.getWritePointer(ch);
        float filterState = 0.0f;

        for (int s = 0; s < numSamples; ++s) {
            float inputSample = channelData[s];

            // Write to circular buffer
            circularBuffer[writeIndex] = inputSample;
            writeIndex = (writeIndex + 1) % circularBufferSize;

            float outputSample = inputSample;

            if (isStuttering && currentRepeatIndex < repeatCount) {
                // Calculate read position in the captured slice
                float slicePosition = currentReadIndex / captureLength;

                // Apply gate envelope
                float gateValue = getGateEnvelopeValue(slicePosition, params.gateShape);

                // Get pitch shift and filter cutoff for this repeat
                float pitchShift = (currentRepeatIndex < (int)pitchShifts.size()) ? pitchShifts[currentRepeatIndex] : 0.0f;
                float filterCutoff = (currentRepeatIndex < (int)filterCutoffs.size()) ? filterCutoffs[currentRepeatIndex] : 8000.0f;

                // Read from captured buffer with pitch shifting
                int readPos = (captureStart + (int)currentReadIndex) % circularBufferSize;
                float stutterSample = linearInterpolate(static_cast<float>(readPos) + pitchShift / 12.0f);

                // Apply filter
                stutterSample = applyLowPassFilter(stutterSample, filterCutoff, filterState);

                // Apply gate
                stutterSample *= gateValue;

                // Apply crossfade at loop point
                if (crossfadeSamples > 0 && crossfadePhase < 1.0f) {
                    float crossfadeGain = equalPowerCrossfade(crossfadePhase);
                    stutterSample *= crossfadeGain;
                    crossfadePhase += 1.0f / crossfadeSamples;
                }

                // Wet/Dry mix
                float mix = params.wetDryMix / 100.0f;
                outputSample = inputSample * (1.0f - mix) + stutterSample * mix;

                // Advance read position
                currentReadIndex += 1.0f;

                // Loop back when slice ends
                if (currentReadIndex >= captureLength) {
                    currentReadIndex = 0.0f;
                    currentRepeatIndex++;
                    crossfadePhase = 0.0f;
                    gatePhase = 0.0f;
                }
            }

            // Apply output gain
            float gainLinear = std::pow(10.0f, params.outputGain / 20.0f);
            outputSample *= gainLinear;

            channelData[s] = outputSample;
        }
    }
}

}  // namespace HVS
