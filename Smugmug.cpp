#include "Smugmug.h"
#include "Settings.h"
#include <liboauthcpp/liboauthcpp.h>

#define API_BASE    "https://api.smugmug.com"
#define UPLOAD_HOST "upload.smugmug.com"
std::string consumer_key = "Vbq5JHJfC8sBrMRdKPqb7GDs9p8RSTkj";
std::string consumer_secret = "xSRXGphmCZK6GmSBdD9rkGLQzgd3pNvVQGsBWLgx7Z7GhSf8xGggLchNSNBbfZ9j";
std::string request_token_url = "https://api.smugmug.com/services/oauth/1.0a/getRequestToken";
std::string request_token_query_args = "oauth_callback=oob";
std::string authorize_url = "https://api.smugmug.com/services/oauth/1.0a/authorize";
std::string access_token_url = "https://api.smugmug.com/services/oauth/1.0a/getAccessToken";

//==============================================================================
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

//==============================================================================
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

//==============================================================================
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

//==============================================================================
class QueueDialog : public DialogWindow, public Timer
{
public:
	QueueDialog(SmugMug* smugMug_) : DialogWindow(("Komodo Drop"), LookAndFeel::getDefaultLookAndFeel().findColour(AlertWindow::backgroundColourId), true), smugMug(smugMug_)
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
		auto tvi = tree->getRootItem();
		tree->setRootItem (nullptr);
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

//==============================================================================
class LogDialog : public DialogWindow, 
	              public ListBoxModel, 
				  public ChangeListener
{
public:
	LogDialog (SmugMug* smugMug_)
        : DialogWindow (("Komodo Drop"), LookAndFeel::getDefaultLookAndFeel().findColour(AlertWindow::backgroundColourId), true),
        smugMug (smugMug_)
	{
		centreWithSize(600, 450);
		list = new ListBox("", this);
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

//==============================================================================
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

//==============================================================================
class DupeThread : public Thread,
                   public AsyncUpdater
{
public:
	DupeThread (SmugMug* smugmug_, UploadRequest* ur_)
      : Thread (("DupesThread")),
        smugmug (smugmug_),
        ur (ur_)
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

		smugmug->lock.enter();
		smugmug->dupeThreads.removeFirstMatchingValue(this);
		smugmug->lock.exit();

        triggerAsyncUpdate();
	}

	void handleAsyncUpdate()
	{
        stopThread(100);
		delete this;
	}

private:
	SmugMug* smugmug; 
	UploadRequest* ur;
};

// Asks for the verifier code SmugMug shows in the browser. Returns immediately;
// `callback` gets the code, or an empty string if the user cancelled.
static void getPinAsync (std::function<void (String)> callback)
{
    auto* aw = new AlertWindow ("Komodo Drop", "Authorization:", MessageBoxIconType::InfoIcon);
    aw->addTextEditor ("code", "", "Code:");
    aw->addButton ("ok", 1);
    aw->addButton ("cancel", 2);

    aw->enterModalState (true, ModalCallbackFunction::create ([aw, callback] (int result)
    {
        callback (result == 1 ? aw->getTextEditorContents ("code").trim() : String());
    }), true);
}

//==============================================================================
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

//==============================================================================
SmugMug::SmugMug()
{
    // Only authorizes if we don't already have a stored token; completes on the
    // message thread once the user has pasted the verifier code.
    ensureLoggedIn (nullptr);
}

void SmugMug::ensureLoggedIn (std::function<void (bool)> callback)
{
    if (isLoggedIn())
    {
        if (callback)
            callback (true);

        return;
    }

    authorize (std::move (callback));
}

void SmugMug::authorize (std::function<void (bool)> callback)
{
    // Only ever run one authorization at a time; later callers wait on this one.
    if (callback)
        authCallbacks.push_back (std::move (callback));

    if (authorizing)
        return;

    authorizing = true;

    auto& settings = *Settings::getInstance();
    //if (settings.accessToken.isNotEmpty())
    //    return;

    OAuth::Consumer consumer (consumer_key, consumer_secret);
    OAuth::Client oauth (&consumer);

    std::string base_request_token_url = request_token_url + (request_token_query_args.empty() ? std::string ("") : (std::string ("?") + request_token_query_args));
    std::string oAuthQueryString = oauth.getURLQueryString (OAuth::Http::Get, base_request_token_url);

    std::string url1 = request_token_url + "?" + oAuthQueryString;
    auto rsp1 = URL (url1.c_str()).readEntireTextStream();

    OAuth::Token request_token = OAuth::Token::extract (rsp1.toStdString());

    std::string url2 = authorize_url + "?oauth_token=" + request_token.key() + "&Access=Full&Permissions=Modify";
    URL (url2.c_str()).launchInDefaultBrowser();

    // The user now has to copy the verifier code out of the browser, so the rest of
    // the exchange has to wait for the prompt instead of blocking here.
    getPinAsync ([this, request_token] (String pin) mutable
    {
        if (pin.isEmpty())
        {
            finishAuthorize (false);
            return;
        }

        request_token.setPin (pin.toStdString());

        OAuth::Consumer consumer (consumer_key, consumer_secret);
        OAuth::Client oauth (&consumer, &request_token);

        auto oAuthQueryString = oauth.getURLQueryString (OAuth::Http::Get, access_token_url, std::string (""), true);
        std::string url3 = access_token_url + "?" + oAuthQueryString;

        auto access_token_resp = URL (url3.c_str()).readEntireTextStream();

        OAuth::KeyValuePairs access_token_resp_data = OAuth::ParseKeyValuePairs (access_token_resp.toStdString());
        OAuth::Token access_token = OAuth::Token::extract (access_token_resp_data);

        auto& s = *Settings::getInstance();
        s.accessToken  = access_token.key().c_str();
        s.accessSecret = access_token.secret().c_str();
        s.save();

        finishAuthorize (isLoggedIn());
    });
}

void SmugMug::finishAuthorize (bool success)
{
    authorizing = false;

    // Move the list aside first: a callback may well start another authorization.
    auto callbacks = std::move (authCallbacks);
    authCallbacks.clear();

    for (auto& cb : callbacks)
        cb (success);
}

SmugMug::~SmugMug()
{
}



//==============================================================================
// API v2 plumbing.
//
// The 1.3.0 REST API this app was written against has been retired, so every
// call below goes through api.smugmug.com/api/v2 instead: JSON rather than XML,
// OAuth in an Authorization header rather than query parameters, and a folder
// tree rather than categories and sub categories.

// Uris in a v2 response are objects carrying a "Uri" field at the default
// verbosity, and bare strings at _verbosity=1. Accept either.
static String uriOf (const var& v)
{
    if (v.isObject())
        return v["Uri"].toString();

    return v.toString();
}

int smugIdForUri (const String& uri)
{
    if (uri.isEmpty())
        return -1;

    // Must stay stable between runs: Settings persists these as lastAlbum /
    // lastCategory / lastSubCategory. Keep it positive so that `id + 1` can't
    // collide with a ComboBox id of 0 or the "<none>" entry's 1.
    auto h = uri.hashCode() & 0x7fffffff;

    return h <= 1 ? 2 : h;
}

// v2 requires UrlName to be url-safe and to start with an upper case letter.
static String urlNameFor (const String& name)
{
    String out;

    for (auto c : name)
    {
        if (CharacterFunctions::isLetterOrDigit (c) || c == '-' || c == '_')
            out << c;
        else if (out.isNotEmpty() && ! out.endsWithChar ('-'))
            out << '-';
    }

    out = out.trimCharactersAtEnd ("-_");

    if (out.isEmpty())
        return "Untitled";

    if (CharacterFunctions::isLetter (out[0]))
        return out.substring (0, 1).toUpperCase() + out.substring (1);

    return "N" + out;
}

var SmugMug::v2Request (const String& httpMethod, const String& path, const var& body)
{
    auto& s = *Settings::getInstance();

    if (s.accessToken.isEmpty())
        return {};

    const String url = String (API_BASE) + path;

    auto type = OAuth::Http::Get;

    if      (httpMethod == "POST")   type = OAuth::Http::Post;
    else if (httpMethod == "DELETE") type = OAuth::Http::Delete;
    else if (httpMethod == "PUT")    type = OAuth::Http::Put;

    String headers;

    try
    {
        OAuth::Consumer consumer (consumer_key, consumer_secret);
        OAuth::Token token (s.accessToken.toStdString(), s.accessSecret.toStdString());
        OAuth::Client oauth (&consumer, &token);

        // A JSON body isn't form encoded, so it is deliberately left out of the
        // signature base string -- only the method and url are signed.
        headers = String (oauth.getFormattedHttpHeader (type, url.toStdString()).c_str());
    }
    catch (const std::exception& e)
    {
        addLogEntry (String ("Error: could not sign request: ") + e.what());
        return {};
    }

    headers = headers.trimEnd() + "\r\nAccept: application/json";

    URL u (url);

    if (! body.isVoid())
    {
        u = u.withPOSTData (JSON::toString (body));
        headers += "\r\nContent-Type: application/json";
    }

    int status = 0;

    auto stream = u.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress)
                                           .withExtraHeaders (headers)
                                           .withHttpRequestCmd (httpMethod)
                                           .withConnectionTimeoutMs (30000)
                                           .withStatusCode (&status));

