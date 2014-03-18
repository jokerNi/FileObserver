#ifndef FileDeleteObserver_h
#define FileDeleteObserver_h

#include "FileObserver.h"

class FileDeleteObserver : public FileObserver::Delegate
{
public:
    FileDeleteObserver(const std::string& path);
    bool startWatching();
    void stopWatching();
	void setHttpRequestOnDelete(const std::string& url);
	void cancel();
    
private:
    virtual void onEvent(FileObserver::Event event, const std::string& path);
    void finish(bool success);
	void onDelete(const std::string& path);
	int sendRequest();

    bool mCancelled;
    std::string mPath;
	std::string mUrl;
    FileObserver* mFileObserver;
};


#endif
