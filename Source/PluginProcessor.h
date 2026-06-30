/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#pragma once

#include <nlohmann/json.hpp>
#include "RTNeuralLSTM.h"

#if defined (__APPLE__)
 #include <Accelerate/Accelerate.h>
#endif

#include "../JuceLibraryCode/JuceHeader.h"
#include "R8BrainResampler.h"

#define GAIN_ID "drive"
#define GAIN_NAME "Drive"
#define MASTER_ID "level"
#define MASTER_NAME "Level"
#define TONE_ID "tone"
#define TONE_NAME "Tone"
#define INPUT_GAIN_ID "input_gain"
#define INPUT_GAIN_NAME "Input Gain"
#define PUSH_ID "push"
#define PUSH_NAME "Push"


//==============================================================================
class TSM1N3AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TSM1N3AudioProcessor();
    ~TSM1N3AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    //float decibelToLinear(float dbValue);

    //void setDrive(float paramDrive);
    //void setTone(float paramTone);
    //void setMaster(float db_ampMaster);

    // Pedal/amp states
    std::atomic<int> fw_state { 1 }; // 0 = off, 1 = on
    //float driveValue = 0.5;
    //float toneValue = 0.5;
    //float masterValue = 0.5;
    //float previousMasterValue = 0.5;

    bool lstm_state = true;

    RT_LSTM LSTM;
    RT_LSTM LSTM2;

    AudioProcessorValueTreeState treeState;

private:

    //float driveValue = 0.5;
    //float toneValue = 0.5;
    //float masterValue = 0.5;

    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* masterParam = nullptr;
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* pushParam = nullptr;

    float previousMasterValue = 0.5;
    float previousPushValue = -1.0f;

    chowdsp::ResampledProcess<R8BrainResampler> resampler;

    // Parametric EQ (Push Mod) centered at 200 Hz
    dsp::IIR::Filter<float> pushFilterL;
    dsp::IIR::Filter<float> pushFilterR;

    void updatePushFilters (double sampleRate);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TSM1N3AudioProcessor)
};
