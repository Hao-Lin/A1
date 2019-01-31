#ifndef MYDB_PAGE_H
#define MYDB_PAGE_H
#include <memory>
#include "MyDB_Table.h"
#include <map>
//#include "MyDB_BufferManager.h"
using namespace std;

class MyDB_PageBase;
class MyDB_BufferManager;

typedef shared_ptr<MyDB_PageBase>  MyDB_Page;

class MyDB_PageBase{

public:
    // Page address in the Buffer
    void* addr;

    // Whether is pined
    bool isPined;

    // The offset of the anonymous page(-1 if is not anonymous)
    int slotId;

    // Whether is dirty
    bool isDirty;

    // The number of handles of this page
    int handleCount;

    // This page record which table(*pageid[0]) and which number of data-page of this table(*pageid[1])
    pair<MyDB_TablePtr, long> pageId;

    // Reference to the buffer manager
    MyDB_BufferManager* bufferManager;

    // Read data from the disk
    void readFromDisk(MyDB_TablePtr table, long i, void* addr);

    // Write data bake to disk
    void writeToDisk(void* data);

    //Constructor with parameters
    MyDB_PageBase(void*, bool, size_t, bool, int, pair<MyDB_TablePtr, long>, MyDB_BufferManager*);

    //Reduce the count number of this page and decide if recycle
    void reduceCount();
};

#endif //MYDB_PAGE_H