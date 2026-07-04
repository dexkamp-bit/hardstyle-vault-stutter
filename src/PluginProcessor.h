#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"
#include "StutterEngine.h"

/**
 * @class HardstyleVaultStutterProcessor
 * @brief Main audio processor class for the Hardstyle Vault Stutter plugin.
 * 
 * Manages:
 * - APVTS parameter state and automation
 * - Audio processing via StutterEngine
 * - MIDI input (optional trigger)
 * - Host tempo sync
 */
class HardstyleVaultStutterProcessor : public juce::AudioProcessor {
public:
    HardstyleVaultStutterProcessor();
    ~HardstyleVaultStutterProcessor() override;

    // JUCE AudioProcessor interface
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Hardstyle Vault Stutter"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Access to APVTS for the editor
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Access to the stutter engine (for visualization, if needed)
    HVS::StutterEngine& getStutterEngine() { return stutterEngine; }

private:
    // APVTS for parameter management and automation
    juce::AudioProcessorValueTreeState apvts;

    // DSP engine
    HVS::StutterEngine stutterEngine;

    // Cache last stutter trigger state to detect edges
    bool lastStutterTrigger = false;

    // Helper to create APVTS
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HardstyleVaultStutterProcessor)
};
