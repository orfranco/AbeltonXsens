/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
// TODO: realize how to import in more style:

using namespace sio;

//==============================================================================
/**
*/
class AbletonXsensAudioProcessorEditor  : public juce::AudioProcessorEditor                                 
{
public:
    AbletonXsensAudioProcessorEditor (AbletonXsensAudioProcessor&);
    ~AbletonXsensAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AbletonXsensAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbletonXsensAudioProcessorEditor)
};
