cmake_minimum_required(VERSION 3.12)

project(ASIOTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")



if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "default is Debug" FORCE)
endif(NOT CMAKE_BUILD_TYPE)


# CPP扩展
set(CPP_HEADER_EXT "*.h" "*.hpp" "*.hrp" "*.hxx")
set(CPP_SOURCE_EXT "*.cpp" "*.cxx" "*.cc")




#输出集中存放
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release/arc")


add_subdirectory(src)
# add_subdirectory(echoServer)
add_subdirectory(sslServer)