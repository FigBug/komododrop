#pragma once

#include <JuceHeader.h>
#include "Smugmug.h"

struct UploadDialogContext;

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

	/** Runs `fn` once there's a SmugMug session, doing nothing if the user
	    cancels or authorization fails. */
	void withLogin (std::function<void()> fn);

	/** Menu items 11/12/13/19 — each picks files or a folder, then uploads. */
	void chooseFilesToUpload (int mode);

	/** Post-login half of filesDropped(): picks the destination album, either
	    silently or via the upload dialog. Takes ownership of `ur`. */
	void chooseUploadDestination (UploadRequest* ur, const String& defaultAlbumName);

	/** Hands `ur` to SmugMug, or discards it if there's no album to upload to. */
	void startUpload (UploadRequest* ur, SmugID albumId, bool openBrowser);

	SmugMug smugMug;
	Component* trayIcon = nullptr;
	ComponentDragger dragger;
	bool drag = false;

	// FileChooser::launchAsync needs the chooser to outlive the call.
	std::unique_ptr<FileChooser> fileChooser;
};
