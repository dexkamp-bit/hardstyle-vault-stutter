#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace HVS {

// Parameter IDs (must be unique, used in automation)
namespace ParamID {
    constexpr auto STUTTER_TRIGGER = "stutterTrigger";
    constexpr auto STUTTER_RATE = "stutterRate";
    constexpr auto STUTTER_RATE_SYNC = "stutterRateSync";  // Toggle: tempo-synced vs. free
    constexpr auto STUTTER_LENGTH = "stutterLength";
    constexpr auto STUTTER_LENGTH_SYNC = "stutterLengthSync";
    constexpr auto REPEAT_COUNT = "repeatCount";
    constexpr auto SMOOTHNESS = "smoothness";
    constexpr auto PITCH_DRIFT = "pitchDrift";
    constexpr auto PITCH_DIRECTION = "pitchDirection";  // 0=Up, 1=Down, 2=Random
    constexpr auto REVERSE_MODE = "reverseMode";
    constexpr auto FILTER_TYPE = "filterType";  // 0=LP, 1=HP, 2=BP
    constexpr auto FILTER_START = "filterStart";
    constexpr auto FILTER_END = "filterEnd";
    constexpr auto RANDOMIZE_AMOUNT = "randomizeAmount";
    constexpr auto GATE_SHAPE = "gateShape";  // 0=Square, 1=Triangle, 2=Sine
    constexpr auto WET_DRY_MIX = "wetDryMix";
    constexpr auto OUTPUT_GAIN = "outputGain";
    constexpr auto MIDI_TRIGGER_MODE = "midiTriggerMode";
}

// Stutter rate note divisions
enum class StutterRateDivision {
    Quarter = 0,      // 1/4
    Eighth,            // 1/8
    Sixteenth,         // 1/16
    ThirtySecond,      // 1/32
    EighthTriplet,     // 1/8T
    SixteenthTriplet,  // 1/16T
    ThirtySecondTriplet, // 1/32T
    Count
};

inline const char* stutterRateDivisionToString(StutterRateDivision division) {
    switch (division) {
        case StutterRateDivision::Quarter: return "1/4";
        case StutterRateDivision::Eighth: return "1/8";
        case StutterRateDivision::Sixteenth: return "1/16";
        case StutterRateDivision::ThirtySecond: return "1/32";
        case StutterRateDivision::EighthTriplet: return "1/8T";
        case StutterRateDivision::SixteenthTriplet: return "1/16T";
        case StutterRateDivision::ThirtySecondTriplet: return "1/32T";
        default: return "Unknown";
    }
}

// Filter types
enum class FilterType { LP = 0, HP = 1, BP = 2, Count };
inline const char* filterTypeToString(FilterType type) {
    switch (type) {
        case FilterType::LP: return "Low-Pass";
        case FilterType::HP: return "High-Pass";
        case FilterType::BP: return "Band-Pass";
        default: return "Unknown";
    }
}

// Pitch direction
enum class PitchDirection { Up = 0, Down = 1, Random = 2, Count };
inline const char* pitchDirectionToString(PitchDirection dir) {
    switch (dir) {
        case PitchDirection::Up: return "Up";
        case PitchDirection::Down: return "Down";
        case PitchDirection::Random: return "Random";
        default: return "Unknown";
    }
}

// Gate shape
enum class GateShape { Square = 0, Triangle = 1, Sine = 2, Count };
inline const char* gateShapeToString(GateShape shape) {
    switch (shape) {
        case GateShape::Square: return "Square";
        case GateShape::Triangle: return "Triangle";
        case GateShape::Sine: return "Sine";
        default: return "Unknown";
    }
}

// Parameter ranges and defaults
namespace Defaults {
    constexpr float STUTTER_TRIGGER = 0.0f;
    constexpr float STUTTER_RATE = static_cast<float>(StutterRateDivision::Sixteenth);
    constexpr float STUTTER_RATE_SYNC = 1.0f;  // 1 = synced, 0 = free
    constexpr float STUTTER_LENGTH = static_cast<float>(StutterRateDivision::Quarter);
    constexpr float STUTTER_LENGTH_SYNC = 1.0f;
    constexpr float REPEAT_COUNT = 4.0f;
    constexpr float SMOOTHNESS = 50.0f;
    constexpr float PITCH_DRIFT = 0.0f;
    constexpr float PITCH_DIRECTION = static_cast<float>(PitchDirection::Up);
    constexpr float REVERSE_MODE = 0.0f;
    constexpr float FILTER_TYPE = static_cast<float>(FilterType::LP);
    constexpr float FILTER_START = 8000.0f;
    constexpr float FILTER_END = 200.0f;
    constexpr float RANDOMIZE_AMOUNT = 0.0f;
    constexpr float GATE_SHAPE = static_cast<float>(GateShape::Square);
    constexpr float WET_DRY_MIX = 100.0f;
    constexpr float OUTPUT_GAIN = 0.0f;
    constexpr float MIDI_TRIGGER_MODE = 0.0f;
}

