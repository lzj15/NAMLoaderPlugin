#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"
#include "LedButtonComponent.h"

// This will need refactoring...
class AssetManager
{
public:
    AssetManager()
    {
        toggleButtonAssets[Buttons::EQ_BUTTON][ToggleButtonStates::OFF] =
            juce::ImageFileFormat::loadFrom(BinaryData::eqoff_png, BinaryData::eqoff_pngSize);
        toggleButtonAssets[Buttons::EQ_BUTTON][ToggleButtonStates::ON] =
            juce::ImageFileFormat::loadFrom(BinaryData::eqon_png, BinaryData::eqon_pngSize);
        toggleButtonAssets[Buttons::NORMALIZE_BUTTON][ToggleButtonStates::OFF] =
            juce::ImageFileFormat::loadFrom(BinaryData::normalizeoff_png, BinaryData::normalizeoff_pngSize);
        toggleButtonAssets[Buttons::NORMALIZE_BUTTON][ToggleButtonStates::ON] =
            juce::ImageFileFormat::loadFrom(BinaryData::normalizeon_png, BinaryData::normalizeon_pngSize);
        toggleButtonAssets[Buttons::TONESTACK_BUTTON][ToggleButtonStates::OFF] =
            juce::ImageFileFormat::loadFrom(BinaryData::tonestackoff_png, BinaryData::tonestackoff_pngSize);
        toggleButtonAssets[Buttons::TONESTACK_BUTTON][ToggleButtonStates::ON] =
            juce::ImageFileFormat::loadFrom(BinaryData::tonestackon_png, BinaryData::tonestackon_pngSize);
        toggleButtonAssets[Buttons::IR_BUTTON][ToggleButtonStates::OFF] =
            juce::ImageFileFormat::loadFrom(BinaryData::caboff_png, BinaryData::caboff_pngSize);
        toggleButtonAssets[Buttons::IR_BUTTON][ToggleButtonStates::ON] =
            juce::ImageFileFormat::loadFrom(BinaryData::cabon_png, BinaryData::cabon_pngSize);
    }

    ~AssetManager() {}

    juce::Image getBackground() { return background; }

    juce::Image getScreens() { return screens; }

    void setLoadButton(std::unique_ptr<juce::ImageButton>& button)
    {
        button->setImages(false, true, false, loadButtonUnpressed, 1.0, juce::Colours::transparentWhite, loadButtonUnpressed, 1.0,
                          juce::Colours::transparentWhite, loadButtonPressed, 1.0, juce::Colours::transparentWhite, 0);
    }

    void setClearButton(std::unique_ptr<juce::ImageButton>& button)
    {
        button->setImages(false, true, false, clearButtonUnpressed, 1.0, juce::Colours::transparentWhite, clearButtonUnpressed, 1.0,
                          juce::Colours::transparentWhite, clearButtonPressed, 1.0, juce::Colours::transparentWhite, 0);
    }

    void setToggleButton(std::unique_ptr<juce::ImageButton>& button, bool moduleState, int buttonIndex)
    {
        button->setImages(false, true, false, toggleButtonAssets[int(moduleState)][buttonIndex + 1], 1.0, juce::Colours::transparentWhite,
                          toggleButtonAssets[int(moduleState)][buttonIndex + 1], 1.0, juce::Colours::transparentWhite,
                          toggleButtonAssets[int(moduleState)][buttonIndex], 1.0, juce::Colours::transparentWhite, 0);
    }

    juce::Image getButtonLabel(int button, int state) { return toggleButtonAssets[button][state]; }

    void initializeButton(std::unique_ptr<LedButtonComponent>& button, int buttonIndex)
    {
        button.reset(new LedButtonComponent(toggleButtonAssets[buttonIndex][1], toggleButtonAssets[buttonIndex][0]));
    }

    enum ToggleButtonStates
    {
        OFF = 0,
        ON
    };

    enum Buttons
    {
        IR_BUTTON = 0,
        NORMALIZE_BUTTON,
        TONESTACK_BUTTON,
        EQ_BUTTON
    };

private:
    juce::Image background = juce::ImageFileFormat::loadFrom(BinaryData::background_png, BinaryData::background_pngSize);
    juce::Image screens = juce::ImageFileFormat::loadFrom(BinaryData::screens_png, BinaryData::screens_pngSize);

    // Load IR Button Assets
    juce::Image loadButtonPressed = juce::ImageFileFormat::loadFrom(BinaryData::loadButtonPushed_png, BinaryData::loadButtonPushed_pngSize);
    juce::Image loadButtonUnpressed = juce::ImageFileFormat::loadFrom(BinaryData::loadButtonUnpushed_png, BinaryData::loadButtonUnpushed_pngSize);

    juce::Image clearButtonPressed = juce::ImageFileFormat::loadFrom(BinaryData::clearButtonPressed_png, BinaryData::clearButtonPressed_pngSize);
    juce::Image clearButtonUnpressed =
        juce::ImageFileFormat::loadFrom(BinaryData::clearButtonUnpressed_png, BinaryData::clearButtonUnpressed_pngSize);

    // Toggle Buttons
    juce::Image toggleButtonAssets[4][2];
};