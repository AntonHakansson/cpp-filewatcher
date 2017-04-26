#include <FileWatcher.hpp>

namespace FW {
  FileWatcher::FileWatcher () {
    m_fd = inotify_init();
    if (m_fd < 0)
      std::cout << "FileWatch Error: " << errno << std::endl;

    int flags = fcntl(m_fd, F_GETFL, 0);
    fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);
  }

  FileWatcher::~FileWatcher () {
    auto iter = m_watches.begin();
    auto end = m_watches.end();
    for (; iter != end; ++iter) {
      remove_watch(iter->first);
    }

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
        perror("Directory");
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
    if (std::chrono::duration_cast<std::chrono::duration<float>>(
            std::chrono::system_clock::now() - m_prev_update) <
        m_update_delay) {
          return;
    }

    static char buffer[BUF_LEN];

    int length = read(m_fd, buffer, BUF_LEN);
    int i = 0;

    if ( length < 0 && errno == EAGAIN)
      return; // This means there is no data to be read

    if ( length < 0 ) {
      perror( "read" );
      return;
    }

    while (i < length) {
      struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

      WatchId id = event->wd;
      const String& filename = event->name;
      auto event_mask = event->mask;

      handle_action (id, filename, event_mask);
      i += EVENT_SIZE + event->len;
    }

    m_prev_update = std::chrono::system_clock::now();
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
