/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BeyondTheInfiniteAudioProcessorEditor  : public AudioProcessorEditor,
                                               public Slider::Listener
{
public:
    BeyondTheInfiniteAudioProcessorEditor (BeyondTheInfiniteAudioProcessor&);
    ~BeyondTheInfiniteAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider* slider) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BeyondTheInfiniteAudioProcessor& processor;
    Slider gainControlSlider, delayTimeSlider, feedbackSlider, dryWetSlider;
    Label gainControlLabel, delayTimeLabel, feedbackLabel, dryWetLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeyondTheInfiniteAudioProcessorEditor)
};