    if (stream == nullptr)
    {
        addLogEntry ("Error: unable to connect to Smugmug.");
        return {};
    }

    auto text = stream->readEntireStreamAsString();

    if (status < 200 || status >= 300)
    {
        auto message = JSON::parse (text)["Message"].toString();

        addLogEntry ("Error: " + httpMethod + " " + path + " failed (HTTP " + String (status) + ")"
                     + (message.isNotEmpty() ? ": " + message : String()));
        return {};
    }

    return JSON::parse (text)["Response"];
}

bool SmugMug::fetchAuthUser()
{
    if (nickName.isNotEmpty())
        return true;

    auto user = v2Request ("GET", "/api/v2!authuser")["User"];

    if (! user.isObject())
        return false;

    nickName = user["NickName"].toString();
    plan     = user["Plan"].toString();

    auto uris = user["Uris"];
    rootNodeUri   = uriOf (uris["Node"]);
    rootFolderUri = uriOf (uris["Folder"]);

    if (rootFolderUri.isEmpty() && nickName.isNotEmpty())
        rootFolderUri = "/api/v2/folder/user/" + nickName;

    return nickName.isNotEmpty();
}

bool SmugMug::listAll (const String& path, const String& locator, Array<var>& items)
{
    if (path.isEmpty())
        return false;

    const int pageSize = 100;
    const bool hasQuery = path.containsChar ('?');

    for (int start = 1;;)
    {
        auto response = v2Request ("GET", path + (hasQuery ? "&" : "?")
                                               + "start=" + String (start)
                                               + "&count=" + String (pageSize));

        if (! response.isObject())
            return false;

        auto found = response[Identifier (locator)];

        if (auto* array = found.getArray())
        {
            for (auto& item : *array)
                items.add (item);
        }
        else if (found.isObject())
        {
            items.add (found);
        }

        // Pages.NextPage is simply absent on the last page.
        if (uriOf (response["Pages"]["NextPage"]).isEmpty())
            return true;

        start += pageSize;
    }
}

