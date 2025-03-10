#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "NeuralAmpModeler.h"
#include <ff_meters.h>
#include "PresetManager/PresetManager.h"
//==============================================================================
/**
 */
class NamJUCEAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                              public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    NamJUCEAudioProcessor();
    ~NamJUCEAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources () override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor () override;
    bool hasEditor () const override;

    //==============================================================================
    const juce::String getName () const override;

    bool acceptsMidi () const override;
    bool producesMidi () const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds () const override;

    //==============================================================================
    int getNumPrograms () override;
    int getCurrentProgram () override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadNamModel (juce::File modelToLoad);
    bool getNamModelStatus ();
    void clearNAM ();

    void loadImpulseResponse (juce::File irToLoad);
    bool getIrStatus ();
    void clearIR ();

    bool getTriggerStatus ();


    const std::string getLastModelPath() { return lastModelPath; };
    const std::string getLastModelName() { return lastModelName; };
    const std::string getLastIrPath() { return lastIrPath; };
    const std::string getLastIrName() { return lastIrName; };
    const std::string getLastModelSearchDirectory() { return lastModelSerachDir; };
    const std::string getLastIrSearchDirectory() { return lastIrSerachDir; };

    foleys::LevelMeterSource& getMeterInSource() { return meterInSource; }
    foleys::LevelMeterSource& getMeterOutSource() { return meterOutSource; }

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters ();

    PresetManager& getPresetManager() { return presetManager; };

    bool eqModuleVisible{false};

    void loadFromPreset (juce::String modelPath, juce::String irPath);


private:
    //==============================================================================

    enum OutputFilters
    {
        LowCutF = 0,
        HighCutF
    };

    NeuralAmpModeler myNAM;

    juce::dsp::Convolution cab;
    bool irFound{false};
    bool irLoaded{false};

    std::atomic<float>* filterCuttofs[2];

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highCut, lowCut;

    std::string lastModelPath = "null";
    std::string lastModelName = "null";

    std::string lastIrPath = "null";
    std::string lastIrName = "null";

    std::string lastModelSerachDir = "null";
    std::string lastIrSerachDir = "null";

    bool namModelLoaded{false};



    foleys::LevelMeterSource meterInSource;
    foleys::LevelMeterSource meterOutSource;

    PresetManager presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamJUCEAudioProcessor)
};
