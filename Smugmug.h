#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SmugMug;
class DupeThread;

//==============================================================================
class SmugID
{
public:
    SmugID() = default;

	int id = -1;
	String key;
};

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
class SmugMug :	public Timer,
				public ChangeBroadcaster
{
public:
	SmugMug();
	~SmugMug();

	void login(const String& username, const String& password);
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

    void authorizeIfNeeded();
    StringPairArray getRequestToken();
    void launchAuthorizeUrl (const StringPairArray& requestToken);
    StringPairArray getAccessToken (const StringPairArray& requestToken, const String& code);
    
	SmugID uploadFile (int queue, int index);
    std::unique_ptr<XmlElement> smugMugRequest (const String& method, const StringPairArray& params, bool upload = false);
	void addLogEntry (const String& message);
	String sessionId;
	String accountType;

	bool getNextFileToDo (int& queue, int& index);

	CriticalSection lock;
	OwnedArray<UploadRequest> uploadQueue;
	Array<UploadThread*> uploadThreads;
	Array<Thread*> dupeThreads;

	OwnedArray<LogItem> smugLog;
};
