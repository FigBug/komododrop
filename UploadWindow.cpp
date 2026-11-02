#include "UploadWindow.h"
#include "Settings.h"
#include "UploadComponent.h"
#include "AsyncDialogs.h"

// The upload dialog writes the user's choices straight into these, so they have to
// outlive the (now asynchronous) dialog.
struct UploadDialogContext
{
	bool newAlbum = false;
	int catId = 0;
	String title;
	StringPairArray params;
	SmugID albumId;
	bool open = false;
};

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
	// Deleted by the ModalComponentManager once the modal state unwinds.
	exitModalState(0);
}

UploadWindow::UploadWindow()
{
	setSize (96, 96);
	setOpaque (true);
	addToDesktop (0);
	setAlwaysOnTop (true);

	juce::Rectangle<int> rc;
	if (auto* display = Desktop::getInstance().getDisplays().getPrimaryDisplay())
		rc = display->userBounds.toNearestInt();

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
   #if JUCE_MAC
	MenuBarModel::setMacMainMenu (nullptr);
   #endif
   #if JUCE_WIN32 || JUCE_LINUX
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
		glyphs.addLineOfText(Font (FontOptions (25.0f).withStyle ("Bold")), String(percent, 1) + ("%"), 0, 0);

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
    if (e.getDistanceFromDragStart() >= 3)
    {
        if (! drag)
            dragger.startDraggingComponent(this, e);

        drag = true;
        dragger.dragComponent (this, e, nullptr);
    }
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
   #if JUCE_DEBUG
	sub3.addItem(18, "Show top photos");
   #endif

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
	Component::SafePointer<UploadWindow> safeThis (this);

	createMenu().showMenuAsync (PopupMenu::Options().withMousePosition(),
		[safeThis] (int res)
		{
			if (safeThis != nullptr && res != 0)
				safeThis->handleCommand (res);
		});
}

void UploadWindow::withLogin (std::function<void()> fn)
{
	Component::SafePointer<UploadWindow> safeThis (this);

	smugMug.ensureLoggedIn ([safeThis, fn] (bool loggedIn)
	{
		if (safeThis != nullptr && loggedIn)
			fn();
	});
}

