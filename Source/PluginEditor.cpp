/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AbletonXsensAudioProcessorEditor::AbletonXsensAudioProcessorEditor (AbletonXsensAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{}

AbletonXsensAudioProcessorEditor::~AbletonXsensAudioProcessorEditor()
{
}

//==============================================================================
void AbletonXsensAudioProcessorEditor::paint (juce::Graphics& g)
{

}

void AbletonXsensAudioProcessorEditor::resized()
{

}
