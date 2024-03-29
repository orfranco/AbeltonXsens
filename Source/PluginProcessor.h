/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.
    
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>
#include "../Builds/VisualStudio2022/XsenSocket.h"
#include "../Builds/VisualStudio2022/StreamAllocator.h"

#define PARAMS_NUM 3
#define SENSITIVITY "_sensitivity"
#define MIN_SENSITIVITY 1
#define MAX_SENSITIVITY 3
#define INVERTION "_invertion"
#define SHIFTING "_shifting"
#define MIN_SHIFTING -3.14
#define MAX_SHIFTING 3.14
#define MAX_SENSORS_NUM 4

//==============================================================================
/**
*/
class AbletonXsensAudioProcessor : public juce::AudioProcessor
{
public:
    juce::AudioProcessorValueTreeState treeState;
    std::unique_ptr<XsenSocket> XsensClient;
    static const struct XsensParameter params[PARAMS_NUM];


    //==============================================================================
    AbletonXsensAudioProcessor();
    ~AbletonXsensAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void onDataTransfer(std::string msg);
    int extractSlot(std::string firstLine);
    void extractSensorData(std::istringstream& stream, std::string& currentLine, int buffer, int currSlot);

private:
    StreamAllocator streamAllocator;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    float AbletonXsensAudioProcessor::mapSensorValues(std::string currParam, double paramValue);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbletonXsensAudioProcessor)

};

const struct XsensParameter {
    std::string name;
    float minValue;
    float maxValue;
};