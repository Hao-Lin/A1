#ifndef MYDB_PAGE_C
#define MYDB_PAGE_C
#include <memory>
#include <iostream>
#include "MyDB_BufferManager.h"
#include "MyDB_Page.h"
#include <fcntl.h> //open
#include <unistd.h> //read write close lseek
using namespace std;

// Read data from the disk
void MyDB_PageBase:: readFromDisk(MyDB_TablePtr table, long i, void* addr) {
    //char* addr = (char*)malloc(bufferManager -> getPageSize());
    //int open(const char *pathname, int oflag, ... /* mode_t mode */); return file descriptor is success
    //pathname is the path name of the file(C:/cpp/a.cpp), oflag is the open mode, third parameter is access permission bits(used for create file)
    int fd = open(table -> getStorageLoc().c_str(), O_CREAT|O_SYNC|O_RDWR, 0666);
    //lseek(int fildes, off_t offset, int whence); lseek change the position where we read/write in a file
    lseek(fd, i * bufferManager -> getPageSize(), SEEK_SET);
    // read(int filedes, void *buf, size_t nbytes); read data(size of nbytes) from filedes to buf
    read(fd, (char*)addr, bufferManager -> getPageSize());
    close(fd);
    //return (void*)addr;
}


// Write data bake to disk
void MyDB_PageBase:: writeToDisk(void* data) {
    int fd = open(pageId.first -> getStorageLoc().c_str(), O_CREAT|O_SYNC|O_RDWR, 0666);
    lseek(fd, pageId.second * bufferManager -> getPageSize(), SEEK_SET);
    //write(int filedes, const void *buf, size_t nbytes); write nbytes data from buf to filedes
    write(fd, (char*)data, bufferManager -> getPageSize());
    close(fd);
    this -> isDirty = false;
}

void MyDB_PageBase:: reduceCount() {

    this -> handleCount--;
    // if count turn to 0, decided if recycle
    if (handleCount == 0) {
        // if this page is a normal page
        if (slotId == -1) {
            // set the normal page unpined no matter if it is in the buffer
            this -> isPined = false;
        }
        // this page is anonymous
        else {
            this -> isPined = false;
            // if this page is in the buffer
            if (this -> addr != nullptr) {
                this -> bufferManager -> availPageQueue.push(this -> addr);
                this -> bufferManager -> slotQueue.push(this -> slotId);
            }
            // if this page is not in the buffer
            else {
                this -> bufferManager -> slotQueue.push(this -> slotId);
            }
            //delete this;
        }
    }
}

// constructor
MyDB_PageBase :: MyDB_PageBase(void* addr, bool isPined , size_t slotId, bool isDirty, int handleCount, pair<MyDB_TablePtr, long> pageId, MyDB_BufferManager* mgr) {
    this->addr = addr;
    this->isPined = isPined;
    this->slotId = slotId;
    this->isDirty = isDirty;
    this->handleCount = handleCount;
    this->pageId = pageId;
    this->bufferManager = mgr;
}

#endif