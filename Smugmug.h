#pragma once

#include <JuceHeader.h>

class SmugMug;
class DupeThread;

//==============================================================================
class SmugID
{
public:
    SmugID() = default;

	/** A stable synthetic id derived from `uri`. API v2 has no integer ids, but the
	    UI uses ints for ComboBox entries and Settings persists them between runs,
	    so this is a hash of the uri rather than an index. -1 means "not set".
	*/
	int id = -1;
	String key;     // v2 AlbumKey / ImageKey
	String uri;     // v2 object uri, e.g. /api/v2/album/XXXXX
};

/** Derives the stable synthetic id used throughout the UI from a v2 uri. Always
    returns a positive value so that `id + 1` can never collide with a ComboBox
    id of 0 ("nothing selected") or 1 (the "<none>" sub category entry).
*/
int smugIdForUri (const String& uri);

//==============================================================================
class UploadFile
{
public:
    UploadFile() = default;

	enum Status { Waiting, Uploading, Finished, Failed, Cancelled, Duplicate };

	File file;
	int size = 0;
	float complete = 0;
	Status status = Waiting;
	SmugID id;
	String url;
};

//==============================================================================
class UploadRequest
{
public:
	UploadRequest();
	~UploadRequest();

	SmugID getId() { return albumId; }

	void addImageFile(const File& image);
	void removeImageFile(int idx);
	void setImageFileComplete(const File& image, float done);
	void setImageFileFailed(const File& image);

	const File& getImageFile(int idx);
	UploadFile& getImageFileInfo(int idx);
	int getNumImages();

	void setAlbumId(SmugID albumId);
	const SmugID getAlbumId();

	void setOpenBrowser(bool b);
	bool getOpenBrowser();

	void setIgnore(bool b);
	bool getIgnore();

	String getName() { return name; }
	void setName(const String& s) { name = s; } 

private:
	static int nextUploadId;
	int uploadId;
	SmugID albumId;
	bool open;
	bool ignore;
	String name;

	OwnedArray<UploadFile> imageFiles;
};

//==============================================================================
class Category
{
public:
	int id;
	String title;
};

//==============================================================================
class SubCategory
{
public:
	int id;
	int parentId;
	String title;
};

//==============================================================================
class Album
{
public:
	SmugID id;
	String title;
	String category;
	int categoryId;
	String subCategory;
	int subCategoryId;
	
	String getDisplayTitle();
};

//==============================================================================
class ImageItem
{
public:
	SmugID id;
	String filename;
	String caption;
	String keywords;
	int position;
	String date;
	String format;
	int serial;
	bool watermark;
	int size;
	int width;
	int height;
	String md5sum;
	String lastUpdated; 
	String originalURL;
	String largeURL;
	String mediumURL;
	String smallURL;
	String tinyURL;
	String thumbURL;
	String albumURL;
};

//==============================================================================
class ImageUrls
{
public:
	String originalURL;
	String largeURL;
	String mediumURL;
	String smallURL;
	String tinyURL;
	String thumbURL;
	String albumURL;
};

//==============================================================================
class LogItem
{
public:
	Time time;
	String message;
};

//==============================================================================
class Views
{
public:
	int id;
	int views;
};

//==============================================================================
class UploadThread : public Thread, 
                     public AsyncUpdater
{
public:
	UploadThread(SmugMug* smugMug);
	~UploadThread();

	void run();
	void handleAsyncUpdate();

private:
	SmugMug* smugMug;
};

//==============================================================================
class SmugMug : public ChangeBroadcaster
{
public:
	SmugMug();
	~SmugMug();

	/** Makes sure we have an access token, authorizing via the browser + PIN
	    dialog if we don't. Returns immediately; `callback` runs on the message
	    thread with the result. Safe to call while an authorization is already in
	    flight — the callbacks queue up behind the one request.
	*/
	void ensureLoggedIn (std::function<void (bool)> callback);
	void logout();
	bool isLoggedIn();
	void cancelUploads();
	bool isPower();
	bool isPro();

