/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (900, 600);
    
    startTimer(10);
    
    deviceManager.initialise(2, 2, 0, true);
    
    nmf = new NMF();
    transcription = new float(nmf->getNumNotes());
    
    addAndMakeVisible (title = new Label (String::empty, "Real-time Polyphonic "));
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
    keyboardUI->setKeyWidth(20.f);
    keyboardUI->setColour(MidiKeyboardComponent::keyDownOverlayColourId, Colours::mediumseagreen);
    keyboardUI->setColour(MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colours::mediumseagreen);
    
}

MainContentComponent::~MainContentComponent()
{
    title = nullptr;
    streamButton = nullptr;
    loadFileButton = nullptr;
    
    keyboardUI = nullptr;
    
    streaming = nullptr;
    player = nullptr;
    
    nmf = nullptr;
    
    delete [] transcription;
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (48,48,48));

//    g.setFont (Font (16.0f));
//    g.setColour (Colours::black);
//    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
//    title->setBounds (getWidth()/2-160, 20, 400, 200);
//    streamButton->setBounds (getWidth()/8, getHeight()/4-80, 120, 30);
//    loadFileButton->setBounds (getWidth()/8 + 120, getHeight()/4-80, 120, 30);
//    
//    keyboardUI ->setBounds(getWidth()/8-30, getHeight()/2 + 10, 350, getHeight()/2 - 70);
//    
//    internalPath1.clear();
//    internalPath1.startNewSubPath (136.0f, 80.0f);
//    internalPath1.quadraticTo (176.0f, 24.0f, 328.0f, 32.0f);
//    internalPath1.quadraticTo (472.0f, 40.0f, 472.0f, 104.0f);
//    internalPath1.quadraticTo (472.0f, 192.0f, 232.0f, 176.0f);
//    internalPath1.lineTo (184.0f, 216.0f);
//    internalPath1.lineTo (200.0f, 168.0f);
//    internalPath1.quadraticTo (96.0f, 136.0f, 136.0f, 80.0f);
//    internalPath1.closeSubPath();
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
            if (!player)
            {
                player = new AudioFileSource(deviceManager, nmf, transcription);
                player->setFile(currentFile);
                std::cout<< "load file: "<<currentFile.getFileName()<<std::endl;
            }
            
        }
    }
    
    
}

void MainContentComponent::timerCallback()
{
    keyboardState.allNotesOff(1);
    for (int j = 0; j<nmf->getNumNotes(); j++) {
        if (transcription[j] == 1) {
            keyboardState.noteOn(1, j+21, 127);
        }
    }
}



