#include <FileWatcher.hpp>

namespace FW {
  FileWatcher::FileWatcher () {
    m_fd = inotify_init();
    if (m_fd < 0)
      std::cout << "FileWatch Error: " << errno << std::endl;
  }

  FileWatcher::~FileWatcher () {
    m_watches.clear();
    close (m_fd);
  }

  WatchId FileWatcher::add_watch(const String &directory, const Callback &callback) {
    int wd = inotify_add_watch(m_fd, directory.c_str(),
                               IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE |
                                   IN_MOVED_FROM | IN_DELETE);
    if (wd < 0) {
      if (errno == ENOENT)
        std::cout << "FileWatch directory not found: "  << directory.c_str() << errno << std::endl;
      else
        std::cout << "FileWatch Error: " << errno << std::endl;
    }

    m_watches.insert( {wd, {callback, directory}} );
    return wd;
  }

  void FileWatcher::remove_watch(const String &directory) {
    auto iter = m_watches.begin();
    auto end = m_watches.end();
    for (; iter != end; ++iter) {
      if (directory == iter->second.dir_name) {
        remove_watch (iter->first);
        return;
      }
    }
  }

  void FileWatcher::remove_watch(WatchId watch_id) {
    auto iter = m_watches.find(watch_id);
    if (iter == m_watches.end())
      return;

    inotify_rm_watch(m_fd, iter->first);

    m_watches.erase(iter);
  }

  void FileWatcher::update () {
    static char buffer[BUF_LEN];

    int length = read(m_fd, buffer, BUF_LEN);
    int i = 0;

    if ( length < 0 ) {
      perror( "read" );
    }

    while (i < length) {
      struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
      WatchId id = event->wd;
      handle_action (id, event->name, event->mask);
      i += EVENT_SIZE + event->len;
    }

  }

  void FileWatcher::handle_action(WatchId watch_id, const String &filename, unsigned long action) {
    auto iter = m_watches.find(watch_id);
    if (iter == m_watches.end())
      return;

    if (IN_CLOSE_WRITE & action) {
      iter->second.callback(watch_id, iter->second.dir_name, filename, Action::Modified);
    }
    if (IN_MOVED_TO & action || IN_CREATE & action) {
      iter->second.callback(watch_id, iter->second.dir_name, filename, Action::Add);
    }
    if (IN_MOVED_FROM & action || IN_DELETE & action) {
      iter->second.callback(watch_id, iter->second.dir_name, filename, Action::Delete);
    }
  }

}