void UploadWindow::handleCommand(int res)
{
	Component::SafePointer<UploadWindow> safeThis (this);

	if (res == 1)
	{
		if (!smugMug.isUploading())
		{
			JUCEApplication::quit();
		}
		else
		{
			showConfirmAsync (MessageBoxIconType::InfoIcon, "Komodo Drop",
				"There is an upload in progress, quit anyway?", "ok", "cancel",
				[] (bool quit)
				{
					if (quit)
						JUCEApplication::quit();
				});
		}
	}
	else if (res == 2)
	{
		Settings::getInstance()->showSettingsDialog();
	}
	else if (res == 3)
	{
		showMessageAsync (MessageBoxIconType::InfoIcon,
			"Komodo Drop " + JUCEApplication::getInstance()->getApplicationVersion(),
			"Created By: Roland Rabien (roland@rabien.com)\nBased on JUCE (www.juce.com)");
	}
	else if (res == 4)
	{
		setVisible(!isVisible());
	}
	else if (res == 5 || res == 16)
	{
		const bool dupesOnly = (res == 16);

		withLogin ([safeThis, dupesOnly]
		{
			OwnedArray<Album> albums;
			safeThis->smugMug.getAlbumList(albums);

			if (albums.isEmpty())
				return;

			StringArray labels;
			Array<int> ids;
			Array<SmugID> albumIds;

			for (int i = 0; i < albums.size(); i++)
			{
				labels.add(albums[i]->getDisplayTitle());
				ids.add(i + 1);
				albumIds.add(albums[i]->id);
			}

			showChooserAsync (dupesOnly ? "Delete duplicate images from album:" : "Delete album:",
				labels, ids, {},
				[safeThis, albumIds, dupesOnly] (int selectedId, String)
				{
					if (safeThis == nullptr || selectedId < 1 || selectedId > albumIds.size())
						return;

					if (dupesOnly)
						safeThis->smugMug.deleteDuplicates(albumIds[selectedId - 1]);
					else
						safeThis->smugMug.deleteAlbum(albumIds[selectedId - 1]);
				});
		});
	}
	else if (res == 6)
	{
		withLogin ([safeThis]
		{
			OwnedArray<Category> categories;
			safeThis->smugMug.getCategoryList(categories);

			if (categories.isEmpty())
				return;

			StringArray labels;
			Array<int> ids;

			for (int i = 0; i < categories.size(); i++)
			{
				labels.add(categories[i]->title);
				ids.add(categories[i]->id + 1);
			}

			showChooserAsync ("Delete category:", labels, ids, {},
				[safeThis] (int selectedId, String)
				{
					if (safeThis != nullptr && selectedId > 0)
						safeThis->smugMug.deleteCategory(selectedId - 1);
				});
		});
	}
	else if (res == 7)
	{
		withLogin ([safeThis]
		{
			OwnedArray<Category> categories;
			safeThis->smugMug.getCategoryList(categories);

			OwnedArray<SubCategory> subCategories;
			safeThis->smugMug.getSubCategoryList(subCategories);

			if (subCategories.isEmpty())
				return;

			StringArray labels;
			Array<int> ids;

			for (int i = 0; i < subCategories.size(); i++)
			{
				// A sub category whose parent isn't in the list is shown on its own
				// rather than indexing past the end of `categories`.
				String label;

				for (int j = 0; j < categories.size(); j++)
				{
					if (categories[j]->id == subCategories[i]->parentId)
					{
						label = categories[j]->title + " >> ";
						break;
					}
				}

				labels.add(label + subCategories[i]->title);
				ids.add(subCategories[i]->id + 1);
			}

			showChooserAsync ("Delete sub category:", labels, ids, {},
				[safeThis] (int selectedId, String)
				{
					if (safeThis != nullptr && selectedId > 0)
						safeThis->smugMug.deleteSubCategory(selectedId - 1);
				});
		});
	}
	else if (res == 8)
	{
		withLogin ([safeThis]
		{
			showChooserAsync ("Create category:", {}, {}, "category name:",
				[safeThis] (int, String name)
				{
					if (safeThis != nullptr)
						safeThis->smugMug.createCategory(name);
				});
		});
	}
	else if (res == 9)
	{
		withLogin ([safeThis]
		{
			OwnedArray<Category> categories;
			safeThis->smugMug.getCategoryList(categories);

			if (categories.isEmpty())
				return;

			StringArray labels;
			Array<int> ids;

			for (int i = 0; i < categories.size(); i++)
			{
				labels.add(categories[i]->title);
				ids.add(categories[i]->id + 1);
			}

			showChooserAsync ("Create sub category:", labels, ids, "sub category name:",
				[safeThis] (int selectedId, String name)
				{
					if (safeThis != nullptr && selectedId > 0)
						safeThis->smugMug.createSubCategory(selectedId - 1, name);
				});
		});
	}
	else if (res == 10)
	{
		smugMug.cancelUploads();
	}
	else if (res == 11 || res == 12 || res == 13 || res == 19)
	{
		chooseFilesToUpload (res);
	}
	else if (res == 14)
	{
		smugMug.showLogFile();
	}
	else if (res == 15)
	{
		smugMug.clearLogFile();
	}
	else if (res == 17)
	{
		smugMug.showQueue();
	}
	else if (res == 18)
	{
		withLogin ([safeThis] { safeThis->smugMug.showTopPhotos(); });
	}
}

void UploadWindow::chooseFilesToUpload (int mode)
{
	fileChooser = std::make_unique<FileChooser> ("Komodo Drop",
		File::getSpecialLocation(File::userDocumentsDirectory));

	int flags = FileBrowserComponent::openMode;

	if (mode == 11)
		flags |= FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;
	else
		flags |= FileBrowserComponent::canSelectDirectories;

	Component::SafePointer<UploadWindow> safeThis (this);

	fileChooser->launchAsync (flags, [safeThis, mode] (const FileChooser& fc)
	{
		if (safeThis == nullptr)
			return;

		StringArray names;

		if (mode == 11)
		{
			for (auto& f : fc.getResults())
				names.add(f.getFullPathName());

			if (names.size())
				safeThis->filesDropped(names, 0, 0);

			return;
		}

		File dir = fc.getResult();

		if (! dir.isDirectory())
			return;

		if (mode == 19)
		{
			// Each sub folder becomes its own upload, so they land in separate albums.
			Array<File> folders;
			dir.findChildFiles(folders, File::findDirectories, false);

			for (int i = 0; i < folders.size(); i++)
			{
				Array<File> files;
				folders[i].findChildFiles(files, File::findFiles, true);

				StringArray folderNames;
				for (int j = 0; j < files.size(); j++)
					folderNames.add(files[j].getFullPathName());

				if (folderNames.size())
					safeThis->filesDropped(folderNames, 0, 0);
			}

			return;
		}

		Array<File> files;
		dir.findChildFiles(files, File::findFiles, mode == 13);

		for (int i = 0; i < files.size(); i++)
			names.add(files[i].getFullPathName());

		if (names.size())
			safeThis->filesDropped(names, 0, 0);
	});
}

