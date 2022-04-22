/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
// TODO: realize how to import in more style:
#include <C:\Users\97250\Desktop\AbeltonXsens\JUCE\Plugin\NewProject\Builds\VisualStudio2022\vcpkg\installed\x64-windows-static\include\sio_client.h>
#include <iostream>
using namespace sio;
//==============================================================================
/**
*/
class AbletonXsensAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          public juce::Slider::Listener

{
public:
    AbletonXsensAudioProcessorEditor (AbletonXsensAudioProcessor&);
    ~AbletonXsensAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    //TODO: implement a currently-connected sensors hashmap.
    AbletonXsensAudioProcessor& audioProcessor;
    std::unique_ptr<client> socketClient;
    juce::Slider gainSlider;
    // gain slider attachment should be deleted before gainSlider and treeState!
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    void onReceiveMsg(event& ev);
    void onDataTransfer(std::string msg);
    void sensorConnect(event& ev);
    void sensorDisconnect(event& ev);
    void changeSampleRate(int sampleRate);
    void bindSocketEvents();
    juce::File m_log_file;
    juce::FileLogger m_logger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbletonXsensAudioProcessorEditor)
};
