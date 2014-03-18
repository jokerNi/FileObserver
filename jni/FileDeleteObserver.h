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
    
private:
    virtual void onEvent(FileObserver::Event event, const std::string& path);
	void onDelete(const std::string& path);
	int sendRequest();
    
    std::string mPath;
	std::string mUrl;
    FileObserver* mFileObserver;
};


#endif