bool UploadWindow::isInterestedInFileDrag(const StringArray& files)
{
	return true;
}

void UploadWindow::filesDropped(const StringArray& filenames, int mouseX, int mouseY)
{
	StringArray extensions;
	extensions.addTokens(".jpg;.jpeg;.jpe;.png;.tif;.tiff;.png;.gif;.mpg;.mpeg;.mpe;.mov;.qt;.avi;.wmv", ";", "");

	auto ur = std::make_unique<UploadRequest>();
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

	if (ur->getNumImages() == 0)
		return;

	String defaultAlbumName;
	File root(filenames[0]);
	if (filenames.size() == 1 && root.isDirectory())
		defaultAlbumName = root.getFileName();
	else 
		defaultAlbumName = root.getParentDirectory().getFileName();

   #ifdef JUCE_DEBUG
	defaultAlbumName = defaultAlbumName.replaceCharacter('_', ' ');
   #endif

	Component::SafePointer<UploadWindow> safeThis (this);
	auto* request = ur.release();

	smugMug.ensureLoggedIn ([safeThis, request, defaultAlbumName] (bool loggedIn)
	{
		if (safeThis == nullptr)
		{
			delete request;
			return;
		}

		if (! loggedIn)
		{
			delete request;
			showMessageAsync (MessageBoxIconType::WarningIcon, "Komodo Drop",
				"Error: Unable to connect to Smugmug.");
			return;
		}

		safeThis->chooseUploadDestination (request, defaultAlbumName);
	});
}

void UploadWindow::chooseUploadDestination (UploadRequest* ur, const String& defaultAlbumName)
{
	if (Settings::getInstance()->silentUpload)
	{
		SmugID albumId;

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
		startUpload (ur, albumId, false);
		return;
	}

	// The dialog writes into `ctx`, which the callback keeps alive for it.
	auto ctx = std::make_shared<UploadDialogContext>();

	auto* ud = new UploadDialog (&smugMug, defaultAlbumName, ctx->newAlbum, ctx->title,
	                             ctx->albumId, ctx->catId, ctx->params, ctx->open);

	Component::SafePointer<UploadWindow> safeThis (this);

	ud->enterModalState (true, ModalCallbackFunction::create (
		[safeThis, ur, ctx] (int result)
		{
			if (safeThis == nullptr || result != 1)
			{
				delete ur;
				return;
			}

			ur->setName(ctx->title);

			auto albumId = ctx->albumId;

			if (ctx->newAlbum)
				albumId = safeThis->smugMug.createAlbum(ctx->title, ctx->catId, ctx->params);

			safeThis->startUpload (ur, albumId, ctx->open);
		}), true);
}

void UploadWindow::startUpload (UploadRequest* ur, SmugID albumId, bool openBrowser)
{
	if (albumId.id == -1)
	{
		delete ur;
		return;
	}

	ur->setOpenBrowser(openBrowser);
	ur->setAlbumId(albumId);
	smugMug.uploadImages(ur);
	startTimer(333);
	repaint();
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
			Component::SafePointer<UploadWindow> safeThis (this);

			showConfirmAsync (MessageBoxIconType::WarningIcon, "Komodo Drop",
				String(failures) + " images failed to upload. Try again?", "yes", "no",
				[safeThis] (bool retry)
				{
					if (safeThis == nullptr)
						return;

					if (retry)
						safeThis->smugMug.reSubmitFailedUploads();
					else
						safeThis->smugMug.cancelFailedUploads();
				});
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
