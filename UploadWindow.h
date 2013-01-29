#pragma once

#include "Smugmug.h"

class UploadDialog : public DialogWindow
{
public:
	UploadDialog(SmugMug* smugMug_, String defaultAlbumName_, bool& newAlbum_, String& title_, SmugID& albumId_, int& catId_, StringPairArray& params_, bool& open_);
	~UploadDialog();
	void closeButtonPressed();

private:
	friend class UploadComponent;

	bool& newAlbum; 
	String& title; 
	SmugID& albumId;
	int& catId;
	bool& open;
	StringPairArray& params;
	String defaultAlbumName;
	SmugMug* smugMug;
};

class UploadWindow : public Component,
	                 public FileDragAndDropTarget,
	                 public Timer,
					 public MenuBarModel
{
public:
	UploadWindow();
	~UploadWindow();

	void paint(Graphics& g);
	void userTriedToCloseWindow();
	void mouseDown(const MouseEvent& e);
	void mouseDrag(const MouseEvent& e);
	void mouseUp(const MouseEvent& e);    
	void timerCallback();

    bool isInterestedInFileDrag(const StringArray& files);
    void filesDropped(const StringArray& files, int x, int y);
	
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);	
	
	juce_UseDebuggingNewOperator;
private:
	void showMenu();
	void handleCommand(int cmd);
	PopupMenu createMenu();

	SmugMug smugMug;
	Component* trayIcon;
	ComponentDragger dragger;
	bool drag;
};
