#include "UploadWindow.h"
#include "Settings.h"
#include "UploadComponent.h"

#if defined (JUCE_WIN32) || defined (JUCE_LINUX)
class TrayIcon : public SystemTrayIconComponent
{
public:
	TrayIcon(UploadWindow* uploadWindow_) : uploadWindow(uploadWindow_)
	{
		Image img(Image::ARGB, 64, 64, true);
		Graphics g(img);
		g.fillAll(Colours::transparentBlack);
		int cx = 32;
		int cy = 32;
		const float r = 10.0f;
		for (int i = 3; i >= 0; i--)
		{
			g.setColour((i % 2 != 0) ? Colours::red : Colours::white);
			g.fillEllipse(cx - (r * i), cy - (r * i), (r * i) * 2, (r * i) * 2);
		}

		Image icon(Image::ARGB, 16, 16, true);
		Graphics g2(icon);
		g2.fillAll(Colours::transparentBlack);
		g2.drawImage(img, 0, 0, 16, 16, 0, 0, 64, 64, false);

		setIconImage(icon);
		setIconTooltip(JUCEApplication::getInstance()->getApplicationName());
	}

	~TrayIcon()
	{
	}

	void mouseDown(const MouseEvent& e)
	{
		uploadWindow->setVisible(!uploadWindow->isVisible());
	}
private:
	UploadWindow* uploadWindow;
};
#endif

UploadDialog::UploadDialog(SmugMug* smugMug_, String defaultAlbumName_, bool& newAlbum_, String& title_, SmugID& albumId_, int& catId_, StringPairArray& params_, bool& open_)
  : DialogWindow("Komodo Drop", LookAndFeel::getDefaultLookAndFeel().findColour(AlertWindow::backgroundColourId), true),
    newAlbum(newAlbum_), 
	title(title_), 
	albumId(albumId_), 
	catId(catId_),
	open(open_),
    params(params_),
    defaultAlbumName(defaultAlbumName_),
    smugMug(smugMug_)
{
	setContentOwned(new UploadComponent(this), true);
	centreWithSize(getWidth(), getHeight());
}

UploadDialog::~UploadDialog()
{
}

void UploadDialog::closeButtonPressed()
{
	exitModalState(0);
	delete this;
}

UploadWindow::UploadWindow()
{
	setSize(96, 96);
	setOpaque(true);
	addToDesktop(0);
	setAlwaysOnTop(true);

	juce::Rectangle<int> rc = Desktop::getInstance().getDisplays().getMainDisplay().userArea;

	Settings* settings = Settings::getInstance();
	if (settings->x == -1 || settings->y == -1 || !Desktop::getInstance().getDisplays().getTotalBounds(true).contains(settings->x, settings->y))
		setTopLeftPosition(rc.getRight() - 96 - 7, rc.getBottom() - 96 - 7);
	else
		setTopLeftPosition(settings->x, settings->y);

	setVisible(true);
	
#if defined (JUCE_WIN32) || defined (JUCE_LINUX)
	trayIcon = new TrayIcon(this);
#endif
#ifdef JUCE_MAC
	MenuBarModel::setMacMainMenu(this);
#endif
}

UploadWindow::~UploadWindow()
{
#ifdef JUCE_MAC
	MenuBarModel::setMacMainMenu(NULL);
#endif
#if defined (JUCE_WIN32) || defined (JUCE_LINUX)	
	delete trayIcon;
#endif
	deleteAllChildren();
}

