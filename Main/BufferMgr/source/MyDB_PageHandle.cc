
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include <iostream>

#include "MyDB_PageHandle.h"
#include "MyDB_BufferManager.h"

void *MyDB_PageHandleBase :: getBytes () {
	//store the spare address in the buffer that we will return
	void* spareAddr;
	pair<MyDB_TablePtr, long> curId = this->pagePtr->pageId;

	//map<pair<MyDB_TablePtr, long>, MyDB_Page>::iterator it_pageMap = this->bufferManager->pageMap.find(curId);
	//If find this page in buffer
	if (this->pagePtr->addr != nullptr) {
		this -> bufferManager -> LRU -> adjust(this -> pagePtr -> pageId);
		spareAddr = this->pagePtr->addr;

	}

	//If this page is in the disk and we need to read from disk and put it in the buffer
	else {
		//If there is no empty space in the buffer
		if (this->bufferManager->availPageQueue.empty()) {
			spareAddr = this -> bufferManager -> LRU -> evictAndReplace(this -> pagePtr -> pageId, this -> pagePtr);
		}
		else {
			this -> bufferManager -> LRU -> add(this -> pagePtr -> pageId, this -> pagePtr);
			spareAddr = this -> bufferManager -> availPageQueue.front();
			this -> bufferManager -> availPageQueue.pop();
		}
		this -> pagePtr -> addr = spareAddr;
		this -> pagePtr -> readFromDisk(this -> pagePtr -> pageId.first, this -> pagePtr -> pageId.second, this -> pagePtr ->addr);
	}

	return spareAddr;
}

void MyDB_PageHandleBase :: wroteBytes () {
	this -> pagePtr -> isDirty = true;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	this -> pagePtr -> reduceCount();
}

#endif

