#include "stdafx.h"
#include "Smugmug.h"
#include "Settings.h"

#define BASE_URL   "http://api.smugmug.com/services/api/rest/1.2.2/"
#define UPLOAD_URL "http://upload.smugmug.com/services/api/rest/1.2.2/"
#define APIKEY     "gaEJ900gzH9ucFiUxBYd9sRfj6T3iTiM"

#define LOGOUT_TIMER (10 * 60 * 1000)

class FileItem : public TreeViewItem
{
public:
	FileItem(SmugMug* sm_, int queue_, int file_) : sm(sm_), queue(queue_), file(file_)
	{
	}

	~FileItem()
	{
	}

	void paintItem (Graphics& g, int width, int height)
	{
		g.setColour(Colours::black);
		UploadFile& uf = sm->getUploadQueue()[queue]->getImageFileInfo(file);
		g.drawText(uf.file.getFileName(), 0, 0, width, height, Justification::centredLeft, true);

		String status;
		switch (uf.status)
		{
			case UploadFile::Waiting:	status = ("Waiting"); break;
			case UploadFile::Uploading: status = String::formatted(("Uploading: %.f%%"), uf.complete * 100.0); break;
			case UploadFile::Finished:  status = ("Complete"); break;
			case UploadFile::Failed:	status = ("Failed"); break;
			case UploadFile::Cancelled: status = ("Cancelled"); break;
			case UploadFile::Duplicate: status = ("Duplicate"); break;
		}
		g.drawText(status, width / 2, 0, width / 2, height, Justification::centredLeft, true);
	}

	bool mightContainSubItems()
	{
		return false;
	}
private:
	SmugMug* sm;
	int queue;
	int file;
};

class QueueItem : public TreeViewItem
{
public:
	QueueItem(SmugMug* sm_, int idx_) : sm(sm_), idx(idx_)
	{
		for (int i = 0; i < sm->getUploadQueue()[idx]->getNumImages(); i++)
			addSubItem(new FileItem(sm, idx, i));
	}

	~QueueItem()
	{
		while (getNumSubItems() > 0)
			removeSubItem(0);
	}

	void paintItem (Graphics& g, int width, int height)
	{
		g.setColour(Colours::black);
		UploadRequest* ur = sm->getUploadQueue()[idx];
		g.drawText(ur->getName(), 0, 0, width, height, Justification::centredLeft, true);
	}

	bool mightContainSubItems()
	{
		return true;
	}
private:
	SmugMug* sm;
	int idx;
};

class RootItem : public TreeViewItem
{
public:
	RootItem(SmugMug* sm)
	{
		for (int i = 0; i < sm->getUploadQueue().size(); i++)
			addSubItem(new QueueItem(sm, i));
	}

	~RootItem()
	{
		while (getNumSubItems() > 0)
			removeSubItem(0);
	}

	bool mightContainSubItems()
	{
		return true;
	}
};

class QueueDialog : public DialogWindow, public Timer
{
public:
	QueueDialog(SmugMug* smugMug_) : DialogWindow(("Komodo Drop"), Colours::white, true), smugMug(smugMug_)
	{
		centreWithSize(600, 450);
		tree = new TreeView();
		tree->setSize(600, 450);
		tree->setRootItemVisible(false);
		tree->setDefaultOpenness(true);
		tree->setRootItem(new RootItem(smugMug));
		setContentOwned(tree, true);
		setVisible(true);
		startTimer(333);
	}

	~QueueDialog()
	{
		TreeViewItem* tvi = tree->getRootItem();
		tree->setRootItem(NULL);
		delete tvi;
		stopTimer();
	}

	void closeButtonPressed()
	{
		delete this;
	}

	void timerCallback()
	{
		repaint();
	}

private:
	SmugMug* smugMug;
	TreeView* tree;
};

