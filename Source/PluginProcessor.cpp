/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const struct XsensParameter AbletonXsensAudioProcessor::params[] =
{
        {"euler_x",-1, 1},
        {"euler_y",-1, 1},
        {"euler_z",-1, 1},
        /* {"acc_x",-180,180},
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
         {"quaternion_z",-180,180} */
};
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
    startTime(juce::Time::getMillisecondCounterHiRes() * 0.001),
    m_log_file("~/log_test.txt"), m_logger(m_log_file, "Welcome to the log", 0),
    streamAllocator(MAX_SENSORS_NUM)

#endif

{
    juce::Logger::setCurrentLogger(&m_logger);
    std::function<void(std::string)> const& handler = std::bind(&AbletonXsensAudioProcessor::onDataTransfer, this, std::placeholders::_1);
    this->XsensClient = std::make_unique<XsenSocket>(handler);
}

AbletonXsensAudioProcessor::~AbletonXsensAudioProcessor()
{

}

juce::AudioProcessorValueTreeState::ParameterLayout AbletonXsensAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> pluginParams;
    for (int sensorIdx = 0; sensorIdx < MAX_SENSORS_NUM; sensorIdx++) {
        for (auto& param : this->params) {
            auto name = param.name + "_" + std::to_string(sensorIdx);
            pluginParams.push_back(
                std::make_unique<juce::AudioParameterFloat>(name, name, param.minValue, param.maxValue, (param.minValue + param.maxValue) / 2));

            auto currSensitivityName = param.name + "_" + std::to_string(sensorIdx) + SENSITIVITY;
            pluginParams.push_back(
                std::make_unique<juce::AudioParameterFloat>(currSensitivityName, currSensitivityName, MIN_SENSITIVITY, MAX_SENSITIVITY, 1));

            auto currInvertionName = param.name + "_" + std::to_string(sensorIdx) + INVERTION;
            pluginParams.push_back(
                std::make_unique<juce::AudioParameterBool>(currInvertionName, currInvertionName, false, currInvertionName));

            auto currShiftingName = param.name + "_" + std::to_string(sensorIdx) + SHIFTING;
            pluginParams.push_back(
                std::make_unique<juce::AudioParameterFloat>(currShiftingName, currShiftingName, MIN_SHIFTING, MAX_SHIFTING, 0));

        }
    }

    return { pluginParams.begin(), pluginParams.end() };
}

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
    int currSlot = extractSlot(currentLine);
    // other rows handling - data:
    this->extractSensorData(stream, currentLine, buffer, logMessage, currSlot);
}

/**
this method extracts the mac-address of the sensor.
it then uses the streamAllocator to get the slot.
*/
int AbletonXsensAudioProcessor::extractSlot(std::string firstLine) {
    std::string::size_type pos = firstLine.find(',');
    std::string currId = firstLine.substr(pos + 1, pos + 18);
    return streamAllocator.IdToSlot(currId);
}

/**
    this method iterates on each row from the data stream, extracts param name and value
    and changes XsensSliders values accordingly.
*/
void AbletonXsensAudioProcessor::extractSensorData(std::istringstream& stream, std::string& currentLine, int buffer, juce::String& logMessage, int currSlot)
{

    //TODO: refactor!!!
    while (std::getline(stream, currentLine, '\n')) {
        std::string::size_type pos = currentLine.find(':');
        // if found:
        if (pos != std::string::npos)
        {
            std::string currParam = currentLine.substr(0, pos) + "_" + std::to_string(currSlot);
            if (this->treeState.getParameter(currParam) != nullptr) {
                //validate that value isn't NaN. TODO: check if it works.
                if (currentLine.find("NaN", pos) == std::string::npos) {
                    double paramValue = std::stod(currentLine.substr(pos + 1, buffer));
                    if (this->treeState.getParameter(currParam + SENSITIVITY) != nullptr && this->treeState.getParameter(currParam + INVERTION) != nullptr) {
                        float newValue = mapSensorValues(currParam, paramValue);
                        this->treeState.getParameter(currParam)->setValueNotifyingHost(
                            this->treeState.getParameterRange(currParam).convertTo0to1(newValue));
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

/**
    this methods maps a sensor parameter value to a new value that is generated by sin function and 
    shifting/sensitivity/invertion params.
*/
float AbletonXsensAudioProcessor::mapSensorValues(std::string currParam, double paramValue) {
    auto paramSensitivity = this->treeState.getRawParameterValue(currParam + SENSITIVITY);
    auto paramShifting = this->treeState.getRawParameterValue(currParam + SHIFTING);
    auto paramInvertion = this->treeState.getParameter(currParam + INVERTION);
    int sign = 1;
    if ((bool)paramInvertion->getValue() == true) {
        sign = -1;
    }
    return sign * std::sin(((paramValue * *paramSensitivity / 180) * 3.14) + *paramShifting);
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

void AbletonXsensAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AbletonXsensAudioProcessor::getProgramName(int index)
{
    return {};
}

void AbletonXsensAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void AbletonXsensAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
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
bool AbletonXsensAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void AbletonXsensAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}

//==============================================================================
bool AbletonXsensAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AbletonXsensAudioProcessor::createEditor()
{
    return new AbletonXsensAudioProcessorEditor(*this);
}

//==============================================================================
void AbletonXsensAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AbletonXsensAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
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