	bool getCategoryList(OwnedArray<Category>& categories);
	bool getSubCategoryList(OwnedArray<SubCategory>& subCategories);
	bool getAlbumList(OwnedArray<Album>& albums);
	bool getImages(OwnedArray<ImageItem>& images, SmugID albumId);

	bool getImageUrls(SmugID id, ImageUrls& urls);
	bool getNumberOfViews(int month, int year, OwnedArray<Views>& albums, OwnedArray<Views>& images);

	bool deleteAlbum(SmugID album);
	bool deleteImage(SmugID image);
	bool deleteCategory(int category);
	bool deleteSubCategory(int subCategory);

	bool deleteDuplicates(SmugID albumId);

	SmugID createAlbum(const String& title, const int categoryId, const StringPairArray& params);
	int createCategory(const String& title);
	int createSubCategory(const int categoryId, const String& title);

	void uploadImages(UploadRequest* ur, bool allowDupeCheck = true);
	bool isUploading();

	int64 getTotalbytesUploaded();
	int64 getTotalBytesToUpload();

	void timerCallback();

	int getNumFailedUploads();
	void reSubmitFailedUploads();
	void cancelFailedUploads();
	
	void showLogFile();
	void clearLogFile();

	void showQueue();
	void showTopPhotos();

	OwnedArray<UploadRequest>& getUploadQueue() { return uploadQueue; }
private:
	friend class DupeThread;
	friend class UploadThread;
	friend class LogDialog;

    void authorize (std::function<void (bool)> callback);
    void finishAuthorize (bool success);
	SmugID uploadFile (int queue, int index);

	/** Performs an OAuth-signed API v2 request. `path` is everything after the
	    host, e.g. "/api/v2!authuser". `body`, when not void, is sent as JSON.
	    Returns the payload's "Response" object, or a void var on failure.
	*/
	var v2Request (const String& httpMethod, const String& path, const var& body = var());

	/** Reads the signed-in user's nickname, root folder and plan. Cached after the
	    first successful call. */
	bool fetchAuthUser();

	/** A folder in the user's tree. The app's "categories" are the folders directly
	    under the root node and its "sub categories" are one level below those —
	    v2 has no categories, just a folder tree.
	*/
	struct Folder
	{
		int id = -1;
		int parentId = -1;
		String name;
		String uri;      // /api/v2/folder/user/<nick>/<path>
		String nodeUri;  // /api/v2/node/<id>
	};

	/** An album found while walking the folder tree, with the folder it lives in.
	    Taking the parent from the walk itself avoids having to infer it from the
	    album object's own uris.
	*/
	struct AlbumNode
	{
		SmugID id;
		String name;
		int parentFolderId = -1;
	};

	/** Re-reads the folder tree, two levels deep (all the UI models), collecting
	    the albums in it as it goes. */
	bool refreshFolders();
	const Folder* folderForId (int id) const;

	/** Lists the child nodes of a node uri, following pagination. */
	bool listChildNodes (const String& nodeUri, Array<var>& nodes);

	/** POSTs a new child folder or album under `parentFolderUri`. */
	var createNode (const String& parentFolderUri, const String& type,
	                const String& name, const var& extraFields);

	void addLogEntry (const String& message);

	bool getNextFileToDo (int& queue, int& index);

	String nickName;
	String rootFolderUri;
	String rootNodeUri;
	String plan;

	Array<Folder> folders;
	Array<AlbumNode> albumNodes;

	CriticalSection lock;
	OwnedArray<UploadRequest> uploadQueue;
	Array<UploadThread*> uploadThreads;
	Array<Thread*> dupeThreads;

	OwnedArray<LogItem> smugLog;

	bool authorizing = false;
	std::vector<std::function<void (bool)>> authCallbacks;
};