class LogDialog : public DialogWindow, 
	              public ListBoxModel, 
				  public ChangeListener
{
public:
	LogDialog(SmugMug* smugMug_) : DialogWindow(("Komodo Drop"), Colours::white, true), smugMug(smugMug_)
	{
		centreWithSize(600, 450);
		list = new ListBox(String::empty, this);
		list->setSize(600, 450);
		setContentOwned(list, true);
		list->updateContent();
		list->setVisible(true);
		setVisible(true);
		smugMug->addChangeListener(this);
	}

	~LogDialog()
	{
		smugMug->removeChangeListener(this);
	}

	int getNumRows()
	{
		return smugMug->smugLog.size();
	}

	void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
	{
		g.setColour(Colours::black);
		g.drawText(smugMug->smugLog[rowNumber]->time.toString(false, true), 2, 0, width - 2, height, Justification::centredLeft, true);
		g.drawText(smugMug->smugLog[rowNumber]->message, 100, 0, width - 100, height, Justification::centredLeft, true);
	}

	void closeButtonPressed()
	{
		delete this;
	}

	void changeListenerCallback(ChangeBroadcaster* source)
	{
		list->updateContent();
	}
private:
	SmugMug* smugMug;
	ListBox* list;
};

int UploadRequest::nextUploadId = 1;

UploadRequest::UploadRequest()
{
	uploadId = nextUploadId++;
	open = false;
	ignore = false;
}

UploadRequest::~UploadRequest()
{
}

void UploadRequest::addImageFile(const File& image)
{
	UploadFile* uf = new UploadFile();
	uf->complete = 0;
	uf->file     = image;
	uf->status   = UploadFile::Waiting;
	uf->size     = int(image.getSize());
	imageFiles.add(uf);
}

void UploadRequest::removeImageFile(int idx)
{
	imageFiles.remove(idx);
}

const File& UploadRequest::getImageFile(int idx)
{
	return imageFiles[idx]->file;
}

UploadFile& UploadRequest::getImageFileInfo(int idx)
{
	return *imageFiles[idx];
}

int UploadRequest::getNumImages()
{
	return imageFiles.size();
}

void UploadRequest::setAlbumId(SmugID albumId_)
{
	albumId = albumId_;
}

const SmugID UploadRequest::getAlbumId()
{
	return albumId;
}

void UploadRequest::setImageFileComplete(const File& image, float done)
{
	for (int i = 0; i < imageFiles.size(); i++)
	{
		if (image == imageFiles[i]->file)
		{
			imageFiles[i]->complete = done;
			if (done >= 1.0f)
				imageFiles[i]->status = UploadFile::Finished;
			break;
		}
	}
}

void UploadRequest::setImageFileFailed(const File& image)
{
	for (int i = 0;i < imageFiles.size(); i++)
	{
		if (image == imageFiles[i]->file)
		{
			imageFiles[i]->complete = 0;
			imageFiles[i]->status   = UploadFile::Failed;
			break;
		}
	}
}

void UploadRequest::setOpenBrowser(bool b)
{
	open = b;
}

bool UploadRequest::getOpenBrowser()
{
	return open;
}

void UploadRequest::setIgnore(bool b)
{
	ignore = b;
}

bool UploadRequest::getIgnore()
{
	return ignore;
}

class DupeThread : public Thread, public AsyncUpdater
{
public:
	DupeThread(SmugMug* smugmug_, UploadRequest* ur_) : Thread(("DupesThread")), smugmug(smugmug_), ur(ur_)
	{
	}
	~DupeThread()
	{
	}

	void run()
	{	
		OwnedArray<ImageItem> images;
		bool res = smugmug->getImages(images, ur->getAlbumId());

		smugmug->lock.enter();
		if (res)
		{
			for (int i = ur->getNumImages() - 1; i >= 0; i--)
			{
				for (int j = 0; j < images.size(); j++)
				{
					if (ur->getImageFile(i).getFileName() == images[j]->filename)
					{
						ur->getImageFileInfo(i).status = UploadFile::Duplicate;
					}
				}
			}
		}

		for (int i = 0; i < smugmug->uploadQueue.size(); i++)
		{
			if (smugmug->uploadQueue[i]->getAlbumId().id == ur->getAlbumId().id)
			{
				for (int j = 0; j < smugmug->uploadQueue[i]->getNumImages(); j++)
				{
					for (int k = 0; k < ur->getNumImages(); k++)
					{
						if (ur->getImageFile(k).getFileName() == smugmug->uploadQueue[i]->getImageFile(j).getFileName())
						{
							ur->getImageFileInfo(k).status = UploadFile::Duplicate;
						}
					}
				}
			}
		}
		smugmug->lock.exit();

		smugmug->uploadImages(ur, false);
		triggerAsyncUpdate();

		smugmug->lock.enter();
		smugmug->dupeThreads.removeFirstMatchingValue(this);
		smugmug->lock.exit();
	}

