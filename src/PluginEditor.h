#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

/**
 * @class HardstyleVaultStutterEditor
 * @brief GUI editor for the Hardstyle Vault Stutter plugin.
 * 
 * Features:
 * - Organized parameter sections (Timing, Pitch, Filter, Gate, Mix)
 * - Rotary knobs for smooth parameter manipulation
 * - Toggle buttons for on/off parameters
 * - Combo boxes for choice parameters (rate, direction, etc.)
 * - Dark/neon hardstyle aesthetic via CustomLookAndFeel
 */
class HardstyleVaultStutterEditor : public juce::AudioProcessorEditor,
                                     private juce::Slider::Listener,
                                     private juce::Button::Listener,
                                     private juce::ComboBox::Listener {
public:
    explicit HardstyleVaultStutterEditor(HardstyleVaultStutterProcessor&);
    ~HardstyleVaultStutterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    HardstyleVaultStutterProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;

    // Timing Section
    std::unique_ptr<juce::Slider> stutterRateSlider;
    std::unique_ptr<juce::ToggleButton> stutterRateSyncButton;
    std::unique_ptr<juce::Slider> stutterLengthSlider;
    std::unique_ptr<juce::ToggleButton> stutterLengthSyncButton;
    std::unique_ptr<juce::Slider> repeatCountSlider;

    // Pitch Section
    std::unique_ptr<juce::Slider> pitchDriftSlider;
    std::unique_ptr<juce::ComboBox> pitchDirectionCombo;
    std::unique_ptr<juce::ToggleButton> reverseModeButton;

    // Filter Section
    std::unique_ptr<juce::ComboBox> filterTypeCombo;
    std::unique_ptr<juce::Slider> filterStartSlider;
    std::unique_ptr<juce::Slider> filterEndSlider;

    // Gate & Envelope Section
    std::unique_ptr<juce::ComboBox> gateShapeCombo;
    std::unique_ptr<juce::Slider> smoothnessSlider;
    std::unique_ptr<juce::Slider> randomizeAmountSlider;

    // Mix Section
    std::unique_ptr<juce::Slider> wetDryMixSlider;
    std::unique_ptr<juce::Slider> outputGainSlider;
    std::unique_ptr<juce::ToggleButton> midiTriggerButton;
    std::unique_ptr<juce::ToggleButton> stutterTriggerButton;

    // Helper methods
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;

    void createSlider(std::unique_ptr<juce::Slider>& slider, const juce::String& name,
                      float min, float max, float defaultValue);
    void createToggleButton(std::unique_ptr<juce::ToggleButton>& button, const juce::String& name);
    void createComboBox(std::unique_ptr<juce::ComboBox>& combo, const juce::StringArray& items);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HardstyleVaultStutterEditor)
};