// "/Road-Cycling/2024" -> "/Road-Cycling"; a top level path -> "" (the root).
static String parentPathOf (const String& urlPath)
{
    return urlPath.upToLastOccurrenceOf ("/", false, false);
}

int SmugMug::folderIdForPath (const String& urlPath) const
{
    if (urlPath.isEmpty())
        return -1;

    for (auto& f : folders)
        if (f.urlPath == urlPath)
            return f.id;

    return -1;
}

bool SmugMug::refreshFolders()
{
    if (! fetchAuthUser())
        return false;

    folders.clearQuick();
    albumNodes.clearQuick();

    // !folderlist and !albums each return everything at any depth, so there's no
    // need to walk the node tree a level at a time -- which also means albums
    // nested deeper than the UI's two levels still show up.
    Array<var> folderItems;

    if (! listAll ("/api/v2/folder/user/" + nickName + "!folderlist", "FolderList", folderItems))
        return false;

    for (auto& item : folderItems)
    {
        Folder f;
        f.name    = item["Name"].toString();
        f.urlPath = item["UrlPath"].toString();
        f.uri     = "/api/v2/folder/user/" + nickName + f.urlPath;
        f.id      = smugIdForUri (f.uri);

        if (f.urlPath.isNotEmpty() && f.id != -1)
            folders.add (f);
    }

    // Link folders to their parents by path, now that they're all known.
    for (auto& f : folders)
        f.parentId = folderIdForPath (parentPathOf (f.urlPath));

    Array<var> albumItems;

    if (! listAll ("/api/v2/user/" + nickName + "!albums", "Album", albumItems))
        return false;

    for (auto& item : albumItems)
    {
        AlbumNode a;
        a.name           = item["Name"].toString();
        a.id.uri         = uriOf (item["Uri"]);
        a.id.key         = item["AlbumKey"].toString();
        a.id.id          = smugIdForUri (a.id.uri);
        a.parentFolderId = folderIdForPath (parentPathOf (item["UrlPath"].toString()));

        if (a.id.uri.isNotEmpty())
            albumNodes.add (a);
    }

    return true;
}

