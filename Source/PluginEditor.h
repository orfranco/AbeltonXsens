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

/**
    XsensSlider is a struct that contains all the relevant data
    for a slider in the plugin.
    - sensitivity: only positive values. represents the sensitivity of the slider.
    - invertion: 1 or -1. represents the direction of the slider.
*/
struct XsensSlider {
    juce::Slider slider;
    float sensitivity = 1;
    int invertion = 1;
};
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
    std::unique_ptr<client> socketClient;
    std::map<std::string, std::unique_ptr<XsensSlider>> XsensSliders;
    // gain slider attachment should be deleted before gainSlider and treeState!
    std::map < std::string, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> gainSliderAttachments;
    void onReceiveMsg(event& ev);
    void onDataTransfer(std::string msg);
    void handleDataRows(std::istringstream& stream, std::string& currentLine, int buffer, juce::String& logMessage);
    void sensorConnect(event& ev);
    void sensorDisconnect(event& ev);
    void changeSampleRate(int sampleRate);
    void bindSocketEvents();
    juce::File m_log_file;
    juce::FileLogger m_logger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbletonXsensAudioProcessorEditor)
};
