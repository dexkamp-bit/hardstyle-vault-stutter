#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * @class CustomLookAndFeel
 * @brief Dark/neon hardstyle aesthetic for rotary knobs and UI elements.
 * 
 * Features:
 * - Black/dark grey backgrounds
 * - Neon magenta, cyan, or acid green accents
 * - Custom rotary knob appearance
 * - Smooth, energetic rave-inspired design
 */
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    // Rotary slider appearance
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    // Button appearance
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool isMouseOverButton, bool isButtonDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool isMouseOverButton, bool isButtonDown) override;

    // Toggle button appearance
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool isMouseOverButton, bool isButtonDown) override;

    // Combo box appearance
    void drawComboBox(juce::Graphics& g, int width, int height,
                      bool isButtonDown, int buttonX, int buttonY,
                      int buttonW, int buttonH, juce::ComboBox& box) override;

    // Label appearance
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    // Colour scheme
    static constexpr juce::uint32 DARK_BG = 0xFF0a0a0a;
    static constexpr juce::uint32 DARK_PANEL = 0xFF1a1a1a;
    static constexpr juce::uint32 NEON_MAGENTA = 0xFFff00ff;
    static constexpr juce::uint32 NEON_CYAN = 0xFF00ffff;
    static constexpr juce::uint32 ACID_GREEN = 0xFFccff00;
    static constexpr juce::uint32 TEXT_COLOR = 0xFFe0e0e0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