const SmugMug::Folder* SmugMug::folderForId (int id) const
{
    for (auto& f : folders)
        if (f.id == id)
            return &f;

    return nullptr;
}

var SmugMug::createNode (const String& parentFolderUri, const String& type,
                         const String& name, const var& extraFields)
{
    if (parentFolderUri.isEmpty() || name.isEmpty())
        return {};

    auto* fields = new DynamicObject();
    fields->setProperty ("Name", name);
    fields->setProperty ("UrlName", urlNameFor (name));

    if (auto* extra = extraFields.getDynamicObject())
        for (auto& prop : extra->getProperties())
            fields->setProperty (prop.name, prop.value);

    const String endpoint = (type == "Album") ? "!albums" : "!folders";

    return v2Request ("POST", parentFolderUri + endpoint, var (fields));
}

//==============================================================================
bool SmugMug::isPower()
{
    return plan.containsIgnoreCase ("Power") || isPro();
}

bool SmugMug::isPro()
{
    return plan.containsIgnoreCase ("Pro") || plan.containsIgnoreCase ("Portfolio");
}

bool SmugMug::isLoggedIn()
{
    return Settings::getInstance()->accessToken.isNotEmpty();
}

void SmugMug::logout()
{
    auto& s = *Settings::getInstance();
    s.accessSecret = {};
    s.accessToken  = {};
    s.save();

    nickName      = {};
    rootFolderUri = {};
    rootNodeUri   = {};
    plan          = {};
    folders.clearQuick();
}

bool SmugMug::getNumberOfViews (int, int, OwnedArray<Views>&, OwnedArray<Views>&)
{
    // The 1.3.0 getTransferStats call has no direct v2 equivalent, and the only
    // caller is the debug-only "Show top photos" menu item.
    addLogEntry ("Info: transfer stats are not available in the SmugMug v2 API");
    return false;
}

bool SmugMug::getImageUrls (SmugID id, ImageUrls& urls)
{
    if (id.uri.isEmpty())
        return false;

    // !sizes hangs off the image route, not the album-image one: an album image
    // uri is /api/v2/album/<ak>/image/<ik>-0, and appending !sizes to that 404s.
    // Its last segment is the image id, so rebuild the uri around that.
    auto imageId = id.uri.fromLastOccurrenceOf ("/", false, false);

    if (imageId.isEmpty())
        return false;

    auto sizes = v2Request ("GET", "/api/v2/image/" + imageId + "!sizes")["ImageSizes"];

    if (! sizes.isObject())
        return false;

    urls.originalURL = sizes["OriginalImageUrl"].toString();
    urls.largeURL    = sizes["LargeImageUrl"].toString();
    urls.mediumURL   = sizes["MediumImageUrl"].toString();
    urls.smallURL    = sizes["SmallImageUrl"].toString();
    urls.tinyURL     = sizes["TinyImageUrl"].toString();
    urls.thumbURL    = sizes["ThumbImageUrl"].toString();

    return true;
}

bool SmugMug::getImages (OwnedArray<ImageItem>& images, SmugID albumId)
{
    if (albumId.uri.isEmpty())
        return false;

    const int pageSize = 100;

    for (int start = 1;;)
    {
        auto response = v2Request ("GET", albumId.uri + "!images?start=" + String (start)
                                                      + "&count=" + String (pageSize));

        if (! response.isObject())
            return false;

        auto parseImage = [&images] (const var& img)
        {
            auto* itm = new ImageItem();

            itm->id.uri       = uriOf (img["Uri"]);
            itm->id.key       = img["ImageKey"].toString();
            itm->id.id        = smugIdForUri (itm->id.uri);
            itm->filename     = img["FileName"].toString();
            itm->caption      = img["Caption"].toString();
            itm->keywords     = img["Keywords"].toString();
            itm->position     = 0;
            itm->date         = img["DateTimeUploaded"].toString();
            itm->format       = img["Format"].toString();
            itm->serial       = 0;
            itm->watermark    = bool (img["Watermarked"]);
            itm->size         = int (img["ArchivedSize"]);
            itm->width        = int (img["OriginalWidth"]);
            itm->height       = int (img["OriginalHeight"]);
            itm->md5sum       = img["ArchivedMD5"].toString();
            itm->lastUpdated  = img["LastUpdated"].toString();
            itm->originalURL  = img["ArchivedUri"].toString();
            itm->thumbURL     = img["ThumbnailUrl"].toString();
            itm->albumURL     = img["WebUri"].toString();

            images.add (itm);
        };

        auto albumImage = response["AlbumImage"];

        if (auto* array = albumImage.getArray())
        {
            for (auto& img : *array)
                parseImage (img);
        }
        else if (albumImage.isObject())
        {
            parseImage (albumImage);
        }

        if (uriOf (response["Pages"]["NextPage"]).isEmpty())
            break;

        start += pageSize;
    }

    return images.size() > 0;
}

