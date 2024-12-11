#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NAMLoaderPluginAudioProcessor::NAMLoaderPluginAudioProcessor()
    :   apvts(*this, nullptr, "PARAMETERS", createParameters()),
        namProcessor(std::make_unique<nam_processor::NAMProcessor>()),
        irProcessor(std::make_unique<dsp::impulse_response::ImpulseResponse>()),
        stateManager(std::make_unique<state_manager::StateManager>(*this, apvts))
{
    apvts.addParameterListener("IR_BYPASS", this);
}

NAMLoaderPluginAudioProcessor::~NAMLoaderPluginAudioProcessor()
{
    irProcessor->clear();
}

//==============================================================================
const juce::String NAMLoaderPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NAMLoaderPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NAMLoaderPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NAMLoaderPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NAMLoaderPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NAMLoaderPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int NAMLoaderPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NAMLoaderPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String NAMLoaderPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void NAMLoaderPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}


juce::AudioProcessorValueTreeState::ParameterLayout NAMLoaderPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    nam_processor::NAMProcessor::createParameters(parameters);
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("IR_BYPASS", "IR Bypass", false));
    return {parameters.begin(), parameters.end()};
}

void NAMLoaderPluginAudioProcessor::loadNAMModel(const juce::File &filePath)
{
    const std::string model_path = filePath.getFullPathName().toStdString();
    namModelLoaded = namProcessor->loadModel(model_path);
}

void NAMLoaderPluginAudioProcessor::clearNAM()
{
    namProcessor->clearModel();
    namModelLoaded = false;
}

void NAMLoaderPluginAudioProcessor::clearImpulseResponse()
{
    irProcessor->clear();
    irLoaded = false;
}

void NAMLoaderPluginAudioProcessor::loadImpulseResponse(const juce::File& file)
{
    irProcessor->setImpulseResponse(file);
    irLoaded = true;
}

void NAMLoaderPluginAudioProcessor::parameterChanged(const juce::String &parameterID, const float newValue)
{
    if (parameterID == "IR_BYPASS")
    {
        irBypass = (newValue > 0.5f);
    }
}


//==============================================================================
void NAMLoaderPluginAudioProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{};
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;
    this->mSampleRate = sampleRate;
    this->mSamplesPerBlock = samplesPerBlock;

    namProcessor->prepareToPlay(spec);
    namProcessor->hookParameters(apvts);
    namProcessor->clearModel();
    namModelLoaded = false;

    irProcessor->prepare(spec);

    if (storedState.isValid())
    {
        restoreNAMModel(storedState);
        storedState = juce::ValueTree();
    }

    rmsOutput.reset(sampleRate, 0.5);
    rmsOutput.setCurrentAndTargetValue(0.0f);

    rmsInput.reset(sampleRate, 0.5);
    rmsInput.setCurrentAndTargetValue(0.0f);
}

void NAMLoaderPluginAudioProcessor::releaseResources()
{
}

bool NAMLoaderPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void NAMLoaderPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const float rmsInputLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rmsInput.setCurrentAndTargetValue(rmsInputLevel);

    namProcessor->process(buffer);

    if (!irBypass && irLoaded)
    {
        irProcessor->process(buffer);
    }

    const float rmsLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rmsOutput.setCurrentAndTargetValue(rmsLevel);
}

//==============================================================================
bool NAMLoaderPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* NAMLoaderPluginAudioProcessor::createEditor()
{
    return new NAMLoaderPluginAudioProcessorEditor(*this);
}

//==============================================================================
void NAMLoaderPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    stateManager->getStateInformation(destData);
}

void NAMLoaderPluginAudioProcessor::setStateInformation(const void* data, const int sizeInBytes)
{
    stateManager->setStateInformation(data, sizeInBytes);
    const auto state = stateManager->getAPVTS().copyState();
    if (const std::unique_ptr<juce::XmlElement> xml = state.createXml(); xml == nullptr)
    {
        throw std::runtime_error("Failed to create XML from state.");
    }

    restoreImpulseResponse(state);
    storedState = state;
}


void NAMLoaderPluginAudioProcessor::restoreImpulseResponse(const juce::ValueTree& state)
{
    const auto impulseResponseFilePath = state.getProperty("IMPULSE_RESPONSE_FILE_PATH").toString();
    if (impulseResponseFilePath.isEmpty()) {
        return;
    }

    const juce::File impulseResponseFile(impulseResponseFilePath);
    if (!impulseResponseFile.existsAsFile()) {
        throw std::runtime_error("Impulse Response file does not exist: " + impulseResponseFilePath.toStdString());
    }

    loadImpulseResponse(impulseResponseFile);

    juce::MessageManager::callAsync([this, impulseResponseFile] {
        if (auto* editor = dynamic_cast<NAMLoaderPluginAudioProcessorEditor*>(getActiveEditor())) {
            editor->setCurrentIRFile(impulseResponseFile);
            editor->updateIRButtonText(impulseResponseFile.getFileName());
        }
    });
}

void NAMLoaderPluginAudioProcessor::restoreNAMModel(const juce::ValueTree& state)
{
    const auto namModelFilePath = state.getProperty("NAM_MODEL_FILE_PATH").toString();
    if (namModelFilePath.isEmpty()) {
        return;
    }

    const juce::File namModelFile(namModelFilePath);
    if (!namModelFile.existsAsFile()) {
        throw std::runtime_error("NAM Model file does not exist: " + namModelFilePath.toStdString());
    }

    loadNAMModel(namModelFile);

    juce::MessageManager::callAsync([this, namModelFile] {
        if (auto* editor = dynamic_cast<NAMLoaderPluginAudioProcessorEditor*>(getActiveEditor())) {
            editor->setCurrentModelFile(namModelFile);
            editor->updateModelButtonText(namModelFile.getFileName());
        }
    });

}

float NAMLoaderPluginAudioProcessor::getRMSOutputValue() const {return rmsOutput.getCurrentValue();}
float NAMLoaderPluginAudioProcessor::getRMSInputValue() const {return rmsInput.getCurrentValue();}


//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NAMLoaderPluginAudioProcessor();
}