namespace Ranges {
    constexpr float STUTTER_TRIGGER_MIN = 0.0f, STUTTER_TRIGGER_MAX = 1.0f;
    constexpr float STUTTER_RATE_MIN = 0.0f, STUTTER_RATE_MAX = static_cast<float>(StutterRateDivision::Count) - 1.0f;
    constexpr float STUTTER_LENGTH_MIN = 0.0f, STUTTER_LENGTH_MAX = 10000.0f;  // ms
    constexpr float REPEAT_COUNT_MIN = 1.0f, REPEAT_COUNT_MAX = 64.0f;
    constexpr float SMOOTHNESS_MIN = 0.0f, SMOOTHNESS_MAX = 100.0f;
    constexpr float PITCH_DRIFT_MIN = -24.0f, PITCH_DRIFT_MAX = 24.0f;  // semitones
    constexpr float PITCH_DIRECTION_MIN = 0.0f, PITCH_DIRECTION_MAX = static_cast<float>(PitchDirection::Count) - 1.0f;
    constexpr float REVERSE_MODE_MIN = 0.0f, REVERSE_MODE_MAX = 1.0f;
    constexpr float FILTER_TYPE_MIN = 0.0f, FILTER_TYPE_MAX = static_cast<float>(FilterType::Count) - 1.0f;
    constexpr float FILTER_CUTOFF_MIN = 20.0f, FILTER_CUTOFF_MAX = 20000.0f;
    constexpr float RANDOMIZE_AMOUNT_MIN = 0.0f, RANDOMIZE_AMOUNT_MAX = 100.0f;
    constexpr float GATE_SHAPE_MIN = 0.0f, GATE_SHAPE_MAX = static_cast<float>(GateShape::Count) - 1.0f;
    constexpr float WET_DRY_MIX_MIN = 0.0f, WET_DRY_MIX_MAX = 100.0f;
    constexpr float OUTPUT_GAIN_MIN = -24.0f, OUTPUT_GAIN_MAX = 24.0f;  // dB
    constexpr float MIDI_TRIGGER_MODE_MIN = 0.0f, MIDI_TRIGGER_MODE_MAX = 1.0f;
}

// Helper to create the APVTS parameter layout
inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamID::STUTTER_TRIGGER, 1),
        "Stutter Trigger",
        false
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamID::STUTTER_RATE, 1),
        "Stutter Rate",
        juce::StringArray("1/4", "1/8", "1/16", "1/32", "1/8T", "1/16T", "1/32T"),
        static_cast<int>(Defaults::STUTTER_RATE)
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamID::STUTTER_RATE_SYNC, 1),
        "Stutter Rate Sync",
        Defaults::STUTTER_RATE_SYNC > 0.5f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::STUTTER_LENGTH, 1),
        "Stutter Length",
        juce::NormalisableRange<float>(Ranges::STUTTER_LENGTH_MIN, Ranges::STUTTER_LENGTH_MAX, 1.0f),
        Defaults::STUTTER_LENGTH,
        "%",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamID::STUTTER_LENGTH_SYNC, 1),
        "Stutter Length Sync",
        Defaults::STUTTER_LENGTH_SYNC > 0.5f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::REPEAT_COUNT, 1),
        "Repeat Count",
        juce::NormalisableRange<float>(Ranges::REPEAT_COUNT_MIN, Ranges::REPEAT_COUNT_MAX, 1.0f),
        Defaults::REPEAT_COUNT,
        "",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::SMOOTHNESS, 1),
        "Smoothness",
        juce::NormalisableRange<float>(Ranges::SMOOTHNESS_MIN, Ranges::SMOOTHNESS_MAX),
        Defaults::SMOOTHNESS,
        "%",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::PITCH_DRIFT, 1),
        "Pitch Drift",
        juce::NormalisableRange<float>(Ranges::PITCH_DRIFT_MIN, Ranges::PITCH_DRIFT_MAX, 0.1f),
        Defaults::PITCH_DRIFT,
        " st",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamID::PITCH_DIRECTION, 1),
        "Pitch Direction",
        juce::StringArray("Up", "Down", "Random"),
        static_cast<int>(Defaults::PITCH_DIRECTION)
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamID::REVERSE_MODE, 1),
        "Reverse Mode",
        Defaults::REVERSE_MODE > 0.5f
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamID::FILTER_TYPE, 1),
        "Filter Type",
        juce::StringArray("LP", "HP", "BP"),
        static_cast<int>(Defaults::FILTER_TYPE)
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::FILTER_START, 1),
        "Filter Start",
        juce::NormalisableRange<float>(Ranges::FILTER_CUTOFF_MIN, Ranges::FILTER_CUTOFF_MAX, 1.0f, 0.5f),
        Defaults::FILTER_START,
        " Hz",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::FILTER_END, 1),
        "Filter End",
        juce::NormalisableRange<float>(Ranges::FILTER_CUTOFF_MIN, Ranges::FILTER_CUTOFF_MAX, 1.0f, 0.5f),
        Defaults::FILTER_END,
        " Hz",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::RANDOMIZE_AMOUNT, 1),
        "Randomize Amount",
        juce::NormalisableRange<float>(Ranges::RANDOMIZE_AMOUNT_MIN, Ranges::RANDOMIZE_AMOUNT_MAX),
        Defaults::RANDOMIZE_AMOUNT,
        "%",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamID::GATE_SHAPE, 1),
        "Gate Shape",
        juce::StringArray("Square", "Triangle", "Sine"),
        static_cast<int>(Defaults::GATE_SHAPE)
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::WET_DRY_MIX, 1),
        "Wet/Dry Mix",
        juce::NormalisableRange<float>(Ranges::WET_DRY_MIX_MIN, Ranges::WET_DRY_MIX_MAX),
        Defaults::WET_DRY_MIX,
        "%",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamID::OUTPUT_GAIN, 1),
        "Output Gain",
        juce::NormalisableRange<float>(Ranges::OUTPUT_GAIN_MIN, Ranges::OUTPUT_GAIN_MAX, 0.1f),
        Defaults::OUTPUT_GAIN,
        " dB",
        juce::AudioProcessorParameter::genericParameter
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamID::MIDI_TRIGGER_MODE, 1),
        "MIDI Trigger Mode",
        Defaults::MIDI_TRIGGER_MODE > 0.5f
    ));

    return { params.begin(), params.end() };
}

}  // namespace HVS
