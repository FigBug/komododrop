/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  29 Jan 2013 11:56:27am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_SETTINGSCOMPONENT_SETTINGSCOMPONENT_D40CF292__
#define __JUCER_HEADER_SETTINGSCOMPONENT_SETTINGSCOMPONENT_D40CF292__

//[Headers]     -- You can add your own extra header files here --
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SettingsComponent  : public Component,
                           public ButtonListener,
                           public SliderListener
{
public:
    //==============================================================================
    SettingsComponent ();
    ~SettingsComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    Label* label2;
    Label* label1;
    TextEditor* email;
    TextEditor* password;
    TextButton* cancel;
    TextButton* ok;
    ToggleButton* silentUploads;
    ToggleButton* duplicate;
    Slider* threads;
    Label* label;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCER_HEADER_SETTINGSCOMPONENT_SETTINGSCOMPONENT_D40CF292__
