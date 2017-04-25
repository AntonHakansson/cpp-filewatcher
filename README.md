# cpp-filewatcher
Lightweight file listener for linux os


```c++


#include <iostream>
#include <string>

#include "../include/FileWatcher.hpp"

// 1) create directory dir is same directory as main.cpp
// 2) Compile with:
//     g++ main.cpp -L../build -lcpp-filewatcher -o sample.out
// 3) Make edits to dir directory

void callback(FW::WatchId, std::string dir, std::string f, FW::Action action) {
  std::cout << dir << "/" << f << " " << (int)action <<  std::endl;
}

int main() {

  FW::FileWatcher filewatcher;
  filewatcher.add_watch("./dir", callback);

  while (true) {
    filewatcher.update();
  }

  return 0;
}


```
