#include "FileDeleteObserver.h"
#include <string>
#include <unistd.h>
#include <curl/curl.h>
#include "MyLog.h"
#include "SimpleTcpClient.h"
#include "BackendServer.h"

const int kFailRetryTimes = 10;
using namespace std;

FileDeleteObserver::FileDeleteObserver(const std::string& path)
{
    mPath = path;
    mFileObserver = new FileObserver(path, this);
    mFileObserver->setEventFilter(FileObserver::Delete | FileObserver::Error);
    mCancelled = false;
}

bool FileDeleteObserver::startWatching()
{
    return mFileObserver->startWatching();
}

void FileDeleteObserver::stopWatching()
{
    mFileObserver->stopWatching();
}

void FileDeleteObserver::setHttpRequestOnDelete(const std::string& url)
{
    XLOG("FileDeleteObserver::setHttpRequestOnDelete url=%s", url.c_str());
    mUrl = url;
}

void FileDeleteObserver::cancel()
{
    XLOG("FileDeleteObserver::cancel");
    mCancelled = true;
}

void FileDeleteObserver::onEvent(FileObserver::Event event, const std::string& path)
{
    XLOG("FileDeleteObserver::onEvent event=%d", event);
    if (event == FileObserver::Delete)
    {
        if (mCancelled)
        {
            finish(true);
            return;
        }
        onDelete(path);
    }
    else if (event == FileObserver::Error)
    {
        XLOG("FileDeleteObserver::onEvent receive error");
        BackendServer::Stop();
        finish(false);
    }
}

void FileDeleteObserver::onDelete(const std::string& path)
{
    XLOG("FileDeleteObserver::onDelete delete path=%s", path.c_str());
    for (int i=0; i<kFailRetryTimes; ++i)
    {
        if (CURLE_OK == sendRequest())
        {
            XLOG("FileDeleteObserver::onDelete sendRequest success");
            break;
        }
        else
        {
            XLOG("FileDeleteObserver::onDelete sendRequest failed");
            usleep(1000 * 1000 * 3);       // Wait for a while
        }
    }
    BackendServer::Stop();
    finish(true);
}

int FileDeleteObserver::sendRequest()
{
    if (mUrl.empty())
        return -1;

    int result = 0;
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) 
    {    
        curl_easy_setopt(curl, CURLOPT_URL, mUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  
        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        
        /* Check for errors */ 
        if(res != CURLE_OK)
        {
            XLOG("FileDeleteObserver::sendRequest curl_easy_perform() failed: %s", curl_easy_strerror(res));
            result = res;
        }
        else
        {
            long retcode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
            XLOG("FileDeleteObserver::sendRequest curl_easy_perform() success, response code=%d", retcode);
        }
  
        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }
    
    return result;
}

void FileDeleteObserver::finish(bool success)
{
    XLOG("FileDeleteObserver::finish success=%d", success);
    delete this;
}