	void handleAsyncUpdate()
	{
		delete this;
	}

private:
	SmugMug* smugmug; 
	UploadRequest* ur;
};

String Album::getDisplayTitle()
{
	if (category.isNotEmpty() && subCategory.isNotEmpty())
	{
		return title + (" [") + category + (": ") + subCategory + ("]");
	}
	else if (category.isNotEmpty())
	{
		return title + (" [") + category + ("]");
	}
	else
	{
		return title;
	}
}

SmugMug::SmugMug()
{
}

SmugMug::~SmugMug()
{
	if (sessionId.isNotEmpty())
		logout();
}

void SmugMug::login(const String& username, const String& password)
{
	sessionId = String::empty;

	StringPairArray params;
	params.set(("EmailAddress"), username);
	params.set(("Password"), password);

	XmlElement* n = smugMugRequest(("smugmug.login.withPassword"), params);
	
	if (n)
	{
		XmlElement* l = n->getChildByName(("Login"));
		if (l)
		{
			accountType = l->getStringAttribute(("AccountType"));
			XmlElement* s = l->getChildByName(("Session"));
			if (s)
			{
				sessionId = s->getStringAttribute(("id"));
				addLogEntry(("Info: logged in session: ") + sessionId);
			}
		}
		delete n;
	}
}

bool SmugMug::isPower()
{
	return accountType == ("Power") || isPro();
}

bool SmugMug::isPro()
{
	return accountType == ("Pro");
}

bool SmugMug::isLoggedIn()
{
	return sessionId.isNotEmpty();
}

void SmugMug::logout()
{
	StringPairArray params;
	XmlElement* n = smugMugRequest(("smugmug.logout"), params);

	addLogEntry(("Info: logged out session ") + sessionId);
	if (n)
		delete n;

	sessionId = String::empty;
}

bool SmugMug::getNumberOfViews(int month, int year, OwnedArray<Views>& albums, OwnedArray<Views>& images)
{
	StringPairArray params;
	params.set(("Month"), String(month));
	params.set(("Year"), String(year));
	XmlElement* n = smugMugRequest(("smugmug.users.getTransferStats"), params);

	if (n)
	{
		XmlElement* albs = n->getChildByName(("Albums"));
		if (albs)
		{
			XmlElement* alb = albs->getChildByName(("Album"));
			while (alb)
			{
				Views* v = new Views();
				v->id    = alb->getIntAttribute(("id"));
				v->views = alb->getIntAttribute(("Tiny")) + alb->getIntAttribute(("Small")) + alb->getIntAttribute(("Medium")) + alb->getIntAttribute(("Large")) + roundDoubleToInt(alb->getDoubleAttribute(("Original")));

				albums.add(v);

				XmlElement* img = n->getChildByName(("Image"));
				while (img)
				{
					Views* v = new Views();
					v->id    = img->getIntAttribute(("id"));
					v->views = img->getIntAttribute(("Tiny")) + img->getIntAttribute(("Small")) + img->getIntAttribute(("Medium")) + img->getIntAttribute(("Large")) + roundDoubleToInt(img->getDoubleAttribute(("Original")));

					img = img->getNextElementWithTagName(("Image"));
				}

				alb = alb->getNextElementWithTagName(("Album"));
			}
		}
		delete n;
		return true;
	}
	return false;
}

bool SmugMug::getImageUrls(SmugID id, ImageUrls& urls)
{
	StringPairArray params;
	params.set(("ImageID"), String(id.id));
	params.set(("ImageKey"), id.key);
	params.set(("Password"), "");
	XmlElement* n = smugMugRequest(("smugmug.images.getURLs"), params);

	if (n)
	{
		XmlElement* img = n->getChildByName(("Image"));

		if (img)
		{
			urls.originalURL = img->getStringAttribute(("OriginalURL"));
			urls.largeURL    = img->getStringAttribute(("LargeURL"));
			urls.mediumURL   = img->getStringAttribute(("MediumURL"));
			urls.smallURL    = img->getStringAttribute(("SmallURL"));
			urls.tinyURL     = img->getStringAttribute(("TinyURL"));
			urls.thumbURL    = img->getStringAttribute(("ThumbURL"));
			urls.albumURL    = img->getStringAttribute(("AlbumURL"));
		}
	
		delete n;
		return true;
	}
	return false;
}

