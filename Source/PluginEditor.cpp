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
void AbletonXsensAudioProcessorEditor::handleDataRows(std::istringstream& stream, std::string& currentLine, int buffer, juce::String& logMessage)
{
    while (std::getline(stream, currentLine, '\n')) {
        std::string::size_type pos = currentLine.find(':');
        // if found:
        if (pos != std::string::npos)
        {
            std::string currParam = currentLine.substr(0, pos);
            if (XsensSliders.find(currParam) != XsensSliders.end()) {
                //validate that value isn't NaN. TODO: check if it works.
                if (currentLine.find("NaN", pos) == std::string::npos) {
                    double paramValue = std::stod(currentLine.substr(pos + 1, buffer));

                    XsensSliders[currParam]->slider.
                        setValue(XsensSliders[currParam]->sensitivity * XsensSliders[currParam]->invertion * paramValue);
                    logMessage << currParam << ": " << paramValue << "\n";
                }
                else {
                    logMessage << currParam << ": nan \n";
                }
            }
        }
    }
    juce::Logger::getCurrentLogger()->writeToLog(logMessage);
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
    setSize(1300,190);

    // initialize AnalyzerSocket:
    this->socketClient = std::make_unique<client>();
    this->socketClient->connect("http://127.0.0.1:3001");
    bindSocketEvents();
    
     //initialize sensor values Sliders and labels:
    int i = 0;
    for (const auto& param: audioProcessor.params) {
        XsensSliders[param.name] = std::make_unique<XsensSlider>();
        XsensSliders[param.name]->slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        XsensSliders[param.name]->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
        XsensSliders[param.name]->slider.setSize(80, 80);
        XsensSliders[param.name]->slider.setCentrePosition(40+80*i, 60);
        XsensSliders[param.name]->slider.setRange(param.minValue, param.maxValue);
        XsensSliders[param.name]->slider.setValue((param.minValue+param.maxValue)/2);
        addAndMakeVisible(XsensSliders[param.name]->slider);
        gainSliderAttachments[param.name] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState,param.name, XsensSliders[param.name]->slider);
        sliderLabels[param.name] = std::make_unique<juce::Label>();
        sliderLabels[param.name]->setText(param.name, juce::dontSendNotification);
        sliderLabels[param.name]->attachToComponent(&XsensSliders[param.name]->slider, false);
        addAndMakeVisible(*sliderLabels[param.name]);
        i++;
    }

    // initialize sensitivity slider:
    sensitivitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sensitivitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    sensitivitySlider.setSize(100, 50);
    sensitivitySlider.setCentrePosition(130, 130);
    sensitivitySlider.setRange(0, 5);
    sensitivitySlider.setValue(1);
    sensitivitySlider.addListener(this);
    addAndMakeVisible(sensitivitySlider);
    sensitivityLabel.setText("Sensitivity:", juce::dontSendNotification);
    sensitivityLabel.attachToComponent(&sensitivitySlider, true);
    addAndMakeVisible(sensitivityLabel);

    //initialize invert button:
    invertButton.setSize(50, 50);
    invertButton.setCentrePosition(260, 130);
    invertButton.addListener(this);
    addAndMakeVisible(invertButton);
    invertLabel.setText("invert:", juce::dontSendNotification);
    invertLabel.attachToComponent(&invertButton, true);
    addAndMakeVisible(invertLabel);

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
    for (auto& pair : XsensSliders) {
        pair.second->slider.setBounds(getLocalBounds());
    }
}

void  AbletonXsensAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sensitivitySlider) {
        for (auto& xsensSlider : XsensSliders) {
            xsensSlider.second->sensitivity = slider->getValue();
        }
    }
}

void AbletonXsensAudioProcessorEditor::buttonClicked(juce::Button* button) {
    if (button == &invertButton) {
        for (auto& xsensSlider : XsensSliders) {
            xsensSlider.second->invertion*=-1;
        }
    }
}