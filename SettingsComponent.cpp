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

//[Headers] You can add your own extra header files here...
#include "stdafx.h"
#include "Settings.h"
//[/Headers]

#include "SettingsComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SettingsComponent::SettingsComponent ()
    : label2 (0),
      label1 (0),
      email (0),
      password (0),
      cancel (0),
      ok (0),
      silentUploads (0),
      duplicate (0),
      threads (0),
      label (0)
{
    addAndMakeVisible (label2 = new Label ("label2",
                                           "password:"));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label1 = new Label ("label1",
                                           "email address:"));
    label1->setFont (Font (15.0000f, Font::plain));
    label1->setJustificationType (Justification::centredLeft);
    label1->setEditable (false, false, false);
    label1->setColour (TextEditor::textColourId, Colours::black);
    label1->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (email = new TextEditor ("email"));
    email->setMultiLine (false);
    email->setReturnKeyStartsNewLine (false);
    email->setReadOnly (false);
    email->setScrollbarsShown (true);
    email->setCaretVisible (true);
    email->setPopupMenuEnabled (true);
    email->setText (String::empty);

    addAndMakeVisible (password = new TextEditor ("password"));
    password->setMultiLine (false);
    password->setReturnKeyStartsNewLine (false);
    password->setReadOnly (false);
    password->setScrollbarsShown (true);
    password->setCaretVisible (true);
    password->setPopupMenuEnabled (true);
    password->setText (String::empty);

    addAndMakeVisible (cancel = new TextButton ("cancel"));
    cancel->addListener (this);

    addAndMakeVisible (ok = new TextButton ("ok"));
    ok->addListener (this);

    addAndMakeVisible (silentUploads = new ToggleButton ("silentUploads"));
    silentUploads->setButtonText ("silent uploading");
    silentUploads->addListener (this);

    addAndMakeVisible (duplicate = new ToggleButton ("duplicate"));
    duplicate->setButtonText ("check for duplicates");
    duplicate->addListener (this);

    addAndMakeVisible (threads = new Slider ("threads"));
    threads->setRange (1, 16, 1);
    threads->setSliderStyle (Slider::IncDecButtons);
    threads->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    threads->addListener (this);

    addAndMakeVisible (label = new Label ("label",
                                          "upload threads:"));
    label->setFont (Font (15.0000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (350, 200);


    //[Constructor] You can add your own custom stuff here..
	Settings* settings = Settings::getInstance();
	password->setText(settings->password);
	email->setText(settings->email);
	silentUploads->setToggleState(settings->silentUpload, false);
	duplicate->setToggleState(settings->checkForDupes, false);
	threads->setValue(settings->uploadThreads);

    //[/Constructor]
}

SettingsComponent::~SettingsComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (label2);
    deleteAndZero (label1);
    deleteAndZero (email);
    deleteAndZero (password);
    deleteAndZero (cancel);
    deleteAndZero (ok);
    deleteAndZero (silentUploads);
    deleteAndZero (duplicate);
    deleteAndZero (threads);
    deleteAndZero (label);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SettingsComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SettingsComponent::resized()
{
    label2->setBounds (16, 48, 104, 24);
    label1->setBounds (16, 16, 104, 24);
    email->setBounds (128, 16, 200, 24);
    password->setBounds (128, 48, 200, 24);
    cancel->setBounds (256, 168, 80, 24);
    ok->setBounds (168, 168, 80, 24);
    silentUploads->setBounds (64, 80, 150, 24);
    duplicate->setBounds (64, 104, 150, 24);
    threads->setBounds (176, 128, 128, 24);
    label->setBounds (64, 128, 112, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SettingsComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == cancel)
    {
        //[UserButtonCode_cancel] -- add your button handler code here..
		delete getParentComponent();
        //[/UserButtonCode_cancel]
    }
    else if (buttonThatWasClicked == ok)
    {
        //[UserButtonCode_ok] -- add your button handler code here..
		Settings* settings = Settings::getInstance();
		settings->password = password->getText();
		settings->email    = email->getText();
		settings->silentUpload = silentUploads->getToggleState();
		settings->checkForDupes = duplicate->getToggleState();
		settings->uploadThreads = roundDoubleToInt(threads->getValue());
		settings->save();
		delete getParentComponent();
        //[/UserButtonCode_ok]
    }
    else if (buttonThatWasClicked == silentUploads)
    {
        //[UserButtonCode_silentUploads] -- add your button handler code here..
        //[/UserButtonCode_silentUploads]
    }
    else if (buttonThatWasClicked == duplicate)
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

    if (sliderThatWasMoved == threads)
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
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SettingsComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="1" initialWidth="350" initialHeight="200">
  <BACKGROUND backgroundColour="ffffffff"/>
  <LABEL name="label2" id="ec6c668c4d0b411d" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="16 48 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="password:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="label1" id="75f19df5ed823858" memberName="label1" virtualName=""
         explicitFocusOrder="0" pos="16 16 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="email address:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
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
          explicitFocusOrder="0" pos="176 128 128 24" min="1" max="16"
          int="1" style="IncDecButtons" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="label" id="f6bc37f58b818a10" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="64 128 112 24" edTextCol="ff000000"
         edBkgCol="0" labelText="upload threads:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[EndFile] You can add extra defines here...
//[/EndFile]
