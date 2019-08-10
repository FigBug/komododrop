#include "Settings.h"
#include "SettingsComponent.h"

juce_ImplementSingleton(Settings);

class SettingsDialog : public DialogWindow
{
public:
    SettingsDialog() : DialogWindow("Komodo Drop", LookAndFeel::getDefaultLookAndFeel().findColour(AlertWindow::backgroundColourId), true)
	{	
		setContentOwned(new SettingsComponent(), true);
		centreWithSize(getWidth(), getHeight());
	}

	~SettingsDialog()
	{
		exitModalState(0);
	}

	void closeButtonPressed()
	{
		delete this;
	}
};

Settings::Settings()
{
	PropertiesFile::Options opts;
	opts.applicationName			= "Komodo Drop";
	opts.folderName					= "Komodo Drop";
	opts.commonToAllUsers			= false;
	opts.filenameSuffix				= ".xml";
	opts.storageFormat				= PropertiesFile::storeAsXML;
	opts.ignoreCaseOfKeyNames		= false;
	opts.millisecondsBeforeSaving	= 1000;
	opts.osxLibrarySubFolder		= "Application Support";

    props = std::make_unique<PropertiesFile> (opts);

	accessToken     = props->getValue("accessToken");
	accessSecret    = props->getValue("accessSecret");
	silentUpload    = props->getBoolValue("silentUpload");
	lastCateory		= props->getIntValue("lastCategory", -1);
	lastSubCategory = props->getIntValue("lastSubCategory", -1);
	lastMode        = props->getIntValue("lastMode", -1);
	lastAlbum       = props->getIntValue("lastAlbum", -1);
	checkForDupes   = props->getBoolValue("checkForDupes", true);
	uploadThreads   = props->getIntValue("uploadThreads", 2);
	x               = props->getIntValue("x", -1);
	y				= props->getIntValue("y", -1);
	sort			= props->getIntValue("sort", 1);
	ascending		= props->getBoolValue("ascending", true);
    public_			= props->getBoolValue("public_", true);
    filenames		= props->getBoolValue("filenames", false);
    comments		= props->getBoolValue("comments", true);
    linking			= props->getBoolValue("linking", true);
    exif			= props->getBoolValue("exif", true);
    aesyshare		= props->getBoolValue("aesyshare", true);
    originals		= props->getBoolValue("originals", true);
    header			= props->getBoolValue("header", false);
    large			= props->getBoolValue("large", true);
    clean			= props->getBoolValue("clean", false);
    protect			= props->getBoolValue("protect", false);
    watermark		= props->getBoolValue("watermark", false);
    smugsearch		= props->getBoolValue("smugsearch", true);
    worldsearch		= props->getBoolValue("worldsearch", true);
    friendedit		= props->getBoolValue("friendedit", false);
    familyedit		= props->getBoolValue("familyedit", false);
	openBrowser     = props->getBoolValue("openBrowser", true);
}

Settings::~Settings()
{
	save();
    clearSingletonInstance();
}

void Settings::save()
{
	props->setValue("accessSecret", accessSecret);
	props->setValue("accessToken", accessToken);
	props->setValue("silentUpload", silentUpload);
	props->setValue("lastCategory", lastCateory);
	props->setValue("lastSubCategory", lastSubCategory);
	props->setValue("lastMode", lastMode);
	props->setValue("lastAlbum", lastAlbum);
	props->setValue("checkForDupes", checkForDupes);
	props->setValue("uploadThreads", uploadThreads);
	props->setValue("x", x);
	props->setValue("y", y);
	props->setValue("sort", sort);
	props->setValue("ascending", ascending);
	props->setValue("public_", public_);
	props->setValue("filenames", filenames);
	props->setValue("comments", comments);
	props->setValue("linking", linking);
	props->setValue("exif", exif);
	props->setValue("aesyshare", aesyshare);
	props->setValue("originals", originals);
	props->setValue("header", header);
	props->setValue("large", large);
	props->setValue("clean", clean);
	props->setValue("protect", protect);
	props->setValue("watermark", watermark);
	props->setValue("smugsearch", smugsearch);
	props->setValue("worldsearch", worldsearch);
    props->setValue("friendedit", friendedit);
    props->setValue("familyedit", familyedit);
	props->setValue("openBrowser", openBrowser);

	props->saveIfNeeded();
}

void Settings::showSettingsDialog()
{
	SettingsDialog* d = new SettingsDialog();
	d->runModalLoop();	
}
