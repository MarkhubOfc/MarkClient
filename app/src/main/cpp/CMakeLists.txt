cmake_minimum_required(VERSION 3.22.1)
project("markclient")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -frtti")

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/main/cpp/*.cpp")

file(GLOB IMGUI_SOURCES
  "${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp"
)

add_library(markclient SHARED 
  ${SOURCES}
  ${IMGUI_SOURCES}
)

target_include_directories(markclient PRIVATE
  ${CMAKE_SOURCE_DIR}/src/main/cpp
  ${CMAKE_SOURCE_DIR}/src/main/cpp/vendor/imgui
)

target_link_libraries(markclient
  android
  log
  EGL
  GLESv3
)
