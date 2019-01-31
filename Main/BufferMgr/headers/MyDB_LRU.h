#ifndef MYDB_LRU_H
#define MYDB_LRU_H

#include <memory>
#include <map>
#include "MyDB_BufferManager.h"

using namespace std;
class Node;

class MyDB_LRU{

public:
    void adjust(pair<MyDB_TablePtr, long> curId);
    void add(pair<MyDB_TablePtr, long> curId, MyDB_PageBase* page);
    void* evictAndReplace(pair<MyDB_TablePtr, long> curId, MyDB_PageBase* page);

    MyDB_LRU(int size);

    Node* head;
    Node* tail;
    int size;
    map<pair<MyDB_TablePtr, long>, Node*> LRUmap;
};

class Node{

public:
    Node* pre;
    Node* next;
    MyDB_PageBase* page;
};
#endif //MYDB_LRU_H
