# cpp-filewatcher
Lightweight modern c++11 file listener for linux os

# Build
```sh

mkdir build
cd build
cmake ..
make

```

# Example usage
```c++


#include <iostream>
#include <string>

#include "../include/FileWatcher.hpp"

// 1) Build lib 
// 2) Create directory "dir" in same directory as main.cpp
// 3) Compile sample program with:
//     g++ main.cpp -L../build -lcpp-filewatcher -o sample.out
// 4) Make edits to dir directory to watch changes

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