bool SmugMug::getImages(OwnedArray<ImageItem>& images, SmugID albumId)
{
	StringPairArray params;
	params.set(("AlbumID"), String(albumId.id));
	params.set(("AlbumKey"), albumId.key);
	params.set(("Heavy"), ("1"));
	XmlElement* n = smugMugRequest(("smugmug.images.get"), params);

	if (n)
	{
		XmlElement* a = n->getChildByName(("Album"));
		if (a)
		{
			XmlElement* i = a->getChildByName(("Images"));
			if (i)
			{
				XmlElement* img = i->getChildByName(("Image"));
				while (img)
				{
					ImageItem* itm = new ImageItem();

					itm->id.id			= img->getIntAttribute(("id"));
					itm->id.key			= img->getStringAttribute(("Key"));
					itm->filename		= img->getStringAttribute(("FileName"));
					itm->caption		= img->getStringAttribute((""));
					itm->keywords		= img->getStringAttribute((""));
					itm->position		= img->getIntAttribute(("Position"));
					itm->date			= img->getStringAttribute(("Date"));
					itm->format			= img->getStringAttribute(("Format"));
					itm->serial			= img->getIntAttribute(("Serial"));
					itm->watermark		= img->getBoolAttribute(("Watermark"));
					itm->size			= img->getIntAttribute(("Size"));
					itm->width			= img->getIntAttribute(("Width"));
					itm->height			= img->getIntAttribute(("Height"));
					itm->md5sum			= img->getStringAttribute(("MD5Sum"));
					itm->lastUpdated	= img->getStringAttribute(("LastUpdated"));
					itm->originalURL	= img->getStringAttribute(("OriginalURL"));
					itm->largeURL		= img->getStringAttribute(("LargeURL"));
					itm->mediumURL		= img->getStringAttribute(("MediumURL"));
					itm->smallURL		= img->getStringAttribute(("SmallURL"));
					itm->tinyURL		= img->getStringAttribute(("TinyURL"));
					itm->thumbURL		= img->getStringAttribute(("ThumbURL"));
					itm->albumURL		= img->getStringAttribute(("AlbumURL"));

					images.add(itm);

					img = img->getNextElementWithTagName(("Image"));
				}
			}
		}
		delete n;
		return images.size() > 0;
	}
	return false;
}

bool SmugMug::deleteCategory(int category)
{
	StringPairArray params;
	params.set(("CategoryID"), String(category));
	XmlElement* n = smugMugRequest(("smugmug.categories.delete"), params);

	if (n)
	{	
		if (n->getTagName() == ("rsp") && n->getStringAttribute(("stat")) == ("ok"))
		{
			delete n;
			return true;
		}
		delete n;
	}
	return false;
}

bool SmugMug::deleteDuplicates(SmugID albumId)
{
	OwnedArray<ImageItem> images;
	if (getImages(images, albumId))
	{
		OwnedArray<SmugID> duplicateIds;
		StringArray fileNames;

		for (int i = 0; i < images.size(); i++)
		{
			if (fileNames.contains(images[i]->filename))
				duplicateIds.add(new SmugID(images[i]->id));
			else
				fileNames.add(images[i]->filename);
		}

		for (int i = 0; i < duplicateIds.size(); i++)
			deleteImage(*duplicateIds[i]);

		return true;
	}
	return false;
}

bool SmugMug::deleteSubCategory(int subCategory)
{
	StringPairArray params;
	params.set(("SubCategoryID"), String(subCategory));
	XmlElement* n = smugMugRequest(("smugmug.subcategories.delete"), params);

	if (n)
	{	
		if (n->getTagName() == ("rsp") && n->getStringAttribute(("stat")) == ("ok"))
		{
			delete n;
			return true;
		}
		delete n;
	}
	return false;
}

bool SmugMug::deleteAlbum(SmugID album)
{
	StringPairArray params;
	params.set(("AlbumID"), String(album.id));
	XmlElement* n = smugMugRequest(("smugmug.albums.delete"), params);

	if (n)
	{	
		if (n->getTagName() == ("rsp") && n->getStringAttribute(("stat")) == ("ok"))
		{
			delete n;
			return true;
		}
		delete n;
	}
	return false;
}