bool SmugMug::deleteCategory (int category)
{
    if (auto* f = folderForId (category))
    {
        if (v2Request ("DELETE", f->uri).isVoid())
            return false;

        folders.clearQuick();
        return true;
    }

    return false;
}

bool SmugMug::deleteDuplicates (SmugID albumId)
{
    OwnedArray<ImageItem> images;

    if (getImages (images, albumId))
    {
        Array<SmugID> duplicateIds;
        StringArray fileNames;

        for (int i = 0; i < images.size(); i++)
        {
            if (fileNames.contains (images[i]->filename))
                duplicateIds.add (images[i]->id);
            else
                fileNames.add (images[i]->filename);
        }

        for (auto& id : duplicateIds)
            deleteImage (id);

        return true;
    }

    return false;
}

bool SmugMug::deleteSubCategory (int subCategory)
{
    return deleteCategory (subCategory);
}

bool SmugMug::deleteAlbum (SmugID album)
{
    if (album.uri.isEmpty())
        return false;

    return ! v2Request ("DELETE", album.uri).isVoid();
}

bool SmugMug::deleteImage (SmugID image)
{
    if (image.uri.isEmpty())
        return false;

    return ! v2Request ("DELETE", image.uri).isVoid();
}

bool SmugMug::getAlbumList (OwnedArray<Album>& albums)
{
    // The albums come out of the folder walk, so each one's category and sub
    // category are exactly the folders it was found under -- no need to infer a
    // parent from the album object. Cached, because the walk costs one request
    // per category.
    if (folders.isEmpty() && ! refreshFolders())
        return false;

    for (auto& node : albumNodes)
    {
        auto* album = new Album();

        album->id    = node.id;
        album->title = node.name;

        album->categoryId    = -1;
        album->subCategoryId = -1;

        // Walk from the album's folder up to the root, so the category is always
        // the top level folder and the sub category the one below it, however
        // deeply the album is actually nested.
        Array<const Folder*> ancestors;

        for (auto* f = folderForId (node.parentFolderId); f != nullptr;
             f = f->parentId == -1 ? nullptr : folderForId (f->parentId))
            ancestors.insert (0, f);

        if (ancestors.size() >= 1)
        {
            album->category   = ancestors[0]->name;
            album->categoryId = ancestors[0]->id;
        }

        if (ancestors.size() >= 2)
        {
            album->subCategory   = ancestors[1]->name;
            album->subCategoryId = ancestors[1]->id;
        }

        albums.add (album);
    }

    return true;
}

bool SmugMug::getCategoryList (OwnedArray<Category>& categories)
{
    if (folders.isEmpty() && ! refreshFolders())
        return false;

    for (auto& f : folders)
    {
        if (f.parentId != -1)
            continue;

        auto* category = new Category();
        category->id    = f.id;
        category->title = f.name;
        categories.add (category);
    }

    return true;
}

bool SmugMug::getSubCategoryList (OwnedArray<SubCategory>& subCategories)
{
    if (folders.isEmpty() && ! refreshFolders())
        return false;

    for (auto& f : folders)
    {
        if (f.parentId == -1)
            continue;

        auto* subCategory = new SubCategory();
        subCategory->id       = f.id;
        subCategory->parentId = f.parentId;
        subCategory->title    = f.name;
        subCategories.add (subCategory);
    }

    return true;
}

int SmugMug::createCategory (const String& title)
{
    if (! fetchAuthUser())
        return -1;

    auto folder = createNode (rootFolderUri, "Folder", title, {})["Folder"];

    if (! folder.isObject())
        return -1;

    folders.clearQuick();

    auto id = smugIdForUri (uriOf (folder["Uris"]["Node"]));

    if (id != -1)
        addLogEntry ("Info: category created: " + title);

    return id;
}