void UploadWindow::paint(Graphics& g)
{
	g.fillAll(Colours::black);

	Colour border = Colours::wheat;
	juce::Rectangle<int> rc(0, 0, getWidth(), getHeight());
	for (int i = 0; i < 4; i++)
	{
		g.setColour(i == 0 ? Colours::black : border);
		g.drawRect(rc.getX(), rc.getY(), rc.getWidth(), rc.getHeight());
		rc.reduce(1, 1);
		border = border.brighter(0.4f);
	}
	
	ColourGradient gf(Colours::red, 0, getHeight()/2.0f, Colours::darkred, float(getWidth()), getHeight()/2.0f, false);
	FillType ft(gf);

	int cx = getWidth() / 2;
	int cy = getHeight() / 2;
	const float r = 12.0f;
	for (int i = 3; i >= 0; i--)
	{
		if (i % 2 != 0)
			g.setFillType(ft);
		else
			g.setColour(Colours::white);
		
		g.fillEllipse(cx - (r * i), cy - (r * i), (r * i) * 2, (r * i) * 2);
	}
	g.setFillType(Colours::transparentWhite);

	if (smugMug.isUploading())
	{
		int64 n = smugMug.getTotalbytesUploaded();
		int64 d = smugMug.getTotalBytesToUpload();
		double percent = (d == 0) ? 0 : (double(n)/double(d)*100);
		

		GlyphArrangement glyphs;
		glyphs.addLineOfText(Font(25.0f, Font::bold), String(percent, 1) + ("%"), 0, 0);

		Path p;
		glyphs.createPath(p);

		juce::Rectangle<float> bounds = p.getBounds();
		float cx = getWidth() / 2.0f - bounds.getWidth() / 2.0f - bounds.getX();
		float cy = getHeight() / 2.0f - bounds.getHeight() / 2.0f - bounds.getY();

		AffineTransform trans = AffineTransform::translation(cx, cy);
		g.setColour(Colours::black);
		g.fillPath(p, trans);
		g.setColour(Colours::white);
		g.strokePath(p, PathStrokeType(1), trans);
	}
}

void UploadWindow::userTriedToCloseWindow()
{
	JUCEApplication::quit();
}

void UploadWindow::mouseDrag(const MouseEvent& e)
{
	if (!drag)
		dragger.startDraggingComponent(this, e);

	drag = true;
	dragger.dragComponent(this, e, NULL);
}

void UploadWindow::mouseDown(const MouseEvent& e)
{
	drag = false;
	if (e.mods.isRightButtonDown())
		showMenu();
}

void UploadWindow::mouseUp(const MouseEvent& e)
{
	if (e.mods.isLeftButtonDown() && !drag)
		showMenu();

	if (drag)
	{
		Settings* settings = Settings::getInstance();
		settings->x = getX();
		settings->y = getY();
		settings->save();
	}
}

PopupMenu UploadWindow::createMenu()
{	
	PopupMenu sub1;
	sub1.addItem(8, "Create category");
	sub1.addItem(9, "Create sub category");
	sub1.addSeparator();
	sub1.addItem(5, "Delete album");
	sub1.addItem(6, "Delete category");
	sub1.addItem(7, "Delete sub category");
	sub1.addSeparator();
	sub1.addItem(16, "Delete duplicate images");

	PopupMenu sub2;
	sub2.addItem(11, "Upload files");
	sub2.addItem(12, "Upload files in folder");
	sub2.addItem(13, "Upload files in folder and sub folders");
	sub2.addItem(19, "Upload folders in folder");

	PopupMenu sub3;
	sub3.addItem(17, "Show Queue");
	sub3.addItem(14, "Show log");
	sub3.addItem(15, "Clear log");
	sub3.addItem(18, "Show top photos");

	PopupMenu m;
	
	m.addItem(3, "About");
	m.addSeparator();
	m.addSubMenu("Manage", sub1);
	m.addSubMenu("Upload", sub2);
	m.addSubMenu("Status", sub3);
	m.addItem(10, "Cancel uploads", smugMug.isUploading());
	m.addItem(2, "Settings");

	if (isVisible())
		m.addItem(4, "Hide");
	else
		m.addItem(4, "Show");

	m.addSeparator();
	m.addItem(1, "Quit");
	
	return m;
}

void UploadWindow::showMenu()
{
	PopupMenu m = createMenu();
	int res = m.show();
	handleCommand(res);
}

