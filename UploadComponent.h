/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  29 Jan 2013 12:04:58pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_UPLOADCOMPONENT_UPLOADCOMPONENT_4A2BDB32__
#define __JUCER_HEADER_UPLOADCOMPONENT_UPLOADCOMPONENT_4A2BDB32__

//[Headers]     -- You can add your own extra header files here --
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
                         public ButtonListener,
                         public ComboBoxListener
{
public:
    //==============================================================================
    UploadComponent (UploadDialog* ud_);
    ~UploadComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	OwnedArray<Category> categories;
	OwnedArray<SubCategory> subCategories;
	OwnedArray<Album> albums;
	UploadDialog* ud;
    //[/UserVariables]

    //==============================================================================
    ToggleButton* addMode;
    ToggleButton* createMode;
    ComboBox* albumName;
    TextEditor* newAlbumName;
    Label* label;
    Label* label2;
    ComboBox* subCategory;
    ComboBox* category;
    TextButton* cancel;
    TextButton* ok;
    Label* label3;
    TextEditor* description;
    Label* label4;
    TextEditor* keywords;
    Label* label5;
    ComboBox* sort;
    ToggleButton* ascending;
    ToggleButton* public_;
    ToggleButton* filenames;
    ToggleButton* comments;
    ToggleButton* linking;
    ToggleButton* exif;
    ToggleButton* aesyshare;
    ToggleButton* originals;
    ToggleButton* header;
    ToggleButton* large;
    ToggleButton* clean;
    ToggleButton* protect;
    ToggleButton* watermark;
    ToggleButton* smugsearch;
    ToggleButton* worldsearch;
    TextButton* addcat;
    TextButton* addsubcat;
    Label* label6;
    TextEditor* password;
    Label* label7;
    TextEditor* hint;
    ToggleButton* familyedit;
    ToggleButton* friendedit;
    ToggleButton* openbrowser;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UploadComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCER_HEADER_UPLOADCOMPONENT_UPLOADCOMPONENT_4A2BDB32__
