cmake_minimum_required(VERSION 3.10)

project(linewal3dxpcmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O0")
set(CMAKE_C_FLAGS "-O0")

include_directories(include/)

add_library(line_wal_3dxp SHARED 
adm.cpp
al.cpp
card.cpp
jamma.cpp
lib.cpp
opengl.cpp
poll.cpp
res.cpp
yac.cpp

shared/line.cpp
)

target_link_libraries(line_wal_3dxp ${CMAKE_SOURCE_DIR}/lib/libSDL2.dll.a ${CMAKE_SOURCE_DIR}/lib/libglfw3.a opengl32 gdi32)