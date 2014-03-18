#ifndef FileObserver_h
#define FileObserver_h
#pragma once

#include <string>

class FileObserver
{
public:
    enum Event {
        None            = 1 << 0,
        Access          = 1 << 1,
        Modify          = 1 << 2,
        AttribChanged   = 1 << 3,
        CloseWrite      = 1 << 4,
        CloseNoWrite    = 1 << 5,
        Open            = 1 << 6,
        MovedFrom       = 1 << 7,
        Delete          = 1 << 8,
        Error           = 1 << 9,
    };

    class Delegate
    {
    public:
        virtual void onEvent(Event event, const std::string& path) = 0;
    };
    
    FileObserver(const std::string& path, Delegate* delegate);
    bool startWatching();
    void stopWatching();
    Delegate* delegate() const { return mDelegate;}
    void setEventFilter(int filter) { mEventFilter = filter; }
    int eventFilter() const { return mEventFilter;}

private:
    std::string mPath;
    Delegate* mDelegate;
    int mEventFilter;
};


#endif
