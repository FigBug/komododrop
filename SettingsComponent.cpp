/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.3.2

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

    label2.reset (new Label ("label2",
                             TRANS("password:")));
    addAndMakeVisible (label2.get());
    label2->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label2->setBounds (16, 48, 104, 24);

    label1.reset (new Label ("label1",
                             TRANS("email address:")));
    addAndMakeVisible (label1.get());
    label1->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label1->setJustificationType (Justification::centredLeft);
    label1->setEditable (false, false, false);
    label1->setColour (TextEditor::textColourId, Colours::black);
    label1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label1->setBounds (16, 16, 104, 24);

    email.reset (new TextEditor ("email"));
    addAndMakeVisible (email.get());
    email->setMultiLine (false);
    email->setReturnKeyStartsNewLine (false);
    email->setReadOnly (false);
    email->setScrollbarsShown (true);
    email->setCaretVisible (true);
    email->setPopupMenuEnabled (true);
    email->setText (String());

    email->setBounds (128, 16, 200, 24);

    password.reset (new TextEditor ("password"));
    addAndMakeVisible (password.get());
    password->setMultiLine (false);
    password->setReturnKeyStartsNewLine (false);
    password->setReadOnly (false);
    password->setScrollbarsShown (true);
    password->setCaretVisible (true);
    password->setPopupMenuEnabled (true);
    password->setText (String());

    password->setBounds (128, 48, 200, 24);

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


    //[UserPreSize]
    //[/UserPreSize]

    setSize (350, 200);


    //[Constructor] You can add your own custom stuff here..
	Settings* settings = Settings::getInstance();
	password->setText(settings->password);
	email->setText(settings->email);
	silentUploads->setToggleState(settings->silentUpload, dontSendNotification);
	duplicate->setToggleState(settings->checkForDupes, dontSendNotification);
	threads->setValue(settings->uploadThreads);

    //[/Constructor]
}

SettingsComponent::~SettingsComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    label2 = nullptr;
    label1 = nullptr;
    email = nullptr;
    password = nullptr;
    cancel = nullptr;
    ok = nullptr;
    silentUploads = nullptr;
    duplicate = nullptr;
    threads = nullptr;
    label = nullptr;


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
		settings->password = password->getText();
		settings->email    = email->getText();
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
  <LABEL name="label2" id="ec6c668c4d0b411d" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="16 48 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="password:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="33"/>
  <LABEL name="label1" id="75f19df5ed823858" memberName="label1" virtualName=""
         explicitFocusOrder="0" pos="16 16 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="email address:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="email" id="a9220190b5eb5b13" memberName="email" virtualName=""
              explicitFocusOrder="0" pos="128 16 200 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <TEXTEDITOR name="password" id="a508254e81adf08a" memberName="password" virtualName=""
              explicitFocusOrder="0" pos="128 48 200 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
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
          explicitFocusOrder="0" pos="176 128 128 24" min="1.00000000000000000000"
          max="16.00000000000000000000" int="1.00000000000000000000" style="IncDecButtons"
          textBoxPos="TextBoxLeft" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.00000000000000000000" needsCallback="1"/>
  <LABEL name="label" id="f6bc37f58b818a10" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="64 128 112 24" edTextCol="ff000000"
         edBkgCol="0" labelText="upload threads:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
