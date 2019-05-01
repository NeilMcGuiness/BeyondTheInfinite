/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BeyondTheInfiniteAudioProcessorEditor::BeyondTheInfiniteAudioProcessorEditor (BeyondTheInfiniteAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), gainControlLabel("gain", "Input Gain"), delayTimeLabel("delayTime","Delay Time"), feedbackLabel("feedback","Feedback"), dryWetLabel("drywetBalance","Dry/Wet") // here the reference to the processor that created this editor is assigned to our local variable (of type MyAudioPluginAudioProcessor&)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    this->setResizable(true, true);
    
    auto& localParamsRef = processor.getParameters(); // Getting access to Processor's params so that we can set appropriate ranges here.
    AudioParameterFloat* gainParam = (AudioParameterFloat*) localParamsRef.getUnchecked(0);
    AudioParameterFloat* delayTimeParam = (AudioParameterFloat*) localParamsRef.getUnchecked(1);
    AudioParameterFloat* feedBackParam = (AudioParameterFloat*) localParamsRef.getUnchecked(2);
    AudioParameterFloat* dryWetParam = (AudioParameterFloat*) localParamsRef.getUnchecked(3);
    
    addAndMakeVisible(&gainControlSlider);
    gainControlSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    gainControlSlider.setRange(gainParam->range.start , gainParam->range.end);
    gainControlSlider.setValue(*gainParam);
    gainControlSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    gainControlSlider.addListener(this); // Because we inheretied the functionality of the class - Slider::listener to our main PLuginEditor class, 'this' main class that we're inside can be the listener to the Slider nested inside it.
    
    addAndMakeVisible(&delayTimeSlider);
    delayTimeSlider.setSliderStyle(Slider::Rotary);
    delayTimeSlider.setRange(delayTimeParam->range.start , delayTimeParam->range.end, 10.f); // 3rd argument is interval of slider, I've set here to 10 ms.
    delayTimeSlider.setValue(*delayTimeParam);
    delayTimeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, delayTimeSlider.getWidth() / 3, delayTimeSlider.getHeight() / 5);
    delayTimeSlider.addListener(this);
    
    addAndMakeVisible(&feedbackSlider);
    feedbackSlider.setSliderStyle(Slider::Rotary);
    feedbackSlider.setRange(feedBackParam->range.start , feedBackParam->range.end);
    feedbackSlider.setValue(*feedBackParam);
    feedbackSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    feedbackSlider.addListener(this);
    
    addAndMakeVisible(&dryWetSlider);
    dryWetSlider.setSliderStyle(Slider::Rotary);
    dryWetSlider.setRange(dryWetParam->range.start , dryWetParam->range.end);
    dryWetSlider.setValue(*dryWetParam);
    dryWetSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    dryWetSlider.addListener(this);
    
    // Don't need to set text here because it's done in this main component constructors arguments.
    gainControlLabel.attachToComponent(&gainControlSlider, false);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    dryWetLabel.attachToComponent(&dryWetSlider, false);
}

BeyondTheInfiniteAudioProcessorEditor::~BeyondTheInfiniteAudioProcessorEditor()
{
}

//==============================================================================
void BeyondTheInfiniteAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::rebeccapurple);
    g.setFont (25.0f);
    g.drawFittedText ("Beyond The Infinite", getLocalBounds(), Justification::centredTop, 1);
}

void BeyondTheInfiniteAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    
    // Laying out components based on a rectangle of the mother component.
    
    Rectangle<int> componentBounds = getLocalBounds();
    Rectangle<int> topHalf = componentBounds.removeFromTop(getHeight() / 2);
    
    topHalf.removeFromTop(60);
    gainControlSlider.setBounds(topHalf.removeFromLeft(topHalf.getWidth() / 2));
    delayTimeSlider.setBounds(topHalf); // Don't need to remove anythng now, the remainder of 'topHalf' is what I want my bounds to be for this component
    componentBounds.removeFromTop(40);
    feedbackSlider.setBounds(componentBounds.removeFromLeft(getWidth() / 2));
    dryWetSlider.setBounds(componentBounds.removeFromRight(getWidth() / 2));
}

void BeyondTheInfiniteAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    DBG("Slider value changed");
    auto& localParamsRef = processor.getParameters(); // auto here creates a variable that matches wht is returned by the following statement, in this case - processor.getParameters returns a REFERENCE to the AudioProcessor's OwnedArray of AudioProcessorParameters.
    if(slider == &gainControlSlider) // If the slider that has just been changed is our gain slider...
    {
        // If we want to adjust the gain parameter from inside the Processor we have to make a pointer to that object - here we CAST with: (type) objectToCast : and in this case we cast the first AudioParameter in the owned array that we have a reference to in localParamsRef[] to our AudioParameterFloat* type.
        AudioParameterFloat* gainParam = (AudioParameterFloat*) localParamsRef.getUnchecked(0);
        *gainParam = gainControlSlider.getValue(); // now we can dereference that pointer to change the value of it directly.
        
    }
    else if (slider == &delayTimeSlider)
    {
        AudioParameterFloat* delayTimeParam = (AudioParameterFloat*) localParamsRef.getUnchecked(1);
        *delayTimeParam = delayTimeSlider.getValue();
        
    }
    
    else if (slider == &feedbackSlider)
    {
        AudioParameterFloat* feedBackParam = (AudioParameterFloat*) localParamsRef.getUnchecked(2);
        *feedBackParam = feedbackSlider.getValue();
        
    }
    
    else if (slider == &dryWetSlider)
    {
        AudioParameterFloat* dryWetParam = (AudioParameterFloat*) localParamsRef.getUnchecked(3);
        *dryWetParam = dryWetSlider.getValue();
        
    }
}

