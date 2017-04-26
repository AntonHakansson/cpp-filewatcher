
#include <iostream>
#include <string>

#include "../include/FileWatcher.hpp"

// 1) Build lib
// 2) Create directory "dir" in same directory as main.cpp
// 3) Compile sample program with:
//     g++ main.cpp -L../build -lcpp-filewatcher -o sample.out
// 4) Make edits to dir directory to watch changes

void callback(FW::WatchId watch_id, std::string dir, std::string filename, FW::Action action) {
  switch(action) {
    case FW::Action::Add:
       std::cout << "File (" << dir + "/" + filename << ") Added! " <<  std::endl;
       break;
    case FW::Action::Delete:
       std::cout << "File (" << dir + "/" + filename << ") Deleted! " << std::endl;
       break;
    case FW::Action::Modified:
       std::cout << "File (" << dir + "/" + filename << ") Modified! " << std::endl;
       break;
    default:
       std::cout << "Should never happen!" << std::endl;
  }
}

int main() {

  FW::FileWatcher filewatcher;
  filewatcher.add_watch("./dir", callback);

  while (true) {
    filewatcher.update();
  }

  return 0;
}
