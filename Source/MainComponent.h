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
#include <ctime>

static String getMidiMessageDescription (const MidiMessage& m)
{
    if (m.isNoteOn())           return "Note on "  + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off " + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change " + String (m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel " + String (m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch " + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) +  ": " + String (m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + String (m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";
    
    if (m.isController())
    {
        String name (MidiMessage::getControllerName (m.getControllerNumber()));
        
        if (name.isEmpty())
            name = "[" + String (m.getControllerNumber()) + "]";
        
        return "Controler " + name + ": " + String (m.getControllerValue());
    }
    
    return String::toHexString (m.getRawData(), m.getRawDataSize());
}

//==============================================================================
/** Simple list box that just displays a StringArray. */
class MidiLogListBoxModel   : public ListBoxModel
{
public:
    MidiLogListBoxModel (const Array<MidiMessage>& list)
    : midiMessageList (list)
    {
    }
    
    int getNumRows() override    { return midiMessageList.size(); }
    
    void paintListBoxItem (int row, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (Colours::blue.withAlpha (0.2f));
        
        if (isPositiveAndBelow (row, midiMessageList.size()))
        {
            g.setColour (Colours::black);
            
            const MidiMessage& message = midiMessageList.getReference (row);
            double time = message.getTimeStamp();
            
            g.drawText (String::formatted ("%02d:%02d:%02d",
                                           ((int) (time / 3600.0)) % 24,
                                           ((int) (time / 60.0)) % 60,
                                           ((int) time) % 60)
                        + "  -  " + getMidiMessageDescription (message),
                        Rectangle<int> (width, height).reduced (4, 0),
                        Justification::centredLeft, true);
        }
    }
    
    
    
private:
    const Array<MidiMessage>& midiMessageList;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiLogListBoxModel)
};


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
    
    
//    struct IncomingMessageCallback   : public CallbackMessage
//    {
//        IncomingMessageCallback (MainContentComponent* mcc, const MidiMessage& m)
//        : mainComponent(mcc), message (m) {}
//        
//        void messageCallback() override
//        {
//        if (mainComponent != nullptr)
//            mainComponent->addMessageToList (message);
//        }
//    
//        Component::SafePointer<MainContentComponent> mainComponent;
//        MidiMessage message;
//    };

    


private:
    AudioDeviceManager deviceManager;
    File currentFile;
    
    ScopedPointer<Label> title;
    ScopedPointer<TextButton> streamButton;
    ScopedPointer<TextButton> loadFileButton;
    
    ScopedPointer<ListBox> messageListBox;
    Array<MidiMessage> midiMessageList;
    MidiLogListBoxModel midiLogListBoxModel;
    MidiMessage midiMsg;
    clock_t startTime;
    double elapsedTimeInSecs;
    
    MidiKeyboardState keyboardState;
    ScopedPointer<MidiKeyboardComponent> keyboardUI;
    
    ScopedPointer<LiveStreaming> streaming;
    ScopedPointer<AudioFileSource> player;
    
    NMF* nmf;
    float* transcription;
    float* previousTranscription;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
