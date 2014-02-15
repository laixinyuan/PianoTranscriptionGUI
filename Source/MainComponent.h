/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "NMF.h"
#include "AudioFileSource.h"
#include "LiveStreaming.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public Timer, public ButtonListener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
    
    void timerCallback();
    
    void buttonClicked(Button* buttonThatWasClicked);
    
private:
    AudioDeviceManager deviceManager;
    File currentFile;
//    Path internalPath1;
    
    ScopedPointer<Label> title;
    ScopedPointer<TextButton> streamButton;
    ScopedPointer<TextButton> loadFileButton;
    
    MidiKeyboardState keyboardState;
    ScopedPointer<MidiKeyboardComponent> keyboardUI;
    
    ScopedPointer<LiveStreaming> streaming;
    ScopedPointer<AudioFileSource> player;
    
    ScopedPointer<NMF> nmf;
    
    float* transcription;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
