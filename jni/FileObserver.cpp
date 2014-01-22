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

    char watchPath[256] = {0};
    sprintf(watchPath, path, strlen(path));
    int wd = inotify_add_watch(fd, watchPath, IN_ALL_EVENTS);
    if (wd == -1)
    {
        XLOG("inotify_add_watch failed");
        return NULL;
    }

    XLOG("start watching %s\n", path);
    while (true)
    {
        const int bufferSize = 16384;
        char buffer[bufferSize];
        ssize_t recvSize = read(fd, buffer, bufferSize);
        if (recvSize < 0)
        {
            XLOG("inotify read error");
            break;
        }

        queue_t q = queue_create();
        struct inotify_event *pevent;
        size_t event_size = 0;
        size_t q_event_size = 0;
        size_t buffer_i = 0;
        queue_entry_t event;
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

        char* cur_event_filename = NULL;
        char* cur_event_file_or_dir = NULL;
        int cur_event_wd;
        int cur_event_cookie;
        while (event = queue_dequeue(q))
        {
            if (event->inot_ev.len)
                cur_event_filename = event->inot_ev.name;

            if ( event->inot_ev.mask & IN_ISDIR )
                cur_event_file_or_dir = "Dir";
            else
                cur_event_file_or_dir = "File";

            cur_event_wd = event->inot_ev.wd;
            cur_event_cookie = event->inot_ev.cookie;
            
            switch (event->inot_ev.mask & 
                   (IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED))
            {
                /* File was accessed */
                case IN_ACCESS:
                    XLOG("ACCESS: %s \"%s\" on WD #%i\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd);
                    break;
  
                /* File was modified */
                case IN_MODIFY:
                    XLOG("MODIFY: %s \"%s\" on WD #%i\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd);
                    break;
  
                /* File changed attributes */
                case IN_ATTRIB:
                    XLOG("ATTRIB: %s \"%s\" on WD #%i\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd);
                    break;
  
                /* File open for writing was closed */
                case IN_CLOSE_WRITE:
                    XLOG("CLOSE_WRITE: %s \"%s\" on WD #%i\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd);
                    break;
  
                /* File open read-only was closed */
                case IN_CLOSE_NOWRITE:
                    XLOG("CLOSE_NOWRITE: %s \"%s\" on WD #%i\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd);
                    break;
  
                /* File was opened */
                case IN_OPEN:
                    XLOG("OPEN: %s \"%s\" on WD #%i\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd);
                    break;
  
                /* File was moved from X */
                case IN_MOVED_FROM:
                    XLOG("MOVED_FROM: %s \"%s\" on WD #%i. Cookie=%d\n",
              	      cur_event_file_or_dir, cur_event_filename, cur_event_wd, 
                        cur_event_cookie);
                    break;

                /* File was deleted */
                case IN_DELETE_SELF:
                    XLOG("DELETE_SELF: %s \"%s\" deleted", cur_event_file_or_dir,
                        cur_event_filename);
                    delegate->onEvent(FileObserver::Deleted, path);
                    break;
  
                /* Watch was removed explicitly by inotify_rm_watch or automatically
                              because file was deleted, or file system was unmounted.  */
                case IN_IGNORED:
                    XLOG("IGNORED: WD #%d\n", cur_event_wd);
                    break;
  
                /* Some unknown message received */
                default:
                    XLOG("UNKNOWN EVENT \"%X\" OCCURRED for file \"%s\" on WD #%i\n",
              	      event->inot_ev.mask, cur_event_filename, cur_event_wd);
                    break;
            }
        }

        queue_dequeue(q);
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