int SmugMug::createSubCategory (const int categoryId, const String& title)
{
    if (! fetchAuthUser())
        return -1;

    if (folders.isEmpty())
        refreshFolders();

    auto* parent = folderForId (categoryId);

    if (parent == nullptr)
        return -1;

    auto folder = createNode (parent->uri, "Folder", title, {})["Folder"];

    if (! folder.isObject())
        return -1;

    folders.clearQuick();

    auto id = smugIdForUri (uriOf (folder["Uris"]["Node"]));

    if (id != -1)
        addLogEntry ("Info: sub category created: " + title);

    return id;
}

SmugID SmugMug::createAlbum (const String& title, const int categoryId, const StringPairArray& albumFlags)
{
    SmugID newAlbum;

    if (! fetchAuthUser())
        return newAlbum;

    if (folders.isEmpty())
        refreshFolders();

    // The dialog hands back a category, and optionally a sub category inside it.
    // The deeper of the two is the album's parent.
    String parentUri = rootFolderUri;

    if (auto* category = folderForId (categoryId))
        parentUri = category->uri;

    if (albumFlags.containsKey ("SubCategoryID"))
        if (auto* subCategory = folderForId (albumFlags["SubCategoryID"].getIntValue()))
            parentUri = subCategory->uri;

    // Album settings that map cleanly onto v2. The remaining 1.3.0 flags the
    // dialog collects (Filenames, Comments, External, EXIF, Share, Header,
    // Larges, Clean, Watermarking, SmugSearchable, WorldSearchable, FamilyEdit,
    // FriendEdit) have no straightforward v2 equivalent and are skipped rather
    // than sent as fields the API would reject.
    auto* fields = new DynamicObject();

    fields->setProperty ("Privacy", albumFlags["Public"] == "0" ? "Unlisted" : "Public");

    if (albumFlags["Description"].isNotEmpty())  fields->setProperty ("Description",  albumFlags["Description"]);
    if (albumFlags["Keywords"].isNotEmpty())     fields->setProperty ("Keywords",     albumFlags["Keywords"]);
    if (albumFlags["Password"].isNotEmpty())     fields->setProperty ("Password",     albumFlags["Password"]);
    if (albumFlags["PasswordHint"].isNotEmpty()) fields->setProperty ("PasswordHint", albumFlags["PasswordHint"]);

    if (albumFlags.containsKey ("Originals"))
        fields->setProperty ("Downloadable", albumFlags["Originals"] == "1");

    if (albumFlags.containsKey ("Protected"))
        fields->setProperty ("Protected", albumFlags["Protected"] == "1");

    if (albumFlags.containsKey ("SortMethod"))
    {
        // The dialog still offers the 1.3.0 sort names.
        auto sort = albumFlags["SortMethod"];

        if (sort == "Position" || sort == "Caption" || sort == "FileName")
            fields->setProperty ("SortMethod", sort);
        else if (sort.isNotEmpty())
            fields->setProperty ("SortMethod", "Date Taken");
    }

    if (albumFlags.containsKey ("SortDirection"))
        fields->setProperty ("SortDirection", albumFlags["SortDirection"] == "1" ? "Descending" : "Ascending");

    auto album = createNode (parentUri, "Album", title, var (fields))["Album"];

    if (album.isObject())
    {
        newAlbum.uri = uriOf (album["Uri"]);
        newAlbum.key = album["AlbumKey"].toString();
        newAlbum.id  = smugIdForUri (newAlbum.uri);

        if (newAlbum.uri.isNotEmpty())
            addLogEntry ("Info: album created: " + title);
    }

    return newAlbum;
}

void SmugMug::uploadImages (UploadRequest* ur, bool allowDupeCheck)
{
	if (Settings::getInstance()->checkForDupes && allowDupeCheck)
	{
		lock.enter();
		auto t = new DupeThread(this, ur);
		dupeThreads.add(t);
		t->startThread();
		lock.exit();
	}
	else
	{
		lock.enter();

		if (uploadThreads.size() == 0)
			for (int i = 0;i < uploadQueue.size(); i++)
				uploadQueue[i]->setIgnore (true);

		uploadQueue.add (ur);

		while (uploadThreads.size() < Settings::getInstance()->uploadThreads)
		{
			auto ut = new UploadThread (this);
			uploadThreads.add (ut);
			ut->startThread();
		}
		lock.exit();
	}
}