void UploadWindow::handleCommand(int res)
{
	if (res == 1)
	{
		if (!smugMug.isUploading() || AlertWindow::showOkCancelBox(AlertWindow::InfoIcon, "Komodo Drop", 
			"There is an upload in progress, quit anyway?") == 1)
		{
			JUCEApplication::quit();
		}
	}
	else if (res == 2)
	{
		Settings::getInstance()->showSettingsDialog();
	}
	else if (res == 3)
	{
		AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Komodo Drop " + JUCEApplication::getInstance()->getApplicationVersion(), 
			"Created By: Roland Rabien (roland@rabien.com)\nBased on JUCE (www.rawmaterialsoftware.com)");
	}
	else if (res == 4)
	{
		setVisible(!isVisible());
	}
	else if (res == 5)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		OwnedArray<Album> albums;
		smugMug.getAlbumList(albums);
		if (albums.size() > 0)
		{
			ComboBox* album = new ComboBox("");
			album->setSize(300, 26);
			album->setVisible(true);

			for (int i = 0; i < albums.size(); i++)
				album->addItem(albums[i]->getDisplayTitle(), i + 1);
			album->setSelectedItemIndex(0);
			
			AlertWindow aw("Komodo Drop", "Delete album:", AlertWindow::InfoIcon);
			aw.addCustomComponent(album);
			aw.addButton("ok", 1);
			aw.addButton("cancel", 2);

			if (aw.runModalLoop() == 1)
			{
				smugMug.deleteAlbum(albums[album->getSelectedId() - 1]->id);
			}
			delete album;
		}
	}
	else if (res == 6)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		OwnedArray<Category> categories;
		smugMug.getCategoryList(categories);
		if (categories.size())
		{
			ComboBox* cats = new ComboBox("");
			cats->setSize(300, 26);
			cats->setVisible(true);

			for (int i = 0; i < categories.size(); i++)
				cats->addItem(categories[i]->title, categories[i]->id + 1);
			cats->setSelectedItemIndex(0);
			
			AlertWindow aw("Komodo Drop", "Delete category:", AlertWindow::InfoIcon);
			aw.addCustomComponent(cats);
			aw.addButton("ok", 1);
			aw.addButton("cancel", 2);

			if (aw.runModalLoop() == 1)
			{
				smugMug.deleteCategory(cats->getSelectedId() - 1);
			}
			delete cats;
		}
	}
	else if (res == 7)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		OwnedArray<Category> categories;
		smugMug.getCategoryList(categories);

		OwnedArray<SubCategory> subCategories;
		smugMug.getSubCategoryList(subCategories);

		if (subCategories.size())
		{
			ComboBox* cats = new ComboBox("");
			cats->setSize(300, 26);
			cats->setVisible(true);

			for (int i = 0; i < subCategories.size(); i++)
			{
				int j;
				for (j = 0; j < categories.size(); j++)
				{
					if (categories[j]->id == subCategories[i]->parentId)
						break;
				}
				cats->addItem(categories[j]->title + " >> " + subCategories[i]->title, subCategories[i]->id + 1);
			}
			cats->setSelectedItemIndex(0);
			
			AlertWindow aw("Komodo Drop", "Delete sub category:", AlertWindow::InfoIcon);
			aw.addCustomComponent(cats);
			aw.addButton("ok", 1);
			aw.addButton("cancel", 2);

			if (aw.runModalLoop() == 1)
			{
				smugMug.deleteSubCategory(cats->getSelectedId() - 1);
			}
			delete cats;
		}
	}
	else if (res == 8)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

			AlertWindow aw("Komodo Drop", "Create category:", AlertWindow::InfoIcon);
			aw.addTextEditor("name", "", "category name:");
			aw.addButton("ok", 1);
			aw.addButton("cancel", 2);

			if (aw.runModalLoop() == 1 && aw.getTextEditorContents("name").isNotEmpty())
			{
				smugMug.createCategory(aw.getTextEditorContents("name"));
			}
	}
	else if (res == 9)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		OwnedArray<Category> categories;
		smugMug.getCategoryList(categories);

		if (categories.size())
		{
			ComboBox* cats = new ComboBox("");
			cats->setSize(300, 26);
			cats->setVisible(true);

			for (int i = 0; i < categories.size(); i++)
				cats->addItem(categories[i]->title, categories[i]->id + 1);
			cats->setSelectedItemIndex(0);
			
			AlertWindow aw("Komodo Drop", "Create sub category:", AlertWindow::InfoIcon);
			aw.addCustomComponent(cats);
			aw.addTextEditor("name", "", "sub category name:");
			aw.addButton("ok", 1);
			aw.addButton("cancel", 2);

			if (aw.runModalLoop() == 1 && aw.getTextEditorContents("name").isNotEmpty())
			{
				smugMug.createSubCategory(cats->getSelectedId() - 1, aw.getTextEditorContents("name"));
			}
			delete cats;
		}
	}
	else if (res == 10)
	{
		smugMug.cancelUploads();
	}
	else if (res == 11)
	{
		FileChooser fc("Komodo Drop", File::getSpecialLocation(File::userDocumentsDirectory));
		if (fc.browseForMultipleFilesToOpen())
		{
			const Array<File>& files = fc.getResults();
			StringArray names;

			for (int i = 0; i < files.size(); i++)
				names.add(files[i].getFullPathName());

			filesDropped(names, 0, 0);
		}
	}
	else if (res == 12)
	{
		FileChooser fc("Komodo Drop", File::getSpecialLocation(File::userDocumentsDirectory));
		if (fc.browseForDirectory())
		{
			File dir = fc.getResult();

			Array<File> files;
			StringArray names;
			dir.findChildFiles(files, File::findFiles, false);

			for (int i = 0; i < files.size(); i++)
				names.add(files[i].getFullPathName());

			if (names.size())
				filesDropped(names, 0, 0);
		}
	}
	else if (res == 13)
	{
		FileChooser fc("Komodo Drop", File::getSpecialLocation(File::userDocumentsDirectory));
		if (fc.browseForDirectory())
		{
			File dir = fc.getResult();

			Array<File> files;
			StringArray names;
			dir.findChildFiles(files, File::findFiles, true);

			for (int i = 0; i < files.size(); i++)
				names.add(files[i].getFullPathName());

			if (names.size())
				filesDropped(names, 0, 0);
		}
	}
	else if (res == 19)
	{
		FileChooser fc("Komodo Drop", File::getSpecialLocation(File::userDocumentsDirectory));
		if (fc.browseForDirectory())
		{
			File dir = fc.getResult();
			
			Array<File> folders;
			dir.findChildFiles(folders, File::findDirectories, false);
			
			for (int i = 0; i < folders.size(); i++)
			{
				Array<File> files;
				folders[i].findChildFiles(files, File::findFiles, true);
				
				StringArray names;
				for (int j = 0; j < files.size(); j++)
					names.add(files[j].getFullPathName());
				
				if (names.size())
					filesDropped(names, 0, 0);
			}
		}		
	}
	else if (res == 14)
	{
		smugMug.showLogFile();
	}
	else if (res == 15)
	{
		smugMug.clearLogFile();
	}
	else if (res == 16)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		OwnedArray<Album> albums;
		smugMug.getAlbumList(albums);
		if (albums.size() > 0)
		{
			ComboBox* album = new ComboBox("");
			album->setSize(300, 26);
			album->setVisible(true);

			for (int i = 0; i < albums.size(); i++)
				album->addItem(albums[i]->getDisplayTitle(), i + 1);
			album->setSelectedItemIndex(0);
			
			AlertWindow aw("Komodo Drop", "Delete duplicate images from album:", AlertWindow::InfoIcon);
			aw.addCustomComponent(album);
			aw.addButton("ok", 1);
			aw.addButton("cancel", 2);

			if (aw.runModalLoop() == 1)
			{
				smugMug.deleteDuplicates(albums[album->getSelectedId() - 1]->id);
			}
			delete album;
		}
	}
	else if (res == 17)
	{
		smugMug.showQueue();
	}
	else if (res == 18)
	{
		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		smugMug.showTopPhotos();
	}
}