bool SmugMug::deleteImage(SmugID image)
{
	StringPairArray params;
	params.set(("ImageID"), String(image.id));
	XmlElement* n = smugMugRequest(("smugmug.images.delete"), params);

	if (n)
	{	
		if (n->getTagName() == ("rsp") && n->getStringAttribute(("stat")) == ("ok"))
		{
			delete n;
			return true;
		}
		delete n;
	}
	return false;
}

bool SmugMug::getAlbumList(OwnedArray<Album>& albums)
{
	StringPairArray params;
	XmlElement* n = smugMugRequest(("smugmug.albums.get"), params);

	if (n)
	{
		XmlElement* a = n->getChildByName(("Albums"));
		if (a)
		{
			XmlElement* alb = a->getChildByName(("Album"));
			while (alb)
			{
				Album* album = new Album();
				album->id.id         = alb->getIntAttribute(("id"));
				album->id.key        = alb->getStringAttribute(("Key"));
				album->title         = alb->getStringAttribute(("Title"));

				XmlElement* cat = alb->getChildByName(("Category"));
				if (cat)
				{
					album->category   = cat->getStringAttribute(("Name"));
					album->categoryId = cat->getIntAttribute(("id"));
				}
				else
				{
					album->category   = String::empty;
					album->categoryId = -1;
				}

				XmlElement* subcat = alb->getChildByName(("SubCategory"));
				if (subcat)
				{
					album->subCategory   = subcat->getStringAttribute(("Name"));
					album->subCategoryId = subcat->getIntAttribute(("id"));
				}
				else
				{
					album->subCategory   = String::empty;
					album->subCategoryId = -1;
				}

				albums.add(album);

				alb = alb->getNextElementWithTagName(("Album"));
			}
		}
		delete n;
		return true;
	}
	return false;
}

bool SmugMug::getCategoryList(OwnedArray<Category>& categories)
{
	StringPairArray params;
	XmlElement* n = smugMugRequest(("smugmug.categories.get"), params);

	if (n)
	{
		XmlElement* c = n->getChildByName(("Categories"));
		if (c)
		{
			XmlElement* cat = c->getChildByName(("Category"));
			while (cat)
			{
				Category* category = new Category();
				category->id    = cat->getIntAttribute(("id"));
				category->title = cat->getStringAttribute(("Name"));
				categories.add(category);

				cat = cat->getNextElementWithTagName(("Category"));
			}
		}
		delete n;
		return true;
	}
	return false;
}

bool SmugMug::getSubCategoryList(OwnedArray<SubCategory>& subCategories)
{
	StringPairArray params;
	XmlElement* n = smugMugRequest(("smugmug.subcategories.getAll"), params);

	if (n)
	{
		XmlElement* c = n->getChildByName(("SubCategories"));
		if (c)
		{
			XmlElement* subCat = c->getChildByName(("SubCategory"));
			while (subCat)
			{
				SubCategory* subCategory = new SubCategory();
				subCategory->id    = subCat->getIntAttribute(("id"));
				subCategory->title = subCat->getStringAttribute(("Name"));

				XmlElement* cat = subCat->getChildByName(("Category"));
				if (cat)
					subCategory->parentId = cat->getIntAttribute(("id"));

				subCategories.add(subCategory);

				subCat = subCat->getNextElementWithTagName(("SubCategory"));
			}
		}
		delete n;
		return true;
	}
	return false;
}

int SmugMug::createCategory(const String& title)
{
	int catId = -1;

	StringPairArray params;
	params.set(("Name"), title);
	XmlElement* n = smugMugRequest(("smugmug.categories.create"), params);

	if (n)
	{
		XmlElement* a = n->getChildByName(("Category"));
		if (a) 
			catId = a->getIntAttribute(("id"), -1);

		delete n;
	}
	return catId;
}

int SmugMug::createSubCategory(const int categoryId, const String& title)
{	
	int subCatId = -1;

	StringPairArray params;
	params.set(("CategoryID"), String(categoryId));
	params.set(("Name"), title);
	XmlElement* n = smugMugRequest(("smugmug.subcategories.create"), params);

	if (n)
	{
		XmlElement* a = n->getChildByName(("SubCategory"));
		if (a) 
			subCatId = a->getIntAttribute(("id"), -1);

		delete n;
	}
	return subCatId;
}

