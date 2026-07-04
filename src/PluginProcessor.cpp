#include "PluginProcessor.h"
#include "PluginEditor.h"

HardstyleVaultStutterProcessor::HardstyleVaultStutterProcessor()
    : AudioProcessor(BusesProperties()
                      .withInput("Input", juce::AudioChannelSet::stereo(), true)
                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", HVS::createParameterLayout()) {
}

HardstyleVaultStutterProcessor::~HardstyleVaultStutterProcessor() {
}

void HardstyleVaultStutterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    stutterEngine.prepare(sampleRate, samplesPerBlock);
}

void HardstyleVaultStutterProcessor::releaseResources() {
}

void HardstyleVaultStutterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;

    // Get current host tempo if available
    if (auto* playHead = getPlayHead()) {
        if (auto tempo = playHead->getTempoAndTimeSigMetaData()) {
            stutterEngine.setHostTempo(*tempo);
        }
    }

    // Read parameter values from APVTS
    HVS::EngineParameters engineParams;
    engineParams.stutterRate = apvts.getRawParameterValue(HVS::ParamID::STUTTER_RATE)->load();
    engineParams.stutterRateSync = apvts.getRawParameterValue(HVS::ParamID::STUTTER_RATE_SYNC)->load() > 0.5f;
    engineParams.stutterLength = apvts.getRawParameterValue(HVS::ParamID::STUTTER_LENGTH)->load();
    engineParams.stutterLengthSync = apvts.getRawParameterValue(HVS::ParamID::STUTTER_LENGTH_SYNC)->load() > 0.5f;
    engineParams.repeatCount = apvts.getRawParameterValue(HVS::ParamID::REPEAT_COUNT)->load();
    engineParams.smoothness = apvts.getRawParameterValue(HVS::ParamID::SMOOTHNESS)->load();
    engineParams.pitchDrift = apvts.getRawParameterValue(HVS::ParamID::PITCH_DRIFT)->load();
    engineParams.pitchDirection = static_cast<HVS::PitchDirection>(
        static_cast<int>(apvts.getRawParameterValue(HVS::ParamID::PITCH_DIRECTION)->load() + 0.5f));
    engineParams.reverseMode = apvts.getRawParameterValue(HVS::ParamID::REVERSE_MODE)->load() > 0.5f;
    engineParams.filterType = static_cast<HVS::FilterType>(
        static_cast<int>(apvts.getRawParameterValue(HVS::ParamID::FILTER_TYPE)->load() + 0.5f));
    engineParams.filterStart = apvts.getRawParameterValue(HVS::ParamID::FILTER_START)->load();
    engineParams.filterEnd = apvts.getRawParameterValue(HVS::ParamID::FILTER_END)->load();
    engineParams.randomizeAmount = apvts.getRawParameterValue(HVS::ParamID::RANDOMIZE_AMOUNT)->load();
    engineParams.gateShape = static_cast<HVS::GateShape>(
        static_cast<int>(apvts.getRawParameterValue(HVS::ParamID::GATE_SHAPE)->load() + 0.5f));
    engineParams.wetDryMix = apvts.getRawParameterValue(HVS::ParamID::WET_DRY_MIX)->load();
    engineParams.outputGain = apvts.getRawParameterValue(HVS::ParamID::OUTPUT_GAIN)->load();

    // Handle stutter trigger automation parameter (rising edge detection)
    bool currentStutterTrigger = apvts.getRawParameterValue(HVS::ParamID::STUTTER_TRIGGER)->load() > 0.5f;
    if (currentStutterTrigger && !lastStutterTrigger) {
        stutterEngine.triggerStutter();
    } else if (!currentStutterTrigger && lastStutterTrigger) {
        stutterEngine.releaseStutter();
    }
    lastStutterTrigger = currentStutterTrigger;

    // Handle MIDI trigger if enabled
    bool midiTriggerMode = apvts.getRawParameterValue(HVS::ParamID::MIDI_TRIGGER_MODE)->load() > 0.5f;
    if (midiTriggerMode) {
        for (auto metadata : midiMessages) {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn()) {
                stutterEngine.triggerStutter();
            } else if (msg.isNoteOff()) {
                stutterEngine.releaseStutter();
            }
        }
    }

    // Process audio through the stutter engine
    stutterEngine.process(buffer, engineParams);
}

juce::AudioProcessorEditor* HardstyleVaultStutterProcessor::createEditor() {
    return new HardstyleVaultStutterEditor(*this);
}

void HardstyleVaultStutterProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyValueTreeToXml(true, true);
    copyXmlToBinary(*state, destData);
}

void HardstyleVaultStutterProcessor::setStateInformation(const void* data, int sizeInBytes) {
    auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState != nullptr) {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

// Entry point for plugin instantiation
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new HardstyleVaultStutterProcessor();
}
