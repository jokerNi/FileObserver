#include "FileObserver.h"

#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stddef.h>
#include <sys/queue.h>
#include <sys/inotify.h>
#include "MyLog.h"
#include "event_queue.h"

using namespace std;

struct ThreadParams
{
    FileObserver::Delegate* delegate;
    const char* path;
};


void* ThreadFunc(void* param)
{
    ThreadParams* thread_params = static_cast<ThreadParams*>(param);
    FileObserver::Delegate* delegate = thread_params->delegate;
    const char* path = thread_params->path;
    
    int fd = inotify_init();
    if (fd == -1)
    {
        XLOG("inotify_init failed");
        return NULL;
    }

    while (true)
    {
        char tmpPath[256];
        sprintf(tmpPath, path, strlen(path));
        XLOG("watching %s\n", path);
        //int wd = inotify_add_watch(fd, tmpPath, IN_DELETE_SELF);
        int wd = inotify_add_watch(fd, tmpPath, IN_ALL_EVENTS);
        if (wd == -1)
        {
            XLOG("inotify_add_watch failed");
            break;
        }

        XLOG("inotify_add_watch success");
        const int bufferSize = 16384;
        char buffer[bufferSize];
        ssize_t recvSize = read(fd, buffer, bufferSize);
        if (recvSize < 0)
        {
            XLOG("inotify read error");
        }
        else
        {
            XLOG("read done, bytes: %d, %s is deleted", recvSize, path);
            delegate->onEvent(FileObserver::Deleted, string(path));
            break;

#if 0
            size_t buffer_i = 0;
            struct inotify_event *pevent;
            size_t event_size, q_event_size;
            queue_entry_t event;
            queue_t q = queue_create();
            int count = 0;
            
            while (buffer_i < recvSize)
            {
                /* Parse events and queue them. */
                pevent = (struct inotify_event*) &buffer[buffer_i];
                event_size =  offsetof(struct inotify_event, name) + pevent->len;
                q_event_size = offsetof(struct queue_entry, inot_ev.name) + pevent->len;
                event = (queue_entry_t)malloc(q_event_size);
                memmove(&(event->inot_ev), pevent, event_size);
                queue_enqueue(event, q);
                buffer_i += event_size;
                count++;
            }
#endif
        }
        usleep(1000 * 1000);
    }

    inotify_rm_watch(fd, IN_ALL_EVENTS);
}

FileObserver::FileObserver(const std::string& path, Delegate* delegate)
{
    assert (delegate);
    mPath = path;
    mDelegate = delegate;
}

bool FileObserver::startWatching()
{
    bool success = false;

    pthread_t threadId;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    ThreadParams* params = new ThreadParams();
    params->delegate = mDelegate;
    params->path = mPath.c_str();
    success = !pthread_create(&threadId, &attributes, ThreadFunc, (void*)params);

    pthread_attr_destroy(&attributes);
    
    return success;
}

void FileObserver::stopWatching()
{
}