SmugID SmugMug::createAlbum(const String& title, const int categoryId, const StringPairArray& albumFlags)
{
	SmugID newAlbum;
	newAlbum.id = -1;

	StringPairArray params(albumFlags);
	params.set(("Title"), title);
	params.set(("CategoryID"), String(categoryId));
	XmlElement* n = smugMugRequest(("smugmug.albums.create"), params);

	if (n)
	{
		XmlElement* a = n->getChildByName(("Album"));
		if (a) 
		{
			newAlbum.id  = a->getIntAttribute(("id"), -1);
			newAlbum.key = a->getStringAttribute(("Key"));
		}

		if (newAlbum.id != -1)
			addLogEntry(("Info: album created: ") + title);

		delete n;
	}
	return newAlbum;
}

XmlElement* SmugMug::smugMugRequest(const String& method, const StringPairArray& params_, bool upload)
{
	StringPairArray params(params_);
	params.set(("method"), method);
	params.set(("APIKey"), APIKEY);

	startTimer(LOGOUT_TIMER);

	if (sessionId.isNotEmpty())
		params.set(("SessionID"), sessionId);

	URL url(upload ? UPLOAD_URL : BASE_URL);

	StringArray keys = params.getAllKeys();
	StringArray vals = params.getAllValues();

	for (int i = 0; i < keys.size(); i++)
		url = url.withParameter(keys[i], vals[i]);

	XmlElement* x = url.readEntireXmlStream(upload);
#ifdef JUCE_DEBUG
	Logger::outputDebugString(url.toString(true));
	if (x)
		Logger::outputDebugString(x->createDocument(String::empty, true));
#endif
	if (x && x->getStringAttribute(("stat")) == ("fail"))
	{
		XmlElement* err = x->getChildByName(("err"));
		if (err)
		{
			addLogEntry(("Error: ") + err->getStringAttribute(("msg")));
		}
	}
	return x;
}

void SmugMug::uploadImages(UploadRequest* ur, bool allowDupeCheck)
{
	if (Settings::getInstance()->checkForDupes && allowDupeCheck)
	{
		lock.enter();
		Thread* t = new DupeThread(this, ur);
		dupeThreads.add(t);
		t->startThread();
		lock.exit();
	}
	else
	{
		lock.enter();

		if (uploadThreads.size() == 0)
		{
			for (int i = 0;i < uploadQueue.size(); i++)
				uploadQueue[i]->setIgnore(true);
		}

		uploadQueue.add(ur);

		while (uploadThreads.size() < Settings::getInstance()->uploadThreads)
		{
			UploadThread* ut = NULL;
			uploadThreads.add(ut = new UploadThread(this));
			ut->startThread();
		}
		lock.exit();
	}
}

