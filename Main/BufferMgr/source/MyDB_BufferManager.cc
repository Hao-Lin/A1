
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_PageHandle.h"
#include "MyDB_LRU.h"
#include <iostream>
#include <string>
#include <stdlib.h>  // malloc

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table, long index) {
	// the handle we will return
	MyDB_PageHandle phd = make_shared<MyDB_PageHandleBase>();
	// Find if this page is in the buffer
	pair<MyDB_TablePtr, long> curId(table, index);
	map<pair<MyDB_TablePtr, long>, MyDB_PageBase*>:: iterator it_pageMap = pageMap.find(curId);

	//If find this page in buffer
	if (it_pageMap != pageMap.end()) {
		phd -> pagePtr = it_pageMap -> second;
	}

	//If this page is in the disk and we need to put it in the buffer
	else {
		//New one page with null buffer address
		MyDB_PageBase* newPage = new MyDB_PageBase (nullptr, false, -1, false, 0, curId, this);
		// Update pageMap
		pageMap.insert(map<pair<MyDB_TablePtr, long>, MyDB_PageBase*>::value_type(curId, newPage));
		phd -> pagePtr = newPage;
	}

	//Update the handleCount of this page and add this new handle's buffer information
	phd -> pagePtr -> handleCount ++;
	phd->bufferManager = this;
	return phd;

}

MyDB_PageHandle MyDB_BufferManager :: getPage () {

	// the handle we will return
	MyDB_PageHandle phd = make_shared<MyDB_PageHandleBase>();

	//New one page
	//If the slotQueue is not empty, then get slotId from the slotQueue
	MyDB_PageBase* newPage;
	if (!slotQueue.empty()) {
		// id for the anonymous page, offset equals to slot
		pair<MyDB_TablePtr, long> curId(tempTable, slotQueue.front());
		newPage = new MyDB_PageBase (nullptr, false, slotQueue.front(), false, 0, curId, this);
		slotQueue.pop();
	}
	//If the slotQueue is empty, means there is no slotId can be recycled, then use the curpos
	else{
		// id for the anonymous page, offset equals to slot
		pair<MyDB_TablePtr, long> curId(tempTable, curpos);
		newPage = new MyDB_PageBase (nullptr, false, curpos, false, 0, curId, this);
		curpos ++;
	}
	// Don't need to update the anonymous page into the pageMap
	phd -> pagePtr = newPage;

	phd -> pagePtr -> handleCount ++;
	phd->bufferManager = this;

	return phd;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table, long index) {
	MyDB_PageHandle phd = getPage (table, index);
	phd -> pagePtr -> isPined = true;
	return phd;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	MyDB_PageHandle phd = getPage ();
	phd -> pagePtr -> isPined = true;
	return phd;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	unpinMe -> pagePtr -> isPined = false;
}

void MyDB_BufferManager::setPageSize(size_t pageSize) {
	MyDB_BufferManager::pageSize = pageSize;
}

size_t MyDB_BufferManager::getPageSize() {
	return pageSize;
}

size_t MyDB_BufferManager::getNumPages() {
	return numPages;
}

void MyDB_BufferManager::setNumPages(size_t numPages) {
	MyDB_BufferManager::numPages = numPages;
}

const string& MyDB_BufferManager::getTempFile() {
	return tempFile;
}

void MyDB_BufferManager::setTempFile(const string &tempFile) {
	MyDB_BufferManager::tempFile = tempFile;
}


MyDB_BufferManager :: MyDB_BufferManager (size_t pSize, size_t pNum, string file) {
	// init private member
	pageSize = pSize;
	numPages = pNum;
	tempFile = file;
	curpos = 0;
	//make a new LRU
	LRU = new MyDB_LRU(numPages);

	//make a temp table for the anonymous pages
	tempTable = make_shared<MyDB_Table>("tempTable", file);

	// malloc space
	buffer = malloc(pageSize * numPages);

	void *ptr = buffer;
	for(int i=0; i<pNum; i++){
		availPageQueue.push((void*)((char*)ptr + i*pSize));
	}
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	map<pair<MyDB_TablePtr, long>, MyDB_PageBase*>:: iterator it_pageMap = pageMap.begin();
	while (it_pageMap != pageMap.end()) {
		if (it_pageMap -> second -> isDirty == true) {
			it_pageMap->second->writeToDisk(it_pageMap->second->addr);
		}
		it_pageMap++;
	}
	free(buffer);
}
	
#endif


