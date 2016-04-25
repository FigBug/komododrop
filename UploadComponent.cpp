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

//[Headers] You can add your own extra header files here...
#include "UploadWindow.h"
#include "Settings.h"
//[/Headers]

#include "UploadComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
UploadComponent::UploadComponent (UploadDialog* ud_)
    : addMode (0),
      createMode (0),
      albumName (0),
      newAlbumName (0),
      label (0),
      label2 (0),
      subCategory (0),
      category (0),
      cancel (0),
      ok (0),
      label3 (0),
      description (0),
      label4 (0),
      keywords (0),
      label5 (0),
      sort (0),
      ascending (0),
      public_ (0),
      filenames (0),
      comments (0),
      linking (0),
      exif (0),
      aesyshare (0),
      originals (0),
      header (0),
      large (0),
      clean (0),
      protect (0),
      watermark (0),
      smugsearch (0),
      worldsearch (0),
      addcat (0),
      addsubcat (0),
      label6 (0),
      password (0),
      label7 (0),
      hint (0),
      familyedit (0),
      friendedit (0),
      openbrowser (0)
{
    addAndMakeVisible (addMode = new ToggleButton ("addMode"));
    addMode->setButtonText ("add to album:");
    addMode->setRadioGroupId (1);
    addMode->addListener (this);

    addAndMakeVisible (createMode = new ToggleButton ("createMode"));
    createMode->setButtonText ("create album:");
    createMode->setRadioGroupId (1);
    createMode->addListener (this);

    addAndMakeVisible (albumName = new ComboBox ("albumName"));
    albumName->setEditableText (false);
    albumName->setJustificationType (Justification::centredLeft);
    albumName->setTextWhenNothingSelected (String::empty);
    albumName->setTextWhenNoChoicesAvailable ("(no choices)");
    albumName->addListener (this);

    addAndMakeVisible (newAlbumName = new TextEditor ("newAlbumName"));
    newAlbumName->setMultiLine (false);
    newAlbumName->setReturnKeyStartsNewLine (false);
    newAlbumName->setReadOnly (false);
    newAlbumName->setScrollbarsShown (true);
    newAlbumName->setCaretVisible (true);
    newAlbumName->setPopupMenuEnabled (true);
    newAlbumName->setText (String::empty);

    addAndMakeVisible (label = new Label ("new label",
                                          "category:"));
    label->setFont (Font (15.0000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label2 = new Label ("new label",
                                           "sub category:"));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (subCategory = new ComboBox ("subCategory"));
    subCategory->setEditableText (false);
    subCategory->setJustificationType (Justification::centredLeft);
    subCategory->setTextWhenNothingSelected (String::empty);
    subCategory->setTextWhenNoChoicesAvailable ("(no choices)");
    subCategory->addListener (this);

    addAndMakeVisible (category = new ComboBox ("category"));
    category->setEditableText (false);
    category->setJustificationType (Justification::centredLeft);
    category->setTextWhenNothingSelected (String::empty);
    category->setTextWhenNoChoicesAvailable ("(no choices)");
    category->addListener (this);

    addAndMakeVisible (cancel = new TextButton ("cancel"));
    cancel->addListener (this);

    addAndMakeVisible (ok = new TextButton ("ok"));
    ok->addListener (this);

    addAndMakeVisible (label3 = new Label ("new label",
                                           "description:"));
    label3->setFont (Font (15.0000f, Font::plain));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (description = new TextEditor ("description"));
    description->setMultiLine (true);
    description->setReturnKeyStartsNewLine (false);
    description->setReadOnly (false);
    description->setScrollbarsShown (true);
    description->setCaretVisible (true);
    description->setPopupMenuEnabled (true);
    description->setText (String::empty);

    addAndMakeVisible (label4 = new Label ("new label",
                                           "keywords:"));
    label4->setFont (Font (15.0000f, Font::plain));
    label4->setJustificationType (Justification::centredLeft);
    label4->setEditable (false, false, false);
    label4->setColour (TextEditor::textColourId, Colours::black);
    label4->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (keywords = new TextEditor ("keywords"));
    keywords->setMultiLine (false);
    keywords->setReturnKeyStartsNewLine (false);
    keywords->setReadOnly (false);
    keywords->setScrollbarsShown (true);
    keywords->setCaretVisible (true);
    keywords->setPopupMenuEnabled (true);
    keywords->setText (String::empty);

    addAndMakeVisible (label5 = new Label ("new label",
                                           "sort:"));
    label5->setFont (Font (15.0000f, Font::plain));
    label5->setJustificationType (Justification::centredLeft);
    label5->setEditable (false, false, false);
    label5->setColour (TextEditor::textColourId, Colours::black);
    label5->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (sort = new ComboBox ("sort"));
    sort->setEditableText (false);
    sort->setJustificationType (Justification::centredLeft);
    sort->setTextWhenNothingSelected (String::empty);
    sort->setTextWhenNoChoicesAvailable ("(no choices)");
    sort->addListener (this);

    addAndMakeVisible (ascending = new ToggleButton ("ascending"));
    ascending->addListener (this);

    addAndMakeVisible (public_ = new ToggleButton ("public_"));
    public_->setButtonText ("public");
    public_->addListener (this);

    addAndMakeVisible (filenames = new ToggleButton ("filenames"));
    filenames->setButtonText ("show filenames");
    filenames->addListener (this);

    addAndMakeVisible (comments = new ToggleButton ("comments"));
    comments->setButtonText ("allow comments");
    comments->addListener (this);

    addAndMakeVisible (linking = new ToggleButton ("linking"));
    linking->setButtonText ("allow external linking");
    linking->addListener (this);

    addAndMakeVisible (exif = new ToggleButton ("exif"));
    exif->setButtonText ("show EXIF metadata");
    exif->addListener (this);

    addAndMakeVisible (aesyshare = new ToggleButton ("easyshare"));
    aesyshare->setButtonText ("allow easy share");
    aesyshare->addListener (this);

    addAndMakeVisible (originals = new ToggleButton ("originals"));
    originals->setButtonText ("allow originals");
    originals->addListener (this);

    addAndMakeVisible (header = new ToggleButton ("header"));
    header->setButtonText ("show custom header");
    header->addListener (this);

    addAndMakeVisible (large = new ToggleButton ("large"));
    large->setButtonText ("allow large");
    large->addListener (this);

    addAndMakeVisible (clean = new ToggleButton ("clean"));
    clean->setButtonText ("use clean style");
    clean->addListener (this);

    addAndMakeVisible (protect = new ToggleButton ("protect"));
    protect->setButtonText ("use heavy protection");
    protect->addListener (this);

    addAndMakeVisible (watermark = new ToggleButton ("watermark"));
    watermark->setButtonText ("apply watermarks");
    watermark->addListener (this);

    addAndMakeVisible (smugsearch = new ToggleButton ("smugsearch"));
    smugsearch->setButtonText ("smugmug searchable");
    smugsearch->addListener (this);

    addAndMakeVisible (worldsearch = new ToggleButton ("worldsearch"));
    worldsearch->setButtonText ("world searchable");
    worldsearch->addListener (this);

    addAndMakeVisible (addcat = new TextButton ("addcat"));
    addcat->setButtonText ("+");
    addcat->addListener (this);

    addAndMakeVisible (addsubcat = new TextButton ("addsubcat"));
    addsubcat->setButtonText ("+");
    addsubcat->addListener (this);

    addAndMakeVisible (label6 = new Label ("new label",
                                           "password:"));
    label6->setFont (Font (15.0000f, Font::plain));
    label6->setJustificationType (Justification::centredLeft);
    label6->setEditable (false, false, false);
    label6->setColour (TextEditor::textColourId, Colours::black);
    label6->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (password = new TextEditor ("password"));
    password->setMultiLine (false);
    password->setReturnKeyStartsNewLine (false);
    password->setReadOnly (false);
    password->setScrollbarsShown (true);
    password->setCaretVisible (true);
    password->setPopupMenuEnabled (true);
    password->setText (String::empty);

    addAndMakeVisible (label7 = new Label ("new label",
                                           "password hint:"));
    label7->setFont (Font (15.0000f, Font::plain));
    label7->setJustificationType (Justification::centredLeft);
    label7->setEditable (false, false, false);
    label7->setColour (TextEditor::textColourId, Colours::black);
    label7->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (hint = new TextEditor ("hint"));
    hint->setMultiLine (false);
    hint->setReturnKeyStartsNewLine (false);
    hint->setReadOnly (false);
    hint->setScrollbarsShown (true);
    hint->setCaretVisible (true);
    hint->setPopupMenuEnabled (true);
    hint->setText (String::empty);

    addAndMakeVisible (familyedit = new ToggleButton ("familyedit"));
    familyedit->setButtonText ("allow family edit");
    familyedit->addListener (this);

    addAndMakeVisible (friendedit = new ToggleButton ("friendedit"));
    friendedit->setButtonText ("allow friend edit");
    friendedit->addListener (this);

    addAndMakeVisible (openbrowser = new ToggleButton ("openbrowser"));
    openbrowser->setButtonText ("open in browser when finished");
    openbrowser->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (592, 478);


    //[Constructor] You can add your own custom stuff here..
	ud = ud_;

	ud->smugMug->getAlbumList(albums);
	ud->smugMug->getCategoryList(categories);
	ud->smugMug->getSubCategoryList(subCategories);

	Settings* settings = Settings::getInstance();

	for (int i = 0; i < albums.size(); i++)
	{
		albumName->addItem(albums[i]->getDisplayTitle(), i + 1);
		if (albums[i]->id.id == settings->lastAlbum)
			albumName->setSelectedId(i + 1);
	}
	if (albumName->getSelectedId() == 0)
		albumName->setSelectedItemIndex(0);

	for (int i = 0; i < categories.size(); i++)
		category->addItem(categories[i]->title, categories[i]->id + 1);
	category->setSelectedId(settings->lastCateory + 1);
	if (category->getSelectedId() == 0)
		category->setSelectedId(1);

	newAlbumName->setText(ud->defaultAlbumName);

	addMode->setRadioGroupId(1);
	createMode->setRadioGroupId(1);

	if (settings->lastMode == 1)
		addMode->setToggleState(true, sendNotification);
	else
		createMode->setToggleState(true, sendNotification);

	sort->addItem(("Position"), 1);
	sort->addItem(("Caption"), 2);
	sort->addItem(("FileName"), 3);
	sort->addItem(("Date"), 4);
	sort->addItem(("DateTime"), 5);
	sort->addItem(("DateTimeOriginal"), 6);
	sort->setSelectedId(1);
	ascending->setToggleState(settings->ascending, dontSendNotification);
	public_->setToggleState(settings->public_, dontSendNotification);
	filenames->setToggleState(settings->filenames, dontSendNotification);
	comments->setToggleState(settings->comments, dontSendNotification);
	linking->setToggleState(settings->linking, dontSendNotification);
	exif->setToggleState(settings->exif, dontSendNotification);
	aesyshare->setToggleState(settings->aesyshare, dontSendNotification);
	originals->setToggleState(settings->originals, dontSendNotification);
	header->setToggleState(settings->header, dontSendNotification);
	large->setToggleState(settings->large, dontSendNotification);
	clean->setToggleState(settings->clean, dontSendNotification);
	protect->setToggleState(settings->protect, dontSendNotification);
	watermark->setToggleState(settings->watermark, dontSendNotification);
	smugsearch->setToggleState(settings->smugsearch, dontSendNotification);
	worldsearch->setToggleState(settings->worldsearch, dontSendNotification);
	friendedit->setToggleState(settings->friendedit, dontSendNotification);
	familyedit->setToggleState(settings->familyedit, dontSendNotification);
	openbrowser->setToggleState(settings->openBrowser, dontSendNotification);

    //[/Constructor]
}

UploadComponent::~UploadComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (addMode);
    deleteAndZero (createMode);
    deleteAndZero (albumName);
    deleteAndZero (newAlbumName);
    deleteAndZero (label);
    deleteAndZero (label2);
    deleteAndZero (subCategory);
    deleteAndZero (category);
    deleteAndZero (cancel);
    deleteAndZero (ok);
    deleteAndZero (label3);
    deleteAndZero (description);
    deleteAndZero (label4);
    deleteAndZero (keywords);
    deleteAndZero (label5);
    deleteAndZero (sort);
    deleteAndZero (ascending);
    deleteAndZero (public_);
    deleteAndZero (filenames);
    deleteAndZero (comments);
    deleteAndZero (linking);
    deleteAndZero (exif);
    deleteAndZero (aesyshare);
    deleteAndZero (originals);
    deleteAndZero (header);
    deleteAndZero (large);
    deleteAndZero (clean);
    deleteAndZero (protect);
    deleteAndZero (watermark);
    deleteAndZero (smugsearch);
    deleteAndZero (worldsearch);
    deleteAndZero (addcat);
    deleteAndZero (addsubcat);
    deleteAndZero (label6);
    deleteAndZero (password);
    deleteAndZero (label7);
    deleteAndZero (hint);
    deleteAndZero (familyedit);
    deleteAndZero (friendedit);
    deleteAndZero (openbrowser);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void UploadComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    g.setColour (Colour (0x80dadada));
    g.fillRoundedRectangle (5.0f, 21.0f, 392.0f, 450.0f, 10.0000f);

    g.setColour (Colour (0x80000000));
    g.drawRoundedRectangle (5.0f, 21.0f, 392.0f, 450.0f, 10.0000f, 1.0000f);

    g.setColour (Colour (0x80dadada));
    g.fillRoundedRectangle (412.0f, 16.0f, 172.0f, 407.0f, 10.0000f);

    g.setColour (Colour (0x80000000));
    g.drawRoundedRectangle (412.0f, 16.0f, 172.0f, 407.0f, 10.0000f, 1.0000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UploadComponent::resized()
{
    addMode->setBounds (16, 24, 150, 24);
    createMode->setBounds (16, 64, 150, 24);
    albumName->setBounds (168, 24, 224, 24);
    newAlbumName->setBounds (168, 64, 224, 24);
    label->setBounds (64, 104, 88, 24);
    label2->setBounds (64, 144, 96, 24);
    subCategory->setBounds (168, 144, 176, 24);
    category->setBounds (168, 104, 176, 24);
    cancel->setBounds (504, 448, 80, 24);
    ok->setBounds (408, 448, 80, 24);
    label3->setBounds (64, 184, 150, 24);
    description->setBounds (168, 184, 224, 88);
    label4->setBounds (64, 288, 150, 24);
    keywords->setBounds (168, 288, 224, 24);
    label5->setBounds (64, 408, 150, 24);
    sort->setBounds (168, 408, 128, 24);
    ascending->setBounds (304, 408, 88, 24);
    public_->setBounds (416, 24, 150, 24);
    filenames->setBounds (416, 48, 150, 24);
    comments->setBounds (416, 72, 150, 24);
    linking->setBounds (416, 96, 150, 24);
    exif->setBounds (416, 120, 150, 24);
    aesyshare->setBounds (416, 144, 150, 24);
    originals->setBounds (416, 168, 150, 24);
    header->setBounds (416, 216, 150, 24);
    large->setBounds (416, 192, 150, 24);
    clean->setBounds (416, 240, 150, 24);
    protect->setBounds (416, 264, 150, 24);
    watermark->setBounds (416, 288, 150, 24);
    smugsearch->setBounds (416, 312, 150, 24);
    worldsearch->setBounds (416, 336, 150, 24);
    addcat->setBounds (352, 104, 40, 24);
    addsubcat->setBounds (352, 144, 40, 24);
    label6->setBounds (64, 328, 150, 24);
    password->setBounds (168, 328, 224, 24);
    label7->setBounds (64, 368, 150, 24);
    hint->setBounds (168, 368, 224, 24);
    familyedit->setBounds (416, 360, 150, 24);
    friendedit->setBounds (416, 384, 150, 24);
    openbrowser->setBounds (168, 440, 224, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UploadComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == addMode)
    {
        //[UserButtonCode_addMode] -- add your button handler code here..
		albumName->setEnabled(true);
		newAlbumName->setEnabled(false);
		category->setEnabled(false);
		subCategory->setEnabled(false);
		description->setEnabled(false);
		keywords->setEnabled(false);
		sort->setEnabled(false);
		ascending->setEnabled(false);
		public_->setEnabled(false);
		filenames->setEnabled(false);
		comments->setEnabled(false);
		linking->setEnabled(false);
		exif->setEnabled(false);
		aesyshare->setEnabled(false);
		originals->setEnabled(false);
		header->setEnabled(false);
		large->setEnabled(false);
		clean->setEnabled(false);
		protect->setEnabled(false);
		watermark->setEnabled(false);
		smugsearch->setEnabled(false);
		worldsearch->setEnabled(false);
		familyedit->setEnabled(false);
		friendedit->setEnabled(false);
        //[/UserButtonCode_addMode]
    }
    else if (buttonThatWasClicked == createMode)
    {
        //[UserButtonCode_createMode] -- add your button handler code here..
		albumName->setEnabled(false);
		newAlbumName->setEnabled(true);
		category->setEnabled(true);
		subCategory->setEnabled(true);
		description->setEnabled(true);
		keywords->setEnabled(true);
		sort->setEnabled(true);
		ascending->setEnabled(true);
		public_->setEnabled(true);
		filenames->setEnabled(true);
		comments->setEnabled(true);
		linking->setEnabled(true);
		exif->setEnabled(true);
		aesyshare->setEnabled(true);
		originals->setEnabled(true);
		header->setEnabled(true && ud->smugMug->isPower());
		large->setEnabled(true && ud->smugMug->isPro());
		clean->setEnabled(true && ud->smugMug->isPro());
		protect->setEnabled(true && ud->smugMug->isPro());
		watermark->setEnabled(true && ud->smugMug->isPro());
		smugsearch->setEnabled(true);
		worldsearch->setEnabled(true);
		familyedit->setEnabled(true);
		friendedit->setEnabled(true);
        //[/UserButtonCode_createMode]
    }
    else if (buttonThatWasClicked == cancel)
    {
        //[UserButtonCode_cancel] -- add your button handler code here..
		getParentComponent()->exitModalState(0);
		delete getParentComponent();
        //[/UserButtonCode_cancel]
    }
    else if (buttonThatWasClicked == ok)
    {
        //[UserButtonCode_ok] -- add your button handler code here..
		Settings* settings = Settings::getInstance();

		ud->open     = openbrowser->getToggleState();
		settings->openBrowser = ud->open;

		if (addMode->getToggleState())
		{
			ud->newAlbum = false;
			ud->albumId  = albums[albumName->getSelectedId() - 1]->id;
			ud->title    = albums[albumName->getSelectedId() - 1]->title;

			settings->lastMode  = 1;
			settings->lastAlbum = albums[albumName->getSelectedId() - 1]->id.id;
		}
		else
		{
			ud->newAlbum = true;
			ud->title  = newAlbumName->getText();
			ud->catId  = category->getSelectedId() - 1;

			int subCat = subCategory->getSelectedId();
			if (subCat > 1)
			{
				ud->params.set(("SubCategoryID"), String(subCat - 1));
			}

			if (description->getText().isNotEmpty())
				ud->params.set(("Description"), description->getText());

			if (keywords->getText().isNotEmpty())
				ud->params.set(("Keywords"), keywords->getText());

			if (password->getText().isNotEmpty())
				ud->params.set(("Password"), password->getText());

			if (password->getText().isNotEmpty() && hint->getText().isNotEmpty())
				ud->params.set(("PasswordHint"), hint->getText());

			ud->params.set(("SortMethod"), sort->getText());
			ud->params.set(("SortDirection"), ascending->getToggleState() ? ("0") : ("1"));
			ud->params.set(("Public"), public_->getToggleState() ? ("1") : ("0"));
			ud->params.set(("Filenames"), filenames->getToggleState() ? ("1") : ("0"));
			ud->params.set(("Comments"), comments->getToggleState() ? ("1") : ("0"));
			ud->params.set(("External"), linking->getToggleState() ? ("1") : ("0"));
			ud->params.set(("EXIF"), exif->getToggleState() ? ("1") : ("0"));
			ud->params.set(("Share"), aesyshare->getToggleState() ? ("1") : ("0"));
			ud->params.set(("Originals"), originals->getToggleState() ? ("1") : ("0"));

			if (ud->smugMug->isPower())
			{
				ud->params.set(("Header"), header->getToggleState() ? ("1") : ("0"));
			}
			if (ud->smugMug->isPro())
			{
				ud->params.set(("Larges"), large->getToggleState() ? ("1") : ("0"));
				ud->params.set(("Clean"), clean->getToggleState() ? ("1") : ("0"));
				ud->params.set(("Protected"), protect->getToggleState() ? ("1") : ("0"));
				ud->params.set(("Watermarking"), watermark->getToggleState() ? ("1") : ("0"));
			}

			ud->params.set(("SmugSearchable"), smugsearch->getToggleState() ? ("1") : ("0"));
			ud->params.set(("WorldSearchable"), worldsearch->getToggleState() ? ("1") : ("0"));
			ud->params.set(("FamilyEdit"), familyedit->getToggleState() ? ("1") : ("0"));
			ud->params.set(("FriendEdit"), friendedit->getToggleState() ? ("1") : ("0"));

			settings->lastMode			= 2;
			settings->lastCateory		= category->getSelectedId() - 1;
			settings->lastSubCategory	= subCat - 1;
			settings->sort				= sort->getSelectedId();
			settings->ascending			= ascending->getToggleState();
			settings->public_			= public_->getToggleState();
			settings->filenames			= filenames->getToggleState();
			settings->comments			= comments->getToggleState();
			settings->linking			= linking->getToggleState();
			settings->exif				= exif->getToggleState();
			settings->aesyshare			= aesyshare->getToggleState();
			settings->originals			= originals->getToggleState();
			settings->header			= header->getToggleState();
			settings->large				= large->getToggleState();
			settings->clean				= clean->getToggleState();
			settings->protect			= protect->getToggleState();
			settings->watermark			= watermark->getToggleState();
			settings->smugsearch		= smugsearch->getToggleState();
			settings->worldsearch		= worldsearch->getToggleState();
			settings->familyedit        = familyedit->getToggleState();
			settings->friendedit        = friendedit->getToggleState();
		}
		settings->save();

		getParentComponent()->exitModalState(1);
		delete getParentComponent();
        //[/UserButtonCode_ok]
    }
    else if (buttonThatWasClicked == ascending)
    {
        //[UserButtonCode_ascending] -- add your button handler code here..
        //[/UserButtonCode_ascending]
    }
    else if (buttonThatWasClicked == public_)
    {
        //[UserButtonCode_public_] -- add your button handler code here..
        //[/UserButtonCode_public_]
    }
    else if (buttonThatWasClicked == filenames)
    {
        //[UserButtonCode_filenames] -- add your button handler code here..
        //[/UserButtonCode_filenames]
    }
    else if (buttonThatWasClicked == comments)
    {
        //[UserButtonCode_comments] -- add your button handler code here..
        //[/UserButtonCode_comments]
    }
    else if (buttonThatWasClicked == linking)
    {
        //[UserButtonCode_linking] -- add your button handler code here..
        //[/UserButtonCode_linking]
    }
    else if (buttonThatWasClicked == exif)
    {
        //[UserButtonCode_exif] -- add your button handler code here..
        //[/UserButtonCode_exif]
    }
    else if (buttonThatWasClicked == aesyshare)
    {
        //[UserButtonCode_aesyshare] -- add your button handler code here..
        //[/UserButtonCode_aesyshare]
    }
    else if (buttonThatWasClicked == originals)
    {
        //[UserButtonCode_originals] -- add your button handler code here..
        //[/UserButtonCode_originals]
    }
    else if (buttonThatWasClicked == header)
    {
        //[UserButtonCode_header] -- add your button handler code here..
        //[/UserButtonCode_header]
    }
    else if (buttonThatWasClicked == large)
    {
        //[UserButtonCode_large] -- add your button handler code here..
        //[/UserButtonCode_large]
    }
    else if (buttonThatWasClicked == clean)
    {
        //[UserButtonCode_clean] -- add your button handler code here..
        //[/UserButtonCode_clean]
    }
    else if (buttonThatWasClicked == protect)
    {
        //[UserButtonCode_protect] -- add your button handler code here..
        //[/UserButtonCode_protect]
    }
    else if (buttonThatWasClicked == watermark)
    {
        //[UserButtonCode_watermark] -- add your button handler code here..
        //[/UserButtonCode_watermark]
    }
    else if (buttonThatWasClicked == smugsearch)
    {
        //[UserButtonCode_smugsearch] -- add your button handler code here..
        //[/UserButtonCode_smugsearch]
    }
    else if (buttonThatWasClicked == worldsearch)
    {
        //[UserButtonCode_worldsearch] -- add your button handler code here..
        //[/UserButtonCode_worldsearch]
    }
    else if (buttonThatWasClicked == addcat)
    {
        //[UserButtonCode_addcat] -- add your button handler code here..
		AlertWindow aw(("Komodo Drop"), ("Create category:"), AlertWindow::InfoIcon);
		aw.addTextEditor(("name"), String::empty, ("category name:"));
		aw.addButton(("ok"), 1);
		aw.addButton(("cancel"), 2);

		if (aw.runModalLoop() == 1 && aw.getTextEditorContents(("name")).isNotEmpty())
		{
			int id = ud->smugMug->createCategory(aw.getTextEditorContents(("name")));
			if (id > 0)
			{
				category->addItem(aw.getTextEditorContents(("name")), id + 1);
				category->setSelectedId(id + 1);
			}
		}
        //[/UserButtonCode_addcat]
    }
    else if (buttonThatWasClicked == addsubcat)
    {
        //[UserButtonCode_addsubcat] -- add your button handler code here..
		AlertWindow aw(("Komodo Drop"), ("Create sub category:"), AlertWindow::InfoIcon);
		aw.addTextEditor(("name"), String::empty, ("sub category name:"));
		aw.addButton(("ok"), 1);
		aw.addButton(("cancel"), 2);

		if (aw.runModalLoop() == 1 && aw.getTextEditorContents(("name")).isNotEmpty())
		{
			int id = ud->smugMug->createSubCategory(category->getSelectedId() - 1, aw.getTextEditorContents(("name")));
			if (id > 0)
			{
				subCategory->addItem(aw.getTextEditorContents(("name")), id + 1);
				subCategory->setSelectedId(id + 1);
			}
		}
        //[/UserButtonCode_addsubcat]
    }
    else if (buttonThatWasClicked == familyedit)
    {
        //[UserButtonCode_familyedit] -- add your button handler code here..
        //[/UserButtonCode_familyedit]
    }
    else if (buttonThatWasClicked == friendedit)
    {
        //[UserButtonCode_friendedit] -- add your button handler code here..
        //[/UserButtonCode_friendedit]
    }
    else if (buttonThatWasClicked == openbrowser)
    {
        //[UserButtonCode_openbrowser] -- add your button handler code here..
        //[/UserButtonCode_openbrowser]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void UploadComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == albumName)
    {
        //[UserComboBoxCode_albumName] -- add your combo box handling code here..
        //[/UserComboBoxCode_albumName]
    }
    else if (comboBoxThatHasChanged == subCategory)
    {
        //[UserComboBoxCode_subCategory] -- add your combo box handling code here..
        //[/UserComboBoxCode_subCategory]
    }
    else if (comboBoxThatHasChanged == category)
    {
        //[UserComboBoxCode_category] -- add your combo box handling code here..
		subCategory->clear();
		subCategory->addItem(("<none>"), 1);
		for (int i = 0; i < subCategories.size(); i++)
		{
			if (subCategories[i]->parentId + 1 == category->getSelectedId())
				subCategory->addItem(subCategories[i]->title, subCategories[i]->id + 1);
		}
		subCategory->setSelectedItemIndex(0);
		subCategory->setSelectedId(Settings::getInstance()->lastSubCategory + 1);

        //[/UserComboBoxCode_category]
    }
    else if (comboBoxThatHasChanged == sort)
    {
        //[UserComboBoxCode_sort] -- add your combo box handling code here..
        //[/UserComboBoxCode_sort]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UploadComponent" componentName=""
                 parentClasses="public Component" constructorParams="UploadDialog* ud_"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="0" initialWidth="592"
                 initialHeight="478">
  <BACKGROUND backgroundColour="ffffffff">
    <ROUNDRECT pos="5 21 392 450" cornerSize="10" fill="solid: 80dadada" hasStroke="1"
               stroke="1, mitered, butt" strokeColour="solid: 80000000"/>
    <ROUNDRECT pos="412 16 172 407" cornerSize="10" fill="solid: 80dadada" hasStroke="1"
               stroke="1, mitered, butt" strokeColour="solid: 80000000"/>
  </BACKGROUND>
  <TOGGLEBUTTON name="addMode" id="d4faa3f1b68ea451" memberName="addMode" virtualName=""
                explicitFocusOrder="0" pos="16 24 150 24" buttonText="add to album:"
                connectedEdges="0" needsCallback="1" radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="createMode" id="428cfea5acd2e0dd" memberName="createMode"
                virtualName="" explicitFocusOrder="0" pos="16 64 150 24" buttonText="create album:"
                connectedEdges="0" needsCallback="1" radioGroupId="1" state="0"/>
  <COMBOBOX name="albumName" id="454601418612d670" memberName="albumName"
            virtualName="" explicitFocusOrder="0" pos="168 24 224 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TEXTEDITOR name="newAlbumName" id="2118c8c7b67a24e1" memberName="newAlbumName"
              virtualName="" explicitFocusOrder="0" pos="168 64 224 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="4b579a12b85fac9b" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="64 104 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="category:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="ef5a9f17c7ff3c93" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="64 144 96 24" edTextCol="ff000000"
         edBkgCol="0" labelText="sub category:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="subCategory" id="c17eb46e944bab75" memberName="subCategory"
            virtualName="" explicitFocusOrder="0" pos="168 144 176 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="category" id="3f050b10752cd020" memberName="category" virtualName=""
            explicitFocusOrder="0" pos="168 104 176 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="cancel" id="d2a4a262a4c00d55" memberName="cancel" virtualName=""
              explicitFocusOrder="0" pos="504 448 80 24" buttonText="cancel"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="ok" id="132ade72af6964aa" memberName="ok" virtualName=""
              explicitFocusOrder="0" pos="408 448 80 24" buttonText="ok" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="13c6003965fe295" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="64 184 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="description:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="description" id="5689d9400c0ea41b" memberName="description"
              virtualName="" explicitFocusOrder="0" pos="168 184 224 88" initialText=""
              multiline="1" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="a61c62975c991cca" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="64 288 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="keywords:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="keywords" id="cbd6469352ebe77c" memberName="keywords" virtualName=""
              explicitFocusOrder="0" pos="168 288 224 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <LABEL name="new label" id="b5558a4d2c805afc" memberName="label5" virtualName=""
         explicitFocusOrder="0" pos="64 408 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="sort:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <COMBOBOX name="sort" id="c43f84b9b6f170ca" memberName="sort" virtualName=""
            explicitFocusOrder="0" pos="168 408 128 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="ascending" id="70de173c4e31d41f" memberName="ascending"
                virtualName="" explicitFocusOrder="0" pos="304 408 88 24" buttonText="ascending"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="public_" id="f3ff5b803e655d2e" memberName="public_" virtualName=""
                explicitFocusOrder="0" pos="416 24 150 24" buttonText="public"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="filenames" id="fcc1da8cc3c07cac" memberName="filenames"
                virtualName="" explicitFocusOrder="0" pos="416 48 150 24" buttonText="show filenames"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="comments" id="4f04a10ec7f45b92" memberName="comments" virtualName=""
                explicitFocusOrder="0" pos="416 72 150 24" buttonText="allow comments"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="linking" id="6ad3c4fb27d6b321" memberName="linking" virtualName=""
                explicitFocusOrder="0" pos="416 96 150 24" buttonText="allow external linking"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="exif" id="b19f30599960a126" memberName="exif" virtualName=""
                explicitFocusOrder="0" pos="416 120 150 24" buttonText="show EXIF metadata"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="easyshare" id="9c92c55b9a0ffc7f" memberName="aesyshare"
                virtualName="" explicitFocusOrder="0" pos="416 144 150 24" buttonText="allow easy share"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="originals" id="8d442fae446aeddd" memberName="originals"
                virtualName="" explicitFocusOrder="0" pos="416 168 150 24" buttonText="allow originals"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="header" id="305bf711b11dd0ca" memberName="header" virtualName=""
                explicitFocusOrder="0" pos="416 216 150 24" buttonText="show custom header"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="large" id="c624a72b0927a0ed" memberName="large" virtualName=""
                explicitFocusOrder="0" pos="416 192 150 24" buttonText="allow large"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="clean" id="784420989b09778d" memberName="clean" virtualName=""
                explicitFocusOrder="0" pos="416 240 150 24" buttonText="use clean style"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="protect" id="b906744d131dd53" memberName="protect" virtualName=""
                explicitFocusOrder="0" pos="416 264 150 24" buttonText="use heavy protection"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="watermark" id="2a3281fc8c24f453" memberName="watermark"
                virtualName="" explicitFocusOrder="0" pos="416 288 150 24" buttonText="apply watermarks"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="smugsearch" id="7a5f3f411b6ebe4d" memberName="smugsearch"
                virtualName="" explicitFocusOrder="0" pos="416 312 150 24" buttonText="smugmug searchable"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="worldsearch" id="1216f3605d7d032f" memberName="worldsearch"
                virtualName="" explicitFocusOrder="0" pos="416 336 150 24" buttonText="world searchable"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TEXTBUTTON name="addcat" id="520dd89cc5dd09b6" memberName="addcat" virtualName=""
              explicitFocusOrder="0" pos="352 104 40 24" buttonText="+" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="addsubcat" id="9614904516b858c5" memberName="addsubcat"
              virtualName="" explicitFocusOrder="0" pos="352 144 40 24" buttonText="+"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="b63e1554e0c1ddcd" memberName="label6" virtualName=""
         explicitFocusOrder="0" pos="64 328 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="password:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="password" id="63e101965dfea7d" memberName="password" virtualName=""
              explicitFocusOrder="0" pos="168 328 224 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <LABEL name="new label" id="816a682db4bae01d" memberName="label7" virtualName=""
         explicitFocusOrder="0" pos="64 368 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="password hint:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="hint" id="be1ad1ab974eef02" memberName="hint" virtualName=""
              explicitFocusOrder="0" pos="168 368 224 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <TOGGLEBUTTON name="familyedit" id="eec8c183d9742cf3" memberName="familyedit"
                virtualName="" explicitFocusOrder="0" pos="416 360 150 24" buttonText="allow family edit"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="friendedit" id="41c8df6c4c25520f" memberName="friendedit"
                virtualName="" explicitFocusOrder="0" pos="416 384 150 24" buttonText="allow friend edit"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="openbrowser" id="fd52319b07adfed" memberName="openbrowser"
                virtualName="" explicitFocusOrder="0" pos="168 440 224 24" buttonText="open in browser when finished"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[EndFile] You can add extra defines here...
//[/EndFile]
