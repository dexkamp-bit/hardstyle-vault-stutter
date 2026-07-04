#include "PluginEditor.h"

HardstyleVaultStutterEditor::HardstyleVaultStutterEditor(HardstyleVaultStutterProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setLookAndFeel(&customLookAndFeel);
    setSize(900, 600);
    setResizable(false, false);

    // Timing Section
    createSlider(stutterRateSlider, "Stutter Rate", 0.0f, 6.0f, 2.0f);
    createToggleButton(stutterRateSyncButton, "Sync Rate");
    createSlider(stutterLengthSlider, "Stutter Length", 10.0f, 500.0f, 100.0f);
    createToggleButton(stutterLengthSyncButton, "Sync Length");
    createSlider(repeatCountSlider, "Repeats", 1.0f, 64.0f, 4.0f);

    // Pitch Section
    createSlider(pitchDriftSlider, "Pitch Drift", -24.0f, 24.0f, 0.0f);
    createComboBox(pitchDirectionCombo, juce::StringArray("Up", "Down", "Random"));
    createToggleButton(reverseModeButton, "Reverse");

    // Filter Section
    createComboBox(filterTypeCombo, juce::StringArray("LP", "HP", "BP"));
    createSlider(filterStartSlider, "Filter Start", 20.0f, 20000.0f, 8000.0f);
    createSlider(filterEndSlider, "Filter End", 20.0f, 20000.0f, 200.0f);

    // Gate & Envelope Section
    createComboBox(gateShapeCombo, juce::StringArray("Square", "Triangle", "Sine"));
    createSlider(smoothnessSlider, "Smoothness", 0.0f, 100.0f, 50.0f);
    createSlider(randomizeAmountSlider, "Randomize", 0.0f, 100.0f, 0.0f);

    // Mix Section
    createSlider(wetDryMixSlider, "Wet/Dry", 0.0f, 100.0f, 100.0f);
    createSlider(outputGainSlider, "Output Gain", -24.0f, 24.0f, 0.0f);
    createToggleButton(midiTriggerButton, "MIDI Trigger");
    createToggleButton(stutterTriggerButton, "Trigger");
}

HardstyleVaultStutterEditor::~HardstyleVaultStutterEditor() {
    setLookAndFeel(nullptr);
}

void HardstyleVaultStutterEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(CustomLookAndFeel::DARK_BG));

    // Title
    g.setColour(juce::Colour(CustomLookAndFeel::NEON_MAGENTA));
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("HARDSTYLE VAULT STUTTER", getLocalBounds().removeFromTop(40),
               juce::Justification::centred);

    // Section labels
    g.setColour(juce::Colour(CustomLookAndFeel::NEON_CYAN));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
}

void HardstyleVaultStutterEditor::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(50);  // Title space

    // Layout in a grid
    int col1 = 20, col2 = 300, col3 = 580;
    int y = 60;
    int rowHeight = 70;

    // Timing Section (top-left)
    if (stutterRateSlider) {
        stutterRateSlider->setBounds(col1, y, 250, 50);
        stutterRateSyncButton->setBounds(col1 + 260, y, 20, 20);
    }
    y += rowHeight;

    if (stutterLengthSlider) {
        stutterLengthSlider->setBounds(col1, y, 250, 50);
        stutterLengthSyncButton->setBounds(col1 + 260, y, 20, 20);
    }
    y += rowHeight;

    if (repeatCountSlider) {
        repeatCountSlider->setBounds(col1, y, 250, 50);
    }
    y += rowHeight;

    // Reset y for middle column
    y = 60;

    // Pitch Section (middle)
    if (pitchDriftSlider) {
        pitchDriftSlider->setBounds(col2, y, 250, 50);
    }
    y += rowHeight;

    if (pitchDirectionCombo) {
        pitchDirectionCombo->setBounds(col2, y, 250, 30);
    }
    y += rowHeight;

    if (reverseModeButton) {
        reverseModeButton->setBounds(col2, y, 250, 30);
    }
    y += rowHeight;

    // Reset y for right column
    y = 60;

    // Filter Section (right)
    if (filterTypeCombo) {
        filterTypeCombo->setBounds(col3, y, 250, 30);
    }
    y += 40;

    if (filterStartSlider) {
        filterStartSlider->setBounds(col3, y, 250, 50);
    }
    y += rowHeight;

    if (filterEndSlider) {
        filterEndSlider->setBounds(col3, y, 250, 50);
    }

    // Bottom row
    y = 400;
    if (gateShapeCombo) {
        gateShapeCombo->setBounds(col1, y, 250, 30);
    }
    if (smoothnessSlider) {
        smoothnessSlider->setBounds(col2, y, 250, 50);
    }
    if (randomizeAmountSlider) {
        randomizeAmountSlider->setBounds(col3, y, 250, 50);
    }

    // Mix controls (bottom)
    y = 470;
    if (wetDryMixSlider) {
        wetDryMixSlider->setBounds(col1, y, 250, 50);
    }
    if (outputGainSlider) {
        outputGainSlider->setBounds(col2, y, 250, 50);
    }

    // Trigger buttons (very bottom)
    y = 540;
    if (midiTriggerButton) {
        midiTriggerButton->setBounds(col1, y, 250, 30);
    }
    if (stutterTriggerButton) {
        stutterTriggerButton->setBounds(col2, y, 250, 30);
    }
}

void HardstyleVaultStutterEditor::createSlider(std::unique_ptr<juce::Slider>& slider,
                                               const juce::String& name,
                                               float min, float max, float defaultValue) {
    slider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag,
                                            juce::Slider::TextBoxBelow);
    slider->setRange(min, max);
    slider->setValue(defaultValue);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider->addListener(this);
    addAndMakeVisible(*slider);
}

void HardstyleVaultStutterEditor::createToggleButton(std::unique_ptr<juce::ToggleButton>& button,
                                                     const juce::String& name) {
    button = std::make_unique<juce::ToggleButton>(name);
    button->addListener(this);
    addAndMakeVisible(*button);
}

void HardstyleVaultStutterEditor::createComboBox(std::unique_ptr<juce::ComboBox>& combo,
                                                 const juce::StringArray& items) {
    combo = std::make_unique<juce::ComboBox>();
    for (int i = 0; i < items.size(); ++i) {
        combo->addItem(items[i], i + 1);
    }
    combo->setSelectedItemIndex(0);
    combo->addListener(this);
    addAndMakeVisible(*combo);
}

void HardstyleVaultStutterEditor::sliderValueChanged(juce::Slider* slider) {
    // Handle slider changes and update APVTS
    // (Implementation would map slider to APVTS parameter)
}

void HardstyleVaultStutterEditor::buttonClicked(juce::Button* button) {
    // Handle button clicks
    // (Implementation would map button to APVTS parameter)
}

void HardstyleVaultStutterEditor::comboBoxChanged(juce::ComboBox* comboBox) {
    // Handle combo box changes
    // (Implementation would map combo box to APVTS parameter)
}
