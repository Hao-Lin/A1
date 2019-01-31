cmake_minimum_required(VERSION 3.13)
project(A1)

set(CMAKE_CXX_STANDARD 14)

include_directories(Main/BufferMgr/headers)
include_directories(Main/Catalog/headers)
include_directories(Main/Qunit)
include_directories(Main/Qunit/headers)

add_executable(A1
        Build/SConstruct
        cmake-build-debug/A1
        cmake-build-debug/A1.cbp
        cmake-build-debug/barfoo
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/foobar
        cmake-build-debug/foobarfoo
        cmake-build-debug/Makefile
        cmake-build-debug/tempDSFSD
        Main/BufferMgr/headers/MyDB_BufferManager.h
        Main/BufferMgr/headers/MyDB_LRU.h
        Main/BufferMgr/headers/MyDB_Page.h
        Main/BufferMgr/headers/MyDB_PageHandle.h
        Main/BufferMgr/source/MyDB_BufferManager.cc
        Main/BufferMgr/source/MyDB_LRU.cc
        Main/BufferMgr/source/MyDB_Page.cc
        Main/BufferMgr/source/MyDB_PageHandle.cc
        Main/BufferTest/source/BufferQUnit.cc
        Main/Catalog/headers/MyDB_Table.h
        Main/Catalog/source/MyDB_Table.cc
        Main/Qunit/headers/QUnit.h
        CMakeLists.txt
        README.md)
