/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent():midiLogListBoxModel (midiMessageList)
{
    
    deviceManager.initialise(2, 2, 0, true);
    
    nmf = new NMF();
    transcription = new float[88];  // hard coded
    
    addAndMakeVisible (title = new Label (String::empty, "Real-time Polyphonic Piano Transcription"));
    title->setFont (Font (28.00f, Font::bold));
    title->setJustificationType (Justification::topLeft);
    title->setEditable (false, false, false);
    title->setColour (Label::textColourId, Colour(255, 178, 102));
    
    addAndMakeVisible(streamButton = new TextButton(String::empty) );
    streamButton->setButtonText("Streaming off");
    streamButton->addListener(this);
    streamButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    streamButton->setColour (TextButton::textColourOnId, Colours::darkgrey);
    
    addAndMakeVisible(loadFileButton = new TextButton(String::empty));
    loadFileButton->setButtonText("Load Audio File");
    loadFileButton->addListener(this);
    loadFileButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    loadFileButton->setColour (TextButton::textColourOnId, Colours::darkgrey);
    
    addAndMakeVisible(keyboardUI = new MidiKeyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard));
    keyboardUI->setAvailableRange(21, 108);
    keyboardUI->setKeyWidth(18.f);
    keyboardUI->setColour(MidiKeyboardComponent::keyDownOverlayColourId, Colours::mediumseagreen);
    keyboardUI->setColour(MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colours::mediumseagreen);
    
    addAndMakeVisible(messageListBox = new ListBox());
    messageListBox->setModel (&midiLogListBoxModel);
    messageListBox->setColour (ListBox::backgroundColourId, Colour (0x32ffffff));
    messageListBox->setColour (ListBox::outlineColourId, Colours::black);
    
    setSize (1080, 720);
    
    startTimer(10);
    
}

MainContentComponent::~MainContentComponent()
{
    title = nullptr;
    streamButton = nullptr;
    loadFileButton = nullptr;
    
    messageListBox = nullptr;
    
    keyboardUI = nullptr;
    
    streaming = nullptr;
    player = nullptr;
    
    delete nmf;
    delete [] transcription;
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0x24affffff));

//    g.setFont (Font (16.0f));
//    g.setColour (Colours::black);
//    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    
    
    title->setBounds (getWidth()/2-160, 20, 400, 200);
    streamButton->setBounds (getWidth()/8, getHeight()/4-80, 120, 30);
    loadFileButton->setBounds (getWidth()/8 + 120, getHeight()/4-80, 120, 30);
    keyboardUI->setBounds(getWidth()/8-30, getHeight()/2 -200, 925, getHeight()/2 - 200);
    messageListBox->setBounds(getWidth()/8-30, getHeight()/2 , 925, getHeight()/2 -30);
    
}

void MainContentComponent::buttonClicked(Button *buttonThatWasClicked)
{
    
    if (buttonThatWasClicked == streamButton)
    {
        if (!streaming)
        {
            player = nullptr;
            streaming = new LiveStreaming(deviceManager, nmf, transcription);
            streamButton->setButtonText("Streaming on");
            streamButton->setColour(TextButton::buttonColourId, Colours::blue);
        }
        else
        {
            streaming = nullptr;
            streamButton->setButtonText("Streaming off");
            streamButton->setColour(TextButton::buttonColourId, Colours::lightblue);
        }
    }
    
    if (buttonThatWasClicked == loadFileButton)
    {
        if (!streaming)
        {
            // load audio file here
            FileChooser chooser (("Choose file to open"),File::nonexistent,"*",true);
            chooser.browseForFileToOpen();
            currentFile = File(chooser.getResult());
//            if (!player)
//            {
            player = nullptr;
            player = new AudioFileSource(deviceManager, nmf, transcription);
            player->setFile(currentFile);
            std::cout<< "load file: "<<currentFile.getFileName()<<std::endl;
//            }
            
        }
    }
    
    
}

void MainContentComponent::timerCallback()
{
    
    for (int j = 0; j<88; j++) {
        if (transcription[j] == 1) {
            keyboardState.noteOn(1, j+21, 127);
        }
        else {
            keyboardState.noteOff(1, j+21);
        }
    }
}



