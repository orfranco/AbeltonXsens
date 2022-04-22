/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AbletonXsensAudioProcessor::AbletonXsensAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), treeState(*this, nullptr, "PARAMETER", createParameters()),
        startTime(juce::Time::getMillisecondCounterHiRes() * 0.001)
#endif
    
{
}

AbletonXsensAudioProcessor::~AbletonXsensAudioProcessor()
{
}

//==============================================================================
const juce::String AbletonXsensAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AbletonXsensAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AbletonXsensAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AbletonXsensAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AbletonXsensAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AbletonXsensAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AbletonXsensAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AbletonXsensAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AbletonXsensAudioProcessor::getProgramName (int index)
{
    return {};
}

void AbletonXsensAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AbletonXsensAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AbletonXsensAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AbletonXsensAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void AbletonXsensAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
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

    
    //auto sliderGainValue = treeState.getRawParameterValue(GAIN_ID);
    //auto message = juce::MidiMessage::controllerEvent(1, 7, (int)sliderGainValue->load());
    //message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
    //auto timestamp = message.getTimeStamp();
    //auto sampleNumber = (int)(timestamp * 44100.0);
    //midiMessages.addEvent(message, sampleNumber);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);
    //    auto sliderGainValue = treeState.getRawParameterValue(GAIN_ID);
    //    // ..do something to the data...
    //    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    //        channelData[sample] = buffer.getSample(channel, sample) * juce::Decibels::decibelsToGain(sliderGainValue->load());
    //    }
    //}
}

//==============================================================================
bool AbletonXsensAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AbletonXsensAudioProcessor::createEditor()
{
    return new AbletonXsensAudioProcessorEditor (*this);
}

//==============================================================================
void AbletonXsensAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AbletonXsensAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AbletonXsensAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout AbletonXsensAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(
        std::make_unique<juce::AudioParameterFloat>("gain", "gain", -48.0f, 0.0f, -1.0f)
    );
    return { params.begin(), params.end() };
}

void AbletonXsensAudioProcessor::addParameters(std::map<std::string, float>& params)
{
    //for (auto param : params) {
    //    treeState.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>(param.first, param.first, 0, 100, param.second));
    //}
    treeState.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("euler_x", "euler_x", 0, 100, params["euler_x"]));
    
    addParameter(new juce::AudioParameterFloat("euler_x", // parameterID
        "euler_x", // parameter name
        0.0f,   // minimum value
        1.0f,   // maximum value
        0.5f)); // default value
}


