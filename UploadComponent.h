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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "Smugmug.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class UploadComponent  : public Component,
                         public Button::Listener,
                         public ComboBox::Listener
{
public:
    //==============================================================================
    UploadComponent (UploadDialog* ud_);
    ~UploadComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	OwnedArray<Category> categories;
	OwnedArray<SubCategory> subCategories;
	OwnedArray<Album> albums;
	UploadDialog* ud;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ToggleButton> addMode;
    std::unique_ptr<ToggleButton> createMode;
    std::unique_ptr<ComboBox> albumName;
    std::unique_ptr<TextEditor> newAlbumName;
    std::unique_ptr<Label> label;
    std::unique_ptr<Label> label2;
    std::unique_ptr<ComboBox> subCategory;
    std::unique_ptr<ComboBox> category;
    std::unique_ptr<TextButton> cancel;
    std::unique_ptr<TextButton> ok;
    std::unique_ptr<Label> label3;
    std::unique_ptr<TextEditor> description;
    std::unique_ptr<Label> label4;
    std::unique_ptr<TextEditor> keywords;
    std::unique_ptr<Label> label5;
    std::unique_ptr<ComboBox> sort;
    std::unique_ptr<ToggleButton> ascending;
    std::unique_ptr<ToggleButton> public_;
    std::unique_ptr<ToggleButton> filenames;
    std::unique_ptr<ToggleButton> comments;
    std::unique_ptr<ToggleButton> linking;
    std::unique_ptr<ToggleButton> exif;
    std::unique_ptr<ToggleButton> aesyshare;
    std::unique_ptr<ToggleButton> originals;
    std::unique_ptr<ToggleButton> header;
    std::unique_ptr<ToggleButton> large;
    std::unique_ptr<ToggleButton> clean;
    std::unique_ptr<ToggleButton> protect;
    std::unique_ptr<ToggleButton> watermark;
    std::unique_ptr<ToggleButton> smugsearch;
    std::unique_ptr<ToggleButton> worldsearch;
    std::unique_ptr<TextButton> addcat;
    std::unique_ptr<TextButton> addsubcat;
    std::unique_ptr<Label> label6;
    std::unique_ptr<TextEditor> password;
    std::unique_ptr<Label> label7;
    std::unique_ptr<TextEditor> hint;
    std::unique_ptr<ToggleButton> familyedit;
    std::unique_ptr<ToggleButton> friendedit;
    std::unique_ptr<ToggleButton> openbrowser;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