SmugID SmugMug::uploadFile(int queue, int index)
{
	SmugID retval;

	lock.enter();
	UploadFile& uf = uploadQueue[queue]->getImageFileInfo(index);
	lock.exit();

	int64 bytesDone = 0;
	MD5 md5(uf.file);

	Time start = Time::getCurrentTime();

	startTimer(LOGOUT_TIMER);

	String headers;
	headers = "PUT http://upload.smugmug.com/" + uf.file.getFileName() + " HTTP/1.1\r\n" +
		      "Host: upload.smugmug.com\r\n" +
			  "Content-Length: " + String(uf.file.getSize()) + "\r\n" +
		      "Content-MD5: " + md5.toHexString() + "\r\n" +
			  "X-Smug-SessionID: " + sessionId + "\r\n" +
			  "X-Smug-Version: 1.2.0\r\n" +
			  "X-Smug-ResponseType: REST\r\n" +
			  "X-Smug-AlbumID: " + String(uploadQueue[queue]->getAlbumId().id) + "\r\n" +
			  "X-Smug-FileName: " + uf.file.getFileName() + "\r\n\r\n";

#ifdef JUCE_DEBUG
	Logger::outputDebugString(headers);
#endif

	const char* headerUtf8 = headers.toUTF8();

	StreamingSocket soc;

	if (soc.connect("upload.smugmug.com", 80))
	{
		int bytesWritten = soc.write(headerUtf8, (int)strlen(headerUtf8));
		if (bytesWritten == -1)
		{
			uf.status = UploadFile::Failed;
			return retval;
		}

		FileInputStream* fos = uf.file.createInputStream();
		if (fos)
		{
			char buffer[1024 * 8];
			while (!fos->isExhausted())
			{
				int in = fos->read(buffer, sizeof(buffer));
				int out = soc.write(buffer, in);

				startTimer(LOGOUT_TIMER);

				if (in != out)
				{
					delete fos;
					uf.status = UploadFile::Failed;
					return retval;
				}
				else
				{
					bytesDone += in;
					uf.complete = float(bytesDone)/float(uf.file.getSize());
				}

				if (uf.status == UploadFile::Cancelled)
				{
					delete fos;
					return retval;
				}
			}
			delete fos;
		}
		else
		{
			uf.status = UploadFile::Failed;
			return retval;
		}

		String response;
		response.preallocateBytes(1024);

		while (1)
		{
			char buffer;
			int read = soc.read(&buffer, 1, true);
			if (read == -1)
				break;

			response += buffer;

			if (response.endsWith(("\r\n\r\n")) || response.endsWith(("\n\n")))
			{
				String len = response.fromFirstOccurrenceOf(("Content-Length: "), false, true);
				if (len.isNotEmpty())
				{
					// normal mode
					String num;

					int i = 0;
					while (CharacterFunctions::isDigit(len[i]))
						num += len[i++];
					
					int bytes = num.getIntValue();

					char* buffer = new char[bytes + 1];
					soc.read(buffer, bytes, true);
					buffer[bytes] = 0;

					response += buffer;
					delete[] buffer;
				}
				else
				{
					// chunked
					while (1)
					{
						String line;
						char ch;
						while (!line.endsWith("\r\n"))
						{
							soc.read(&ch, 1, true);
							line += ch;
						}

						int sz = line.getHexValue32();
						if (sz == 0)
							break;

						char* buf = new char[sz + 1];
						soc.read(buf, sz, true);
						buf[sz] = 0;

						response += buf;
						delete buf;

						soc.read(&ch, 1, true);
						soc.read(&ch, 1, true);
					}
				}

#ifdef JUCE_DEBUG				
				Logger::outputDebugString(response);
#endif
				soc.close();

				String xml = response.fromFirstOccurrenceOf(("<?xml"), true, true); 
				XmlDocument doc(xml);
				XmlElement* e = doc.getDocumentElement();
				if (e)
				{
					XmlElement* image = e->getChildByName(("Image"));
					if (image)
					{
						int val = image->getIntAttribute(("id"));
						if (val >= 0)
						{
							uf.status = UploadFile::Finished;
							uf.complete = 1.0f;
							uf.url = image->getStringAttribute("URL");

							Time end = Time::getCurrentTime();
							RelativeTime diff = end - start;

							addLogEntry(("Info: ") + uf.file.getFileName() + (" uploaded in ") + String(int(diff.inSeconds())) + (" seconds [") + String(uf.file.getSize() / 1024 / diff.inSeconds(), 1) + ("KB/s]"));

							retval.id  = val;
							retval.key = image->getStringAttribute(("Key"));
								
							delete e;
							return retval;
						}
					}
					delete e;
				}
			}
		} 		
	}

	uf.status = UploadFile::Failed;
	return retval;
}

bool SmugMug::getNextFileToDo(int& queue, int& index)
{
	lock.enter();
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			UploadFile& uf = ur->getImageFileInfo(j);
			if (uf.status == UploadFile::Waiting)
			{
				queue = i;
				index = j;
				lock.exit();
				return true;
			}
		}
	}
	lock.exit();
	return false;
}

bool SmugMug::isUploading()
{
	ScopedLock sl(lock);
	return uploadThreads.size() > 0 || dupeThreads.size() > 0;
}

int64 SmugMug::getTotalbytesUploaded()
{
	ScopedLock sl(lock);
	int uploaded = 0;
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		if (!ur->getIgnore())
		{
			for (int j = 0; j < ur->getNumImages(); j++)
			{
				UploadFile& uf = ur->getImageFileInfo(j);
				if (uf.status == UploadFile::Finished)
					uploaded += uf.size;
				else if (uf.status == UploadFile::Uploading)
					uploaded += int(uf.size * uf.complete);
			}
		}
	}
	return uploaded;
}

