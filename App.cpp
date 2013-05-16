#include "stdafx.h"
#include "UploadWindow.h"

class SmugmugUploaderApplication : public JUCEApplication
{
    UploadWindow* mainWindow;
	LookAndFeel lf;

public:
    SmugmugUploaderApplication() : mainWindow (NULL)
    {
    }

    ~SmugmugUploaderApplication()
    {
    }

    void initialise (const String& commandLine)
    {
        Logger::outputDebugString(commandLine);
        
		LookAndFeel::setDefaultLookAndFeel(&lf);
        mainWindow = new UploadWindow();
		
		StringArray files;
		files.addTokens(commandLine, true);
		
		if (files.size() > 0)
			mainWindow->filesDropped(files, 0, 0);
    }

    void shutdown()
    {
        delete mainWindow;
    }

    const String getApplicationName()
    {
        return ("KomodoDrop for Smugmug");
    }

    const String getApplicationVersion()
    {
        return ("0.0.0.6");
    }
	
	void anotherInstanceStarted (const String& commandLine)
	{
		StringArray files;
		files.addTokens(commandLine, true);
		
		if (files.size() > 0)
			mainWindow->filesDropped(files, 0, 0);
	}
	
	 bool moreThanOneInstanceAllowed()
	{
		 return false;
	}
};

START_JUCE_APPLICATION (SmugmugUploaderApplication)

