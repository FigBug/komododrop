/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "Settings.h"
//[/Headers]

#include "SettingsComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SettingsComponent::SettingsComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    cancel.reset (new TextButton ("cancel"));
    addAndMakeVisible (cancel.get());
    cancel->addListener (this);

    cancel->setBounds (256, 168, 80, 24);

    ok.reset (new TextButton ("ok"));
    addAndMakeVisible (ok.get());
    ok->addListener (this);

    ok->setBounds (168, 168, 80, 24);

    silentUploads.reset (new ToggleButton ("silentUploads"));
    addAndMakeVisible (silentUploads.get());
    silentUploads->setButtonText (TRANS("silent uploading"));
    silentUploads->addListener (this);

    silentUploads->setBounds (64, 80, 150, 24);

    duplicate.reset (new ToggleButton ("duplicate"));
    addAndMakeVisible (duplicate.get());
    duplicate->setButtonText (TRANS("check for duplicates"));
    duplicate->addListener (this);

    duplicate->setBounds (64, 104, 150, 24);

    threads.reset (new Slider ("threads"));
    addAndMakeVisible (threads.get());
    threads->setRange (1, 16, 1);
    threads->setSliderStyle (Slider::IncDecButtons);
    threads->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    threads->addListener (this);

    threads->setBounds (176, 128, 128, 24);

    label.reset (new Label ("label",
                            TRANS("upload threads:")));
    addAndMakeVisible (label.get());
    label->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label->setBounds (64, 128, 112, 24);

    auth.reset (new TextButton ("auth"));
    addAndMakeVisible (auth.get());
    auth->setButtonText (TRANS("authorize Smugmug"));
    auth->addListener (this);

    auth->setBounds (104, 32, 150, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (350, 200);


    //[Constructor] You can add your own custom stuff here..
	Settings* settings = Settings::getInstance();
	silentUploads->setToggleState(settings->silentUpload, dontSendNotification);
	duplicate->setToggleState(settings->checkForDupes, dontSendNotification);
	threads->setValue(settings->uploadThreads);

    //[/Constructor]
}

SettingsComponent::~SettingsComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    cancel = nullptr;
    ok = nullptr;
    silentUploads = nullptr;
    duplicate = nullptr;
    threads = nullptr;
    label = nullptr;
    auth = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SettingsComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SettingsComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SettingsComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == cancel.get())
    {
        //[UserButtonCode_cancel] -- add your button handler code here..
		delete getParentComponent();
        //[/UserButtonCode_cancel]
    }
    else if (buttonThatWasClicked == ok.get())
    {
        //[UserButtonCode_ok] -- add your button handler code here..
		Settings* settings = Settings::getInstance();
		settings->silentUpload = silentUploads->getToggleState();
		settings->checkForDupes = duplicate->getToggleState();
		settings->uploadThreads = roundToInt(threads->getValue());
		settings->save();
		delete getParentComponent();
        //[/UserButtonCode_ok]
    }
    else if (buttonThatWasClicked == silentUploads.get())
    {
        //[UserButtonCode_silentUploads] -- add your button handler code here..
        //[/UserButtonCode_silentUploads]
    }
    else if (buttonThatWasClicked == duplicate.get())
    {
        //[UserButtonCode_duplicate] -- add your button handler code here..
        //[/UserButtonCode_duplicate]
    }
    else if (buttonThatWasClicked == auth.get())
    {
        //[UserButtonCode_auth] -- add your button handler code here..
        //[/UserButtonCode_auth]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void SettingsComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == threads.get())
    {
        //[UserSliderCode_threads] -- add your slider handling code here..
        //[/UserSliderCode_threads]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SettingsComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="200">
  <BACKGROUND backgroundColour="ffffff"/>
  <TEXTBUTTON name="cancel" id="b302b81768bcf84f" memberName="cancel" virtualName=""
              explicitFocusOrder="0" pos="256 168 80 24" buttonText="cancel"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="ok" id="fc1a94ed4dd2cea5" memberName="ok" virtualName=""
              explicitFocusOrder="0" pos="168 168 80 24" buttonText="ok" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="silentUploads" id="7208c2dd88597e80" memberName="silentUploads"
                virtualName="" explicitFocusOrder="0" pos="64 80 150 24" buttonText="silent uploading"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="duplicate" id="1cd3f8be6b3b53ca" memberName="duplicate"
                virtualName="" explicitFocusOrder="0" pos="64 104 150 24" buttonText="check for duplicates"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="threads" id="35f0013081e13bf1" memberName="threads" virtualName=""
          explicitFocusOrder="0" pos="176 128 128 24" min="1" max="1.6e1"
          int="1" style="IncDecButtons" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1" needsCallback="1"/>
  <LABEL name="label" id="f6bc37f58b818a10" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="64 128 112 24" edTextCol="ff000000"
         edBkgCol="0" labelText="upload threads:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="1.5e1" kerning="0" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="auth" id="3a9656f4c2a33c38" memberName="auth" virtualName=""
              explicitFocusOrder="0" pos="104 32 150 24" buttonText="authorize Smugmug"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

