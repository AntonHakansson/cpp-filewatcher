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
#include <chrono>
#include <utility> // make_pair
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

namespace FW {

  using namespace std::chrono_literals;

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
    WatchId id;
    bool recursive;
  };

  typedef std::map<WatchId, WatchStruct> WatchMap;

  class FileWatcher {
  public:
    FileWatcher ();
    virtual ~FileWatcher ();

    WatchId add_watch (const String& directory, bool recursive, const Callback& callback);

    void remove_watch (const String& directory);

    void remove_watch (WatchId watch_id);

    void update ();

  private:

    void handle_action (WatchId, const String& filename, unsigned long action);
    void remove_subwatches (const String& root_dir);

    WatchMap m_watches;

    std::chrono::milliseconds m_update_delay{500ms};
    std::chrono::time_point<std::chrono::system_clock> m_prev_update;

    // Inotify file descriptor
    int m_fd;
  };

}
