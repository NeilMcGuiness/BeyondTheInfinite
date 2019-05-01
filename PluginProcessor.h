/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class BeyondTheInfiniteAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    BeyondTheInfiniteAudioProcessor();
    ~BeyondTheInfiniteAudioProcessor();

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
    void smoothParameter(float* smoothedResult, float scalar, AudioParameterFloat* targetValueParam);
    float fractionalDelay (float currentReadHead, float* delayLineToInterp); // Fractional delay based on linear interpolation

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeyondTheInfiniteAudioProcessor)
    
    AudioParameterFloat* pGain;
    AudioParameterFloat* delayInMs;
    AudioParameterFloat* feedbackGain;
    AudioParameterFloat* wetAmount;
    // I'm making the circular buffers from float* because i will only instansiate the size of the buffers later when I know the sample rate.
    float* pCircularBufferL;
    float* pCircularBufferR;
    float feedbackPathL, feedbackPathR;
    int circularBufferWriteHead, circularBufferLength;
    int delayInSamps;
    float circularBufferReadHead;
    float smoothedGain, smoothedDelTime, smoothedFBGain, smoothedDryWet;
    int sR;
};
