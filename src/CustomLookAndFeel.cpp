#include "CustomLookAndFeel.h"
#include <cmath>

CustomLookAndFeel::CustomLookAndFeel() {
    // Set dark theme colors
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(DARK_BG));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(NEON_MAGENTA));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(DARK_PANEL));
    setColour(juce::Slider::thumbColourId, juce::Colour(NEON_CYAN));
    setColour(juce::TextButton::buttonColourId, juce::Colour(DARK_PANEL));
    setColour(juce::TextButton::textColourOffId, juce::Colour(TEXT_COLOR));
    setColour(juce::TextButton::textColourOnId, juce::Colour(NEON_MAGENTA));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(DARK_PANEL));
    setColour(juce::ComboBox::textColourId, juce::Colour(TEXT_COLOR));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(NEON_MAGENTA));
    setColour(juce::Label::textColourId, juce::Colour(TEXT_COLOR));
    setColour(juce::Label::outlineColourId, juce::Colour(DARK_BG));
}

CustomLookAndFeel::~CustomLookAndFeel() {
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider& slider) {
    auto radius = (float)std::min(width / 2, height / 2) - 2.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Draw dark background circle
    g.setColour(juce::Colour(DARK_PANEL));
    g.fillEllipse(rx, ry, rw, rw);

    // Draw neon outline
    g.setColour(juce::Colour(NEON_MAGENTA));
    g.drawEllipse(rx, ry, rw, rw, 2.0f);

    // Draw rotating indicator line (neon cyan)
    juce::Path p;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = 2.5f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle)
                     .translated(centreX, centreY));

    g.setColour(juce::Colour(NEON_CYAN));
    g.fillPath(p);

    // Draw center dot (acid green)
    g.setColour(juce::Colour(ACID_GREEN));
    g.fillEllipse(centreX - 5.0f, centreY - 5.0f, 10.0f, 10.0f);
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                             const juce::Colour& backgroundColour,
                                             bool isMouseOverButton, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds();

    // Draw background
    g.setColour(backgroundColour);
    g.fillRect(buttonArea);

    // Draw border
    if (isMouseOverButton) {
        g.setColour(juce::Colour(NEON_CYAN));
        g.drawRect(buttonArea, 2.0f);
    } else {
        g.setColour(juce::Colour(NEON_MAGENTA));
        g.drawRect(buttonArea, 1.5f);
    }

    if (isButtonDown) {
        g.setColour(juce::Colour(ACID_GREEN).withAlpha(0.3f));
        g.fillRect(buttonArea);
    }
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                       bool isMouseOverButton, bool isButtonDown) {
    g.setColour(isButtonDown ? juce::Colour(ACID_GREEN) : juce::Colour(TEXT_COLOR));
    g.setFont(juce::Font(14.0f, juce::Font::bold));

    g.drawFittedText(button.getButtonText(),
                     button.getLocalBounds(),
                     juce::Justification::centred,
                     1);
}

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool isMouseOverButton, bool isButtonDown) {
    auto tickBox = button.getLocalBounds();
    tickBox.setSize(20, 20);

    // Draw box background
    g.setColour(juce::Colour(DARK_PANEL));
    g.fillRect(tickBox);

    // Draw border
    if (button.getToggleState()) {
        g.setColour(juce::Colour(ACID_GREEN));
        g.drawRect(tickBox, 2.0f);
    } else {
        g.setColour(juce::Colour(NEON_MAGENTA));
        g.drawRect(tickBox, 1.5f);
    }

    // Draw checkmark if toggled
    if (button.getToggleState()) {
        g.setColour(juce::Colour(ACID_GREEN));
        juce::Path checkPath;
        checkPath.startNewSubPath(tickBox.getX() + 5.0f, tickBox.getCentreY());
        checkPath.lineTo(tickBox.getX() + 10.0f, tickBox.getBottom() - 5.0f);
        checkPath.lineTo(tickBox.getRight() - 4.0f, tickBox.getY() + 4.0f);
        g.strokePath(checkPath, juce::PathStrokeType(2.0f));
    }

    // Draw label
    g.setColour(juce::Colour(TEXT_COLOR));
    g.setFont(juce::Font(12.0f));
    g.drawText(button.getButtonText(),
               tickBox.getRight() + 8, tickBox.getY(),
               button.getWidth() - tickBox.getWidth() - 12, tickBox.getHeight(),
               juce::Justification::centredLeft, true);
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                     bool isButtonDown, int buttonX, int buttonY,
                                     int buttonW, int buttonH, juce::ComboBox& box) {
    auto boxArea = juce::Rectangle<int>(0, 0, width, height);

    g.setColour(juce::Colour(DARK_PANEL));
    g.fillRect(boxArea);

    g.setColour(juce::Colour(isButtonDown ? NEON_CYAN : NEON_MAGENTA));
    g.drawRect(boxArea, 1.5f);

    // Draw dropdown button
    juce::Path dropdownArrow;
    auto arrowZone = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
    float arrowCentreX = (float)(arrowZone.getCentreX());
    float arrowCentreY = (float)(arrowZone.getCentreY());
    float arrowSize = 4.5f;

    dropdownArrow.startNewSubPath(arrowCentreX - arrowSize, arrowCentreY - arrowSize * 0.5f);
    dropdownArrow.lineTo(arrowCentreX + arrowSize, arrowCentreY - arrowSize * 0.5f);
    dropdownArrow.lineTo(arrowCentreX, arrowCentreY + arrowSize);
    dropdownArrow.closeSubPath();

    g.setColour(juce::Colour(ACID_GREEN));
    g.fillPath(dropdownArrow);
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(juce::Font(11.0f));

    auto textArea = label.getLocalBounds().reduced(2);
    g.drawText(label.getText(),
               textArea,
               label.getJustificationType(),
               true);
}
