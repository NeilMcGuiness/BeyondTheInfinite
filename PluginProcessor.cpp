/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define MAX_DELAY_TIME 2 // 2 seconds max delay time for now.


//==============================================================================
BeyondTheInfiniteAudioProcessor::BeyondTheInfiniteAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(pGain = new AudioParameterFloat("gain",
                                                 "Gain'",
                                                 0.0f,
                                                 1.0f,
                                                 0.5f));
    
    addParameter(delayInMs = new AudioParameterFloat("delayTime",
                                                     "DelayTime'",
                                                     0.10f,
                                                     2000.f,
                                                     500.f));
    
    addParameter(feedbackGain = new AudioParameterFloat("feedback",
                                                        "Feedback'",
                                                        0.0f,
                                                        1.2f,
                                                        0.5f));
    
    addParameter(wetAmount = new AudioParameterFloat("dryWet",
                                                     "DryWet'",
                                                     0.0f,
                                                     1.0f,
                                                     0.5f));
    
    smoothedGain = pGain->get();
    smoothedDelTime = delayInMs->get();
    smoothedFBGain = feedbackGain->get();
    smoothedDryWet = wetAmount->get();
    
    // Setting my circular buffers to nullptr to begin with as I don't yet know the sample rate.
    pCircularBufferL = nullptr;
    pCircularBufferR = nullptr;
    feedbackPathL = 0;
    feedbackPathR = 0;
    circularBufferWriteHead = 0; // so that we start to write at the start of the buffer.
    circularBufferReadHead = 0;
    circularBufferLength = 0; // Hasn't yet been set, will be set in PrepareToPlay().
    delayInSamps = 0;
    sR = 0;
}

BeyondTheInfiniteAudioProcessor::~BeyondTheInfiniteAudioProcessor()
{
    // Memory management - if they are not nullptr (they have been instansiated) then we should delete them and set them back to nullptr.
    if (pCircularBufferL != nullptr)
    {
        delete [] pCircularBufferL;
        pCircularBufferL = nullptr;
    }
    
    if (pCircularBufferR != nullptr)
    {
        delete [] pCircularBufferR;
        pCircularBufferR = nullptr;
    }
}

//==============================================================================
const String BeyondTheInfiniteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BeyondTheInfiniteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BeyondTheInfiniteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BeyondTheInfiniteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BeyondTheInfiniteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BeyondTheInfiniteAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BeyondTheInfiniteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BeyondTheInfiniteAudioProcessor::setCurrentProgram (int index)
{
}

const String BeyondTheInfiniteAudioProcessor::getProgramName (int index)
{
    return {};
}

void BeyondTheInfiniteAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BeyondTheInfiniteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    sR = sampleRate;
    circularBufferWriteHead = 0;
    circularBufferLength = sR * MAX_DELAY_TIME;
    feedbackPathL = 0;
    feedbackPathR = 0;
    
    if (pCircularBufferL == nullptr) // If it hasn't been instansiated yet.
    {
        // So we create a float array to the size of the sample rate multiplied by the number of seconds in our MAX_DELAY_TIME define. we also cast this number of samples to an (int)
        pCircularBufferL = new float[circularBufferLength];
    }
    
    zeromem(pCircularBufferL, circularBufferLength * sizeof(float));
    
    if (pCircularBufferR == nullptr)
    {
        pCircularBufferR = new float[circularBufferLength];
    }
    
    zeromem(pCircularBufferR, circularBufferLength * sizeof(float));
}

void BeyondTheInfiniteAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BeyondTheInfiniteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void BeyondTheInfiniteAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    float* channelDataL = buffer.getWritePointer (0); // we create a pointer to point at the audio buffer data.
    float* channelDataR = buffer.getWritePointer (1); // we create a pointer to point at the audio buffer data.
    
    for (int sample = 0; sample < buffer.getNumSamples(); sample ++) // This loop iterates through every sample in the buffer for the channel we're currently in, decided by the outer loop.
    {
        // Using the smoothing formula: x = x - coEf * (x - y) the minuses work because if Y (target value) is begger than x, then the result of the bracketed portion will be negative, therefore you'll be subtracting a negative, resulting in a positive trend.
        // smoothedGain = smoothedGain - 0.005*(smoothedGain - pGain->get());
        
        // smoothing my parameters.
        smoothParameter(&smoothedGain, 0.005, pGain);
        smoothParameter(&smoothedDelTime, 0.001, delayInMs);
        smoothParameter(&smoothedFBGain, 0.005, feedbackGain);
        smoothParameter(&smoothedDryWet, 0.005, wetAmount);
        
        // READHEAD SETTING
        circularBufferReadHead = circularBufferWriteHead - ((smoothedDelTime / 1000) * sR); // setting the readHead back by the amount of samples required to reflect that time in ms MAYBE CALCULATE THE NUM OF SAMPLES OUTSIDE OF THIS LOOP TO OPTIMISE COMPUTATION?
        if (circularBufferReadHead < 0) // Wrap around if our calculaton goes outside of the buffer.
        {
            circularBufferReadHead += circularBufferLength;
        }
        
        // DELAY SIGNAL PROCESSING %%%%%%%%%%%%%%%%%%%%%%%%
        
        // Storing the feedback signal and the dry signal into the delay buffer FIRST.
        pCircularBufferL[circularBufferWriteHead] = feedbackPathL + channelDataL[sample]; // Here we writing the (possibly attenuated) samples of audio held in the Channel Data arrays into the circular buffers.
        pCircularBufferR[circularBufferWriteHead] = feedbackPathR + channelDataR[sample];
        
        // Feeding the dry signal and delayed signal into the feedback path.
        feedbackPathL = (channelDataL[sample] * smoothedGain) + fractionalDelay(circularBufferReadHead, pCircularBufferL) * smoothedFBGain;
        feedbackPathR = (channelDataR[sample] * smoothedGain) + fractionalDelay(circularBufferReadHead, pCircularBufferR) * smoothedFBGain;
        
        // Attenuating the dry signal.
        channelDataL[sample] *= smoothedGain; // We don't need to dereference the pointer with *channelData, because the [sample] syntax dereferences it for us. Essentially ChannelData as a variable is a pointer to the whole buffer of data in the channel.
        channelDataR[sample] *= smoothedGain;
        
        
        // FINAL OUTPUT -- Outputting a weighed balance of dry and wet signal.
        channelDataL[sample] = (channelDataL[sample] * (1 - smoothedDryWet)) + (fractionalDelay(circularBufferReadHead, pCircularBufferL) * smoothedDryWet);
        channelDataR[sample] = (channelDataR[sample] * (1 - smoothedDryWet)) + (fractionalDelay(circularBufferReadHead, pCircularBufferR) * smoothedDryWet);
        
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        circularBufferWriteHead ++; // increment the writeHead.
        
        if(circularBufferWriteHead >= circularBufferLength) // wrap around if we're at the end of the circular buffer.
        {
            circularBufferWriteHead = 0;
        }
    }
}

//==============================================================================
bool BeyondTheInfiniteAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BeyondTheInfiniteAudioProcessor::createEditor()
{
    return new BeyondTheInfiniteAudioProcessorEditor (*this);
}

//==============================================================================
void BeyondTheInfiniteAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BeyondTheInfiniteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// Simple encapsulating of the parameter smoothing process.
void BeyondTheInfiniteAudioProcessor::smoothParameter(float* smoothedResult, float scalar, AudioParameterFloat*targetValueParam)
{
    *smoothedResult = *smoothedResult - scalar*(*smoothedResult - targetValueParam->get());
}


float BeyondTheInfiniteAudioProcessor::fractionalDelay (float currentReadHead, float* delayLineToInterp)
{
    int readHeadInt, readHeadIntNext;
    float sampleLast, sampleNext;
    float timeFraction, sampleDifference;
    
    readHeadInt = (int)currentReadHead;  // casting the readhead to an int to get the previous integer spot in the buffer.
    readHeadIntNext = readHeadInt + 1; // Getting next position in the buffer
    if(readHeadIntNext == circularBufferLength) // If the previous integer spot was the last in the array then the 'next' must be the first spot in the array.
    {
        readHeadIntNext = 0;
    }
    
    timeFraction = currentReadHead - readHeadInt; // subtracting the integer value from the decimal to get only the fractional portion (along the x axis).
    sampleLast = delayLineToInterp[readHeadInt]; // Sample at the previous integer spot
    sampleNext = delayLineToInterp[readHeadIntNext];// Sample at the next integer spot
    sampleDifference = sampleNext - sampleLast; // The difference between the two...
    
    return sampleLast + (sampleDifference * timeFraction); // So we return the fractional delay of the sample of the previous integer spot + the correct fractional amount of the difference between the previous integer's sample and the next integer's sample.
}
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BeyondTheInfiniteAudioProcessor();
}
