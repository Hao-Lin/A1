#ifndef MYDB_PAGE_C
#define MYDB_PAGE_C
#include <memory>
#include <iostream>
#include <string>
#include "MyDB_LRU.h"

using namespace std;

void MyDB_LRU:: add(pair<MyDB_TablePtr, long> curId, MyDB_PageBase* page) {
    Node* node = new Node();
    node -> page = page;
    //node -> page -> addr = addr;
    node -> next = this -> head -> next;
    node -> pre = this -> head;
    this -> head -> next -> pre = node;
    this -> head -> next = node;

    this -> LRUmap.insert(map<pair<MyDB_TablePtr, long>, Node*>::value_type(curId, node));
}

void* MyDB_LRU:: evictAndReplace(pair<MyDB_TablePtr, long> curId, MyDB_PageBase* page) {

    Node* node = this -> tail -> pre;
    while (node -> page -> isPined == true) {
        node = node->pre;
    }
    node -> pre -> next = node -> next;
    node -> next -> pre = node -> pre;

    // consider if write the data back to the disk
    // if this page is a normal page
    if (node -> page -> slotId == -1) {
        node -> page -> writeToDisk(node -> page -> addr);

    }
    // if this page is anonymous page
    else {
        if (node -> page -> handleCount != 0) {
            node -> page -> writeToDisk(node -> page -> addr);
        }
    }

    this -> LRUmap.erase(node -> page -> pageId);

    void* res = node -> page -> addr;
    node -> page -> addr = nullptr;
    //delete node;
    node -> page = page;
    node -> next = this -> head -> next;
    node -> pre = this -> head;
    this -> head -> next -> pre = node;
    this -> head -> next = node;

    this -> LRUmap.insert(map<pair<MyDB_TablePtr, long>, Node*>::value_type(curId, node));
    return res;
}

void MyDB_LRU:: adjust(pair<MyDB_TablePtr, long> curId) {
    map<pair<MyDB_TablePtr, long>, Node*>:: iterator it_LRUmap = LRUmap.find(curId);
    if (it_LRUmap != LRUmap.end()) {
        Node* node = it_LRUmap -> second;
        node -> pre -> next = node -> next;
        node -> next -> pre = node -> pre;
        node -> pre = this -> head;
        node -> next = this -> head -> next;
        this -> head -> next -> pre = node;
        this -> head -> next = node;
    }
}

MyDB_LRU:: MyDB_LRU(int size) {
    this -> size = size;
    this -> head = new Node();
    this -> tail = new Node();
    this -> head -> next = this -> tail;
    this -> tail -> pre = this -> head;
}

#endif