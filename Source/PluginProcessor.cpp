/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <fstream>

//==============================================================================
TSM1N3AudioProcessor::TSM1N3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    ),    
    treeState(*this, nullptr, "PARAMETER", { std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f),
                        std::make_unique<AudioParameterFloat>(TONE_ID, TONE_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f),
                        std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5),
                        std::make_unique<AudioParameterChoice>(INPUT_GAIN_ID, INPUT_GAIN_NAME, StringArray { "LO", "MID", "HI" }, 1) })

#endif
{
    driveParam = treeState.getRawParameterValue (GAIN_ID);
    masterParam = treeState.getRawParameterValue (MASTER_ID);
    toneParam = treeState.getRawParameterValue (TONE_ID);
    inputGainParam = treeState.getRawParameterValue (INPUT_GAIN_ID);
}


TSM1N3AudioProcessor::~TSM1N3AudioProcessor()
{
}

//==============================================================================
const String TSM1N3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TSM1N3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TSM1N3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TSM1N3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TSM1N3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TSM1N3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TSM1N3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void TSM1N3AudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String TSM1N3AudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void TSM1N3AudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void TSM1N3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    LSTM.reset();
    LSTM2.reset();

    // Sanitize sample rate to prevent division by zero or NaN ratios during DAW initialization
    double safeSampleRate = sampleRate;
    if (safeSampleRate <= 0.0)
        safeSampleRate = 48000.0;

    // Enforce a minimum block size of 4096 to prevent buffer under-allocation
    // and heap corruption if the DAW initializes with a tiny/zero block size.
    int safeBlockSize = std::max (samplesPerBlock, 4096);

    // prepare resampler for target sample rate: 48 kHz
    constexpr double targetSampleRate = 48000.0;
    resampler.prepareWithTargetSampleRate ({ safeSampleRate, (uint32) safeBlockSize, 2 }, targetSampleRate);

    // Report total resampler latency to the host DAW so it can align tracks
    setLatencySamples (resampler.getLatencySamples());

    // load 48 kHz sample rate model from MessagePack binary format using custom allocator
    AllocationTracker::reset();

    const auto* msgpackData = reinterpret_cast<const uint8_t*> (BinaryData::model_ts9_48k_cond2_msgpack);
    TS_JSON weights_json = TS_JSON::from_msgpack (msgpackData, msgpackData + BinaryData::model_ts9_48k_cond2_msgpackSize);

#if JUCE_DEBUG
    size_t activeAllocs = AllocationTracker::activeAllocations.load();
    size_t allocatedBytes = AllocationTracker::totalAllocatedBytes.load();
    DBG ("[ramer-ts-m1n3] Neural model parsed: " << activeAllocs << " active allocations, " << allocatedBytes << " bytes allocated via MyJSONAllocator.");
#endif

    bool success1 = LSTM.load_json3 (weights_json);
    bool success2 = LSTM2.load_json3 (weights_json);
    if (!success1 || !success2)
    {
        DBG ("ERROR: Failed to load and validate LSTM models!");
        jassertfalse;
    }
}

void TSM1N3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TSM1N3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void TSM1N3AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    ScopedNoDenormals noDenormals;

    // Setup Audio Data

    auto driveValue = static_cast<float> (driveParam->load());
    auto toneValue = static_cast<float> (toneParam->load());
    auto masterValue = static_cast<float> (masterParam->load());

    // Amp =============================================================================
    if (fw_state == 1) {
        // Apply selected pre-gain factor: LO (1.5), MID (3.0), HI (4.5)
        float inputGainFactor = 3.0f;
        if (inputGainParam != nullptr)
        {
            auto inputGainSelection = static_cast<int> (inputGainParam->load());
            if (inputGainSelection == 0)
                inputGainFactor = 1.5f;
            else if (inputGainSelection == 1)
                inputGainFactor = 3.0f;
            else if (inputGainSelection == 2)
                inputGainFactor = 4.5f;
        }
        buffer.applyGain (inputGainFactor);
		
        // resample to target sample rate
        dsp::AudioBlock<float> block(buffer);
        //auto block = dsp::AudioBlock<float>(buffer.getArrayOfWritePointers(), 1, numSamples);
        auto block48k = resampler.processIn(block);

        // Apply LSTM model
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            // Apply LSTM model
            if (ch == 0) {
                LSTM.process(block48k.getWritePointer(0), driveValue, toneValue, block48k.getWritePointer(0), (int) block48k.getNumSamples());
            }
            else if (ch == 1) {
                LSTM2.process(block48k.getWritePointer(1), driveValue, toneValue, block48k.getWritePointer(1), (int) block48k.getNumSamples());
            }
        }

        // resample back to original sample rate
        resampler.processOut(block48k, block);

        // Master Volume 
        // Apply ramped changes for gain smoothing
        if (masterValue == previousMasterValue)
        {
            buffer.applyGain(masterValue);
        }
        else {
            buffer.applyGainRamp(0, (int) buffer.getNumSamples(), previousMasterValue, masterValue);
            previousMasterValue = masterValue;
        }
    }
}

//==============================================================================
bool TSM1N3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TSM1N3AudioProcessor::createEditor()
{
    return new TSM1N3AudioProcessorEditor (*this);
}

//==============================================================================
void TSM1N3AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    xml->setAttribute ("fw_state", fw_state);
    copyXmlToBinary (*xml, destData);
}

void TSM1N3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (treeState.state.getType()))
        {
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
            fw_state = xmlState->getBoolAttribute ("fw_state");
            if (auto* editor = dynamic_cast<TSM1N3AudioProcessorEditor*> (getActiveEditor()))
                editor->resetImages();
        }
    }
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TSM1N3AudioProcessor();
}
