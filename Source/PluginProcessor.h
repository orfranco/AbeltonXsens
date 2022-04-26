/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>
#include <C:\Users\97250\Desktop\AbeltonXsens\JUCE\Plugin\NewProject\Builds\VisualStudio2022\vcpkg\installed\x64-windows-static\include\sio_client.h>
#include <iostream>

using namespace sio;

#define PARAMS_NUM 16
#define SENSITIVITY "_sensitivity"
#define MIN_SENSITIVITY 0
#define MAX_SENSITIVITY 10
#define INVERTION "_invertion"


//==============================================================================
/**
*/
class AbletonXsensAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AbletonXsensAudioProcessor();
    ~AbletonXsensAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState treeState;
    static const struct XsensParameter params[PARAMS_NUM];

private:
    double startTime;
    std::unique_ptr<client> socketClient;
    juce::File m_log_file;
    juce::FileLogger m_logger;


    void onDataTransfer(std::string msg);

    void handleDataRows(std::istringstream& stream, std::string& currentLine, int buffer, juce::String& logMessage);

    void onReceiveMsg(event& ev);

    void sensorConnect(event& ev);

    void sensorDisconnect(event& ev);

    void changeSampleRate(int sampleRate);

    void bindSocketEvents();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbletonXsensAudioProcessor)




};

const struct XsensParameter {
    const std::string name;
    float minValue;
    float maxValue;
};