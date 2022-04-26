/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const struct XsensParameter AbletonXsensAudioProcessor::params[] =
{
        {"euler_x",-180,180},
        {"euler_y",-180,180},
        {"euler_z",-180,180},
        {"acc_x",-180,180},
        {"acc_y",-180,180},
        {"acc_z",-180,180},
        {"gyr_x",-2000,2000},
        {"gyr_y",-2000,2000},
        {"gyr_z",-2000,2000},
        //TODO: validate min-max values:
        {"mag_x",-10,10},
        {"mag_y",-10,10},
        {"mag_z",-10,100},
        {"quaternion_w",-180,180},
        {"quaternion_x",-180,180},
        {"quaternion_y",-180,180},
        {"quaternion_z",-180,180}
};
//==============================================================================

/**
    Binded to 'recieve-data' Socket event. this method handles data transfer from sensors
    connected to the XsensModule.
*/
void AbletonXsensAudioProcessor::onDataTransfer(std::string msg) {
    int buffer = 8;
    std::istringstream stream(msg);
    std::string currentLine;
    juce::String logMessage;
    // first row handling - control:
    std::getline(stream, currentLine, '\n');
    // other rows handling - data:
    handleDataRows(stream, currentLine, buffer, logMessage);
}

/**
    this method iterates on each row from the data stream, extracts param name and value
    and changes XsensSliders values accordingly.
*/
void AbletonXsensAudioProcessor::handleDataRows(std::istringstream& stream, std::string& currentLine, int buffer, juce::String& logMessage)
{
    //TODO: refactor!!!
    while (std::getline(stream, currentLine, '\n')) {
        std::string::size_type pos = currentLine.find(':');
        // if found:
        if (pos != std::string::npos)
        {
            std::string currParam = currentLine.substr(0, pos);
            if (treeState.getParameter(currParam) != nullptr) {
                //validate that value isn't NaN. TODO: check if it works.
                if (currentLine.find("NaN", pos) == std::string::npos) {
                    double paramValue = std::stod(currentLine.substr(pos + 1, buffer));
                    //XsensSliders[currParam]->slider.setValue(XsensSliders[currParam]->sensitivity * XsensSliders[currParam]->invertion * paramValue);
                    if (treeState.getParameter(currParam + SENSITIVITY) != nullptr && treeState.getParameter(currParam + INVERTION) != nullptr) {
                        auto paramSensitivity = treeState.getRawParameterValue(currParam + SENSITIVITY);
                        auto paramInvertion = treeState.getParameter(currParam + INVERTION);
                        float newValue;
                        if ((bool)paramInvertion->getValue() == false) {
                            newValue = paramValue * *paramSensitivity;
                        }
                        else {
                            newValue = -1 * paramValue * *paramSensitivity;
                        }
                        treeState.getParameter(currParam)->setValueNotifyingHost(
                            treeState.getParameterRange(currParam).convertTo0to1(newValue));

                        logMessage << currParam << ": " << paramValue << "\n";
                    }
                }
                else {
                    logMessage << currParam << ": nan \n";
                }
            }
        }
    }
    juce::Logger::getCurrentLogger()->writeToLog(logMessage);
}

void AbletonXsensAudioProcessor::onReceiveMsg(event& ev) {
    std::string msg = ev.get_message()->get_string();
    onDataTransfer(msg);
}

/**
   binded to sensor-connect, this method updates this Module when a sensor connected
   to the Xsens Module.
   creates a sensor object and add it to the currConnectedSensors hashmap.
*/
void AbletonXsensAudioProcessor::sensorConnect(event& ev) {
    //TODO:implement.
}

/**
   binded to sensor-disconnect, this method updates this Module when a sensor disconnected
   from the Xsens Module.
   deletes the disconnected sensor from the currConnectedSensors hashmap.
*/
void AbletonXsensAudioProcessor::sensorDisconnect(event& ev) {
    //TODO:implement.
}

/**
   this method send message to Xsens Module for changing the sample rate.
*/
void AbletonXsensAudioProcessor::changeSampleRate(int sampleRate) {
    //TODO:implement.
}

// bind Socket Events:
void AbletonXsensAudioProcessor::bindSocketEvents() {
    std::function<void(event& event)> const& func1 = std::bind(&AbletonXsensAudioProcessor::onReceiveMsg, this, std::placeholders::_1);
    this->socketClient->socket()->on("recieve-data", func1);
    std::function<void(event& event)> const& func2 = std::bind(&AbletonXsensAudioProcessor::sensorConnect, this, std::placeholders::_1);
    this->socketClient->socket()->on("sensor-connect", func2);
    std::function<void(event& event)> const& func3 = std::bind(&AbletonXsensAudioProcessor::sensorDisconnect, this, std::placeholders::_1);
    this->socketClient->socket()->on("sensor-disconnect", func3);
}

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
        startTime(juce::Time::getMillisecondCounterHiRes() * 0.001),
        m_log_file("~/log_test.txt"), m_logger(m_log_file, "Welcome to the log", 0)

#endif
    
{
    juce::Logger::setCurrentLogger(&m_logger);

    // initialize AnalyzerSocket:
    this->socketClient = std::make_unique<client>();
    this->socketClient->connect("http://127.0.0.1:3001");
    bindSocketEvents();

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
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> pluginParams;
    for (auto& param : this->params) {
        pluginParams.push_back(
            std::make_unique<juce::AudioParameterFloat>(param.name, param.name, param.minValue, param.maxValue, (param.minValue + param.maxValue) / 2));
    }
    // adding sensitivity and invert sliders:
    for (auto& param : this->params) {
        pluginParams.push_back(
            std::make_unique<juce::AudioParameterFloat>(param.name+SENSITIVITY, param.name+SENSITIVITY, MIN_SENSITIVITY, MAX_SENSITIVITY, 1));
        pluginParams.push_back(
            std::make_unique<juce::AudioParameterBool>(param.name + INVERTION, param.name + INVERTION, false, param.name + INVERTION));

    }
    return { pluginParams.begin(), pluginParams.end() };
}