SmugID SmugMug::uploadFile (int queue, int index)
{
	SmugID retval;

	lock.enter();
	UploadFile& uf = uploadQueue[queue]->getImageFileInfo (index);
	lock.exit();

	int64 bytesDone = 0;
	MD5 md5 (uf.file);

	Time start = Time::getCurrentTime();

	lock.enter();
	auto albumUri = uploadQueue[queue]->getAlbumId().uri;
	lock.exit();

	if (albumUri.isEmpty())
	{
		uf.status = UploadFile::Failed;
		addLogEntry ("Error: no destination album for " + uf.file.getFileName());
		return retval;
	}

	// v2 uploads go to upload.smugmug.com as an OAuth-signed POST, with the
	// destination carried in X-Smug-AlbumUri. The old X-Smug-SessionID scheme
	// died with the 1.3.0 API.
	const String uploadUrl = String ("https://") + UPLOAD_HOST + "/";
	String authHeader;

	{
		auto& settings = *Settings::getInstance();

		try
		{
			OAuth::Consumer consumer (consumer_key, consumer_secret);
			OAuth::Token token (settings.accessToken.toStdString(), settings.accessSecret.toStdString());
			OAuth::Client oauth (&consumer, &token);

			authHeader = String (oauth.getFormattedHttpHeader (OAuth::Http::Post, uploadUrl.toStdString()).c_str()).trimEnd();
		}
		catch (const std::exception& e)
		{
			uf.status = UploadFile::Failed;
			addLogEntry (String ("Error: could not sign upload: ") + e.what());
			return retval;
		}
	}

	String headers;
    String filename = uf.file.getFileName();
    headers = "POST / HTTP/1.1\r\n"
		      "Host: " UPLOAD_HOST "\r\n"
		      + authHeader + "\r\n"
			  "Content-Length: " + String(uf.file.getSize()) + "\r\n"
		      "Content-MD5: " + md5.toHexString() + "\r\n"
			  "X-Smug-Version: v2\r\n"
			  "X-Smug-ResponseType: JSON\r\n"
			  "X-Smug-AlbumUri: " + albumUri + "\r\n"
			  "X-Smug-FileName: " + filename + "\r\n"
			  "Connection: close\r\n\r\n";

   #ifdef JUCE_DEBUG
	Logger::outputDebugString (headers);
  #endif

	auto headerUtf8 = headers.toUTF8();

    gin::SecureStreamingSocket soc;

	if (soc.connect ("upload.smugmug.com", 443))
	{
		int bytesWritten = soc.write (headerUtf8, (int)strlen (headerUtf8));
		if (bytesWritten == -1)
		{
			uf.status = UploadFile::Failed;
			return retval;
		}

        if (auto fos = std::unique_ptr<FileInputStream> (uf.file.createInputStream()))
		{
			char buffer[1024 * 8];
			while (! fos->isExhausted())
			{
				int in = fos->read (buffer, sizeof (buffer));
				int out = soc.write (buffer, in);

				if (in != out)
				{
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
					return retval;
				}
			}
		}
		else
		{
			uf.status = UploadFile::Failed;
			return retval;
		}

		String response;
		response.preallocateBytes(1024);

		while (true)
		{
			char buffer;
			int read = soc.read (&buffer, 1, true);
			if (read == -1)
				break;

			response += buffer;

			if (response.endsWith (("\r\n\r\n")) || response.endsWith (("\n\n")))
			{
				String len = response.fromFirstOccurrenceOf (("Content-Length: "), false, true);
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
					while (true)
					{
						String line;
						char ch;
						while (! line.endsWith ("\r\n"))
						{
							soc.read (&ch, 1, true);
							line += ch;
						}

						int sz = line.getHexValue32();
						if (sz == 0)
							break;

						char* buf = new char[sz + 1];
						soc.read (buf, sz, true);
						buf[sz] = 0;

						response += buf;
						delete[] buf;

						soc.read (&ch, 1, true);
						soc.read (&ch, 1, true);
					}
				}

               #ifdef JUCE_DEBUG
				Logger::outputDebugString(response);
               #endif
				soc.close();

				auto body = response.fromFirstOccurrenceOf ("{", true, false);
				auto parsed = JSON::parse (body);

				if (parsed["stat"].toString() == "ok")
				{
					if (auto image = parsed["Image"]; image.isObject())
					{
						uf.status = UploadFile::Finished;
						uf.complete = 1.0f;
						uf.url = image["URL"].toString();

						Time end = Time::getCurrentTime();
						RelativeTime diff = end - start;

						addLogEntry(("Info: ") + uf.file.getFileName() + (" uploaded in ") + String(int(diff.inSeconds())) + (" seconds [") + String(uf.file.getSize() / 1024 / diff.inSeconds(), 1) + ("KB/s]"));

						retval.uri = image["ImageUri"].toString();
						retval.key = image["Key"].toString();
						retval.id  = smugIdForUri (retval.uri);

						return retval;
					}
				}
				else
				{
					addLogEntry ("Error: upload of " + uf.file.getFileName() + " failed: "
					             + parsed["message"].toString());
				}
			}
		} 		
	}

	uf.status = UploadFile::Failed;
	return retval;
}

