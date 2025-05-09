cmake_minimum_required(VERSION 3.10)
project(chatBot01)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include libcurl headers
include_directories("C:/2024_Fall/cit66_Cpp/libs/curl/curl-8.11.0_2-win64-mingw/include")

# Include json.hpp header
include_directories("C:/2024_Fall/cit66_Cpp/chatBot01/include")

# Add libcurl library directory
link_directories("C:/2024_Fall/cit66_Cpp/libs/curl/curl-8.11.0_2-win64-mingw/lib")

# Add the executable
add_executable(chatBot01 chatBot01.cpp
        testBot01.cpp)

# Link libcurl
target_link_libraries(chatBot01 libcurl)