int64 SmugMug::getTotalBytesToUpload()
{
	ScopedLock sl(lock);
	int uploaded = 0;
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		if (!ur->getIgnore())
		{
			for (int j = 0; j < ur->getNumImages(); j++)
			{
				UploadFile& uf = ur->getImageFileInfo(j);
				if (uf.status == UploadFile::Finished || uf.status == UploadFile::Uploading || uf.status == UploadFile::Waiting)
					uploaded += uf.size;
			}
		}
	}
	return uploaded;
}

void SmugMug::timerCallback()
{
	stopTimer();
	logout();
}

int SmugMug::getNumFailedUploads()
{
	ScopedLock sl(lock);
	int failed = 0;
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			UploadFile& uf = ur->getImageFileInfo(j);
			if (uf.status == UploadFile::Failed)
				failed++;
		}
	}
	return failed;
}

void SmugMug::cancelFailedUploads()
{
	ScopedLock sl(lock);
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			UploadFile& uf = ur->getImageFileInfo(j);
			if (uf.status == UploadFile::Failed)
				uf.status = UploadFile::Cancelled;
		}
	}
}

void SmugMug::reSubmitFailedUploads()
{
	ScopedLock sl(lock);

	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			UploadFile& uf = ur->getImageFileInfo(j);
			if (uf.status == UploadFile::Failed)
				uf.status = UploadFile::Waiting;
		}
	}

	while (uploadThreads.size() < Settings::getInstance()->uploadThreads)
	{
		UploadThread* ut = NULL;
		uploadThreads.add(ut = new UploadThread(this));
		ut->startThread();
	}

	addLogEntry(("Info: failed uploads readded to queue"));
}

void SmugMug::cancelUploads()
{
	ScopedLock sl(lock);

	for (int i = 0; i < uploadQueue.size(); i++)
	{
		UploadRequest* ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			UploadFile& uf = ur->getImageFileInfo(j);
			uf.status = UploadFile::Cancelled;
		}
	}

	addLogEntry(("Info: all uploads cancelled"));
}

void SmugMug::addLogEntry(const String& message)
{
	LogItem* li = new LogItem();
	li->message = message;
	li->time = Time::getCurrentTime();
	smugLog.add(li);
	sendChangeMessage();
}

void SmugMug::showQueue()
{
	new QueueDialog(this);
}

void SmugMug::showLogFile()
{
	new LogDialog(this);
}

void SmugMug::clearLogFile()
{
	smugLog.clear();
	sendChangeMessage();
}

void SmugMug::showTopPhotos()
{
	Time tm = Time::getCurrentTime();

	OwnedArray<Views> albums;
	OwnedArray<Views> images;
	if (getNumberOfViews(tm.getMonth(), tm.getYear(), albums, images))
	{
	}
}

UploadThread::UploadThread(SmugMug* smugMug_) 
  : Thread(("UploadThread")),
    smugMug(smugMug_)
{
}

UploadThread::~UploadThread()
{
}

void  UploadThread::handleAsyncUpdate()
{
	delete this;
}

void UploadThread::run()
{
	while (1)
	{
		int queue = 0;
		int index = 0;
		UploadRequest* ur = NULL;
		smugMug->lock.enter();	
		bool res = smugMug->getNextFileToDo(queue, index);
		if (res)
		{
			ur = smugMug->uploadQueue[queue];
			ur->getImageFileInfo(index).status = UploadFile::Uploading;
		}
		smugMug->lock.exit();

		if (!res)
		{
			smugMug->lock.enter();
			smugMug->uploadThreads.removeFirstMatchingValue(this);
			smugMug->lock.exit();
			triggerAsyncUpdate();
			return;
		}
		SmugID id = smugMug->uploadFile(queue, index);

		ur->getImageFileInfo(index).id = id;		
		if (ur->getOpenBrowser())
		{
			bool done = true;
			String url;
			for (int i = 0; i < ur->getNumImages(); i++)
			{
				UploadFile& uf = ur->getImageFileInfo(i);
				if (uf.status == UploadFile::Waiting || uf.status == UploadFile::Uploading)
				{
					done = false;
					break;
				}
				else if (url.isEmpty())
				{
					url = uf.url;
				}
			}
			if (done && url.isNotEmpty())
			{
				URL(url).launchInDefaultBrowser();
			}
		}
	}
}