bool UploadWindow::isInterestedInFileDrag(const StringArray& files)
{
	return true;
}

void UploadWindow::filesDropped(const StringArray& filenames, int mouseX, int mouseY)
{
	StringArray extensions;
	extensions.addTokens(".jpg;.jpeg;.jpe;.png;.tif;.tiff;.png;.gif;.mpg;.mpeg;.mpe;.mov;.qt;.avi;.wmv", ";", "");

	UploadRequest* ur = new UploadRequest();
	for (int i = 0; i < filenames.size(); i++)
	{
		File imageFile(File::getCurrentWorkingDirectory().getChildFile (filenames[i]));
        
		if (imageFile.existsAsFile() && extensions.contains(imageFile.getFileExtension(), true))
		{
			ur->addImageFile(imageFile);
		}
		else if (imageFile.isDirectory())
		{
			Array<File> results;			
			imageFile.findChildFiles(results, File::findFiles, true);

			for (int i = 0; i < results.size(); i++)
			{
				if (extensions.contains(results[i].getFileExtension(), true))
					ur->addImageFile(results[i]);
			}
		}
	}

	if (ur->getNumImages() > 0)
	{
		Settings* settings = Settings::getInstance();
		if (settings->password.isEmpty() || settings->email.isEmpty())
			settings->showSettingsDialog();

		if (settings->password.isEmpty() || settings->email.isEmpty())
			return;

		if (!smugMug.isLoggedIn())
			smugMug.login(Settings::getInstance()->email, Settings::getInstance()->password);

		if (!smugMug.isLoggedIn())
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Komodo Drop", "Error: Unable to connect to Smugmug.");
			return;
		}

		String defaultAlbumName;
		File root(filenames[0]);
		if (filenames.size() == 1 && root.isDirectory())
			defaultAlbumName = root.getFileName();
		else 
			defaultAlbumName = root.getParentDirectory().getFileName();

#ifdef JUCE_DEBUG
		defaultAlbumName = defaultAlbumName.replaceCharacter('_', ' ');
#endif

		SmugID albumId;
		bool open = false;
		if (Settings::getInstance()->silentUpload)
		{
			OwnedArray<Album> albums;
			smugMug.getAlbumList(albums);
			
			for (int i = 0; i < albums.size(); i++)
			{
				if (albums[i]->title == defaultAlbumName)
				{
					albumId = albums[i]->id;
					break;
				}
			}
			if (albumId.id == -1)
			{
				StringPairArray params;
				albumId = smugMug.createAlbum(defaultAlbumName, 0, params);
			}
			ur->setName(defaultAlbumName);
		}
		else
		{
			bool newAlbum;
			int catId;
			String title;
			StringPairArray params;

			UploadDialog* ud = new UploadDialog(&smugMug, defaultAlbumName, newAlbum, title, albumId, catId, params, open);
			if (ud->runModalLoop() == 1)
			{
				ur->setName(title);
				if (newAlbum)
					albumId = smugMug.createAlbum(title, catId, params);
			}
			else
			{
				return;
			}
		}

		if (albumId.id != -1)
		{
			ur->setOpenBrowser(open);
			ur->setAlbumId(albumId);
			smugMug.uploadImages(ur);
			startTimer(333);
			repaint();
		}
		else
		{
			delete ur;
		}
	}
	else
	{
		delete ur;
	}
	return;
}

void UploadWindow::timerCallback()
{
	repaint();

	if (!smugMug.isUploading())
	{
		stopTimer();

		int failures = smugMug.getNumFailedUploads();
		if (failures > 0)
		{
			if (AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Komodo Drop", String(failures) + " images failed to upload. Try again?", "yes", "no") == 1)
				smugMug.reSubmitFailedUploads();
			else
				smugMug.cancelFailedUploads();
		}
	}
}

StringArray UploadWindow::getMenuBarNames()
{
	StringArray s;
	s.add("File");
	return s;
}

PopupMenu UploadWindow::getMenuForIndex(int topLevelMenuIndex, const String& menuName)
{
	return createMenu();
}

void UploadWindow::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
	handleCommand(menuItemID);
}
