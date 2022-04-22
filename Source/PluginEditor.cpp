/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//TODO: Move Socket to the pluginProcessor class. or to a new class.

/**
    Binded to 'recieve-data' Socket event. this method handles data transfer from sensors
    connected to the XsensModule.
*/
void AbletonXsensAudioProcessorEditor::onDataTransfer(std::string msg) {
    //TODO: add a data analyzer object that will do all the data handling.
    int buffer = 8;
    std::string param_name = "euler_x";
    double param_value = std::stod(msg.substr(msg.find(param_name) + 8, buffer));
    param_value = ((param_value + 180) * -100) / 360;
    juce::String message;
    message << param_name << ": " << param_value << "\n";
    juce::Logger::getCurrentLogger()->writeToLog(message);
    //audioProcessor.treeState.getParameter(param_name)->setValue(param_value);
    gainSlider.setValue(param_value);
}

void AbletonXsensAudioProcessorEditor::onReceiveMsg(event& ev) {
    std::string msg = ev.get_message()->get_string();
    onDataTransfer(msg);
}

/**
   binded to sensor-connect, this method updates this Module when a sensor connected
   to the Xsens Module.
   creates a sensor object and add it to the currConnectedSensors hashmap.
*/
void AbletonXsensAudioProcessorEditor::sensorConnect(event& ev) {
    //TODO:implement.
}

/**
   binded to sensor-disconnect, this method updates this Module when a sensor disconnected
   from the Xsens Module.
   deletes the disconnected sensor from the currConnectedSensors hashmap.
*/
void AbletonXsensAudioProcessorEditor::sensorDisconnect(event& ev) {
    //TODO:implement.
}

/**
   this method send message to Xsens Module for changing the sample rate.
*/
void AbletonXsensAudioProcessorEditor::changeSampleRate(int sampleRate) {
    //TODO:implement.
}

// bind Socket Events:
void AbletonXsensAudioProcessorEditor::bindSocketEvents() {
    std::function<void(event& event)> const& func1 = std::bind(&AbletonXsensAudioProcessorEditor::onReceiveMsg, this, std::placeholders::_1);
    this->socketClient->socket()->on("recieve-data", func1);
    std::function<void(event& event)> const& func2 = std::bind(&AbletonXsensAudioProcessorEditor::sensorConnect, this, std::placeholders::_1);
    this->socketClient->socket()->on("sensor-connect", func2);
    std::function<void(event& event)> const& func3 = std::bind(&AbletonXsensAudioProcessorEditor::sensorDisconnect, this, std::placeholders::_1);
    this->socketClient->socket()->on("sensor-disconnect", func3);
}


//==============================================================================
AbletonXsensAudioProcessorEditor::AbletonXsensAudioProcessorEditor (AbletonXsensAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), 
    m_log_file("~/log_test.txt"), m_logger(m_log_file, "Welcome to the log", 0)
{
    juce::Logger::setCurrentLogger(&m_logger);
    setSize(200, 400);

    // initialize AnalyzerSocket:
    this->socketClient = std::make_unique<client>();
    this->socketClient->connect("http://127.0.0.1:3001");
    bindSocketEvents();
    
    //add first parameter to map:
    std::map<std::string, float> params;
    params["euler_x"] = 0;
    audioProcessor.addParameters(params);
     //initialize Slider:
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    gainSlider.setRange(-48.0, 0.0);
    gainSlider.setValue(-1.0);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "gain", gainSlider);
}

AbletonXsensAudioProcessorEditor::~AbletonXsensAudioProcessorEditor()
{
}

//==============================================================================
void AbletonXsensAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AbletonXsensAudioProcessorEditor::resized()
{
    gainSlider.setBounds(getLocalBounds());
}

void AbletonXsensAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    //audioProcessor.treeState.getParameter("euler_x")->setValue(slider->getValue());
}
