# Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.8)
# Name your project here
set(PNAME cpp-filewatcher)
project(${PNAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")

include_directories(include)
include_directories(src)

add_library(${PROJECT_NAME}
  src/FileWatcher.cpp
)

# Set location for library installation
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/FileWatcher DESTINATION /usr/include)

# Link to boost libraries
find_package(Boost REQUIRED COMPONENTS system filesystem)
target_link_libraries(${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
add_definitions(-DBOOST_LOG_DYN_LINK)