bool SmugMug::getNextFileToDo (int& queue, int& index)
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
	ScopedLock sl (lock);
	return uploadThreads.size() > 0 || dupeThreads.size() > 0;
}

int64 SmugMug::getTotalbytesUploaded()
{
	ScopedLock sl (lock);
	int uploaded = 0;
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		auto ur = uploadQueue[i];
		if (! ur->getIgnore())
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
	ScopedLock sl (lock);
	int uploaded = 0;
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		auto ur = uploadQueue[i];
		if (! ur->getIgnore())
		{
			for (int j = 0; j < ur->getNumImages(); j++)
			{
				auto& uf = ur->getImageFileInfo (j);
				if (uf.status == UploadFile::Finished || uf.status == UploadFile::Uploading || uf.status == UploadFile::Waiting)
					uploaded += uf.size;
			}
		}
	}
	return uploaded;
}

int SmugMug::getNumFailedUploads()
{
	ScopedLock sl(lock);
	int failed = 0;
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		auto ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			auto& uf = ur->getImageFileInfo (j);
			if (uf.status == UploadFile::Failed)
				failed++;
		}
	}
	return failed;
}

void SmugMug::cancelFailedUploads()
{
	ScopedLock sl (lock);
	for (int i = 0; i < uploadQueue.size(); i++)
	{
		auto ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			auto& uf = ur->getImageFileInfo(j);
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
		auto ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			auto& uf = ur->getImageFileInfo(j);
			if (uf.status == UploadFile::Failed)
				uf.status = UploadFile::Waiting;
		}
	}

	while (uploadThreads.size() < Settings::getInstance()->uploadThreads)
	{
		auto ut = new UploadThread (this);
		uploadThreads.add (ut);
		ut->startThread();
	}

	addLogEntry (("Info: failed uploads readded to queue"));
}

void SmugMug::cancelUploads()
{
	ScopedLock sl(lock);

	for (int i = 0; i < uploadQueue.size(); i++)
	{
		auto ur = uploadQueue[i];
		for (int j = 0; j < ur->getNumImages(); j++)
		{
			auto& uf = ur->getImageFileInfo(j);
			uf.status = UploadFile::Cancelled;
		}
	}

	addLogEntry(("Info: all uploads cancelled"));
}

void SmugMug::addLogEntry(const String& message)
{
	auto li = new LogItem();
	li->message = message;
	li->time = Time::getCurrentTime();
	smugLog.add (li);
	sendChangeMessage();
}

void SmugMug::showQueue()
{
	new QueueDialog (this);
}

void SmugMug::showLogFile()
{
	new LogDialog (this);
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
	if (getNumberOfViews (tm.getMonth(), tm.getYear(), albums, images))
	{
	}
}

//==============================================================================
UploadThread::UploadThread (SmugMug* smugMug_)
  : Thread (("UploadThread")),
    smugMug (smugMug_)
{
}

UploadThread::~UploadThread()
{
    stopThread (100);
}

void  UploadThread::handleAsyncUpdate()
{
	delete this;
}

void UploadThread::run()
{
	while (true)
	{
		int queue = 0;
		int index = 0;
		UploadRequest* ur = nullptr;
		smugMug->lock.enter();	
		bool res = smugMug->getNextFileToDo (queue, index);
		if (res)
		{
			ur = smugMug->uploadQueue[queue];
			ur->getImageFileInfo(index).status = UploadFile::Uploading;
		}
		smugMug->lock.exit();

		if (!res)
		{
			smugMug->lock.enter();
			smugMug->uploadThreads.removeFirstMatchingValue (this);
			smugMug->lock.exit();
			triggerAsyncUpdate();
			return;
		}
		SmugID id = smugMug->uploadFile(queue, index);

		ur->getImageFileInfo (index).id = id;
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
				URL (url).launchInDefaultBrowser();
			}
		}
	}
}

