#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Settings : public DeletedAtShutdown
{
public:
	Settings();
	~Settings();

	void save();

	void showSettingsDialog();

	String email;
	String password;

	bool silentUpload;
	bool checkForDupes;

	int lastMode;
	int lastCateory;
	int lastSubCategory;
	int lastAlbum;
	bool openBrowser;

	int uploadThreads;

	int x;
	int y;

	int sort;
	bool ascending;
    bool public_;
    bool filenames;
    bool comments;
    bool linking;
    bool exif;
    bool aesyshare;
    bool originals;
    bool header;
    bool large;
    bool clean;
    bool protect;
    bool watermark;
    bool smugsearch;
    bool worldsearch;
	bool friendedit;
	bool familyedit;

	juce_DeclareSingleton(Settings, true);
private:
	PropertiesFile* props;
};
