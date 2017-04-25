#pragma once

#define FILEWATCHER_PLATFORM_WIN32 1
#define FILEWATCHER_PLATFORM_LINUX 2
#define FILEWATCHER_PLATFORM_KQUEUE 3

#if defined(_WIN32)
#	define FILEWATCHER_PLATFORM FILEWATCHER_PLATFORM_WIN32
#elif defined(__APPLE_CC__) || defined(BSD)
#	define FILEWATCHER_PLATFORM FILEWATCHER_PLATFORM_KQUEUE
#elif defined(__linux__)
#	define FILEWATCHER_PLATFORM FILEWATCHER_PLATFORM_LINUX
#endif

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <utility> // make_pair
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

namespace FW {

  typedef std::string String;
  typedef unsigned long WatchId;

  namespace Actions {
    enum class Action {
      Add = 1,
      Delete,
      Modified
    };
  }
  typedef Actions::Action Action;

  typedef std::function<void(WatchId, const String&, const String&, Action)> Callback;
  struct WatchStruct {
    Callback callback;
    String dir_name;
  };

  typedef std::map<WatchId, WatchStruct> WatchMap;

  class FileWatcher {
  public:
    FileWatcher ();
    virtual ~FileWatcher ();

    WatchId add_watch (const String& directory, const Callback& callback);

    void remove_watch (const String& directory);

    void remove_watch (WatchId watch_id);

    void update ();

  private:

    void handle_action(WatchId, const String& filename, unsigned long action);

    WatchMap m_watches;

    // Inotify file descriptor
    int m_fd;
  };

}
