
#ifndef CATALOG_UNIT_H
#define CATALOG_UNIT_H

#include "MyDB_BufferManager.h"
#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include "QUnit.h"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

// these functions write a bunch of characters to a string... used to produce data
void writeNums (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '0' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeLetters (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = 'i' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeSymbols (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '!' + (i % 10);
	}
	bytes[len - 1] = 0;
}

int main () {

	QUnit::UnitTest qunit(cerr, QUnit::verbose);

	// UNIT TEST 1: A BIG ONE!!
	{

		// create a buffer manager
		MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");

		// allocate a pinned page
		MyDB_PageHandle pinnedPage = myMgr.getPinnedPage (table1, 0);
		char *bytes = (char *) pinnedPage->getBytes ();
		writeNums (bytes, 64, 0);
		pinnedPage->wroteBytes ();


		// create a bunch of pinned pages and remember them
		vector <MyDB_PageHandle> myHandles;
		for (int i = 1; i < 10; i++) {
			MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
			char *bytes = (char *) temp->getBytes ();
			writeNums (bytes, 64, i);
			temp->wroteBytes ();
			myHandles.push_back (temp);
		}

		// now forget the pages we created
		vector <MyDB_PageHandle> temp;
		myHandles = temp;

		// now remember 8 more pages
		for (int i = 0; i < 8; i++) {
			MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
			char *bytes = (char *) temp->getBytes ();

			// write numbers at the 0th position
			if (i == 0)
				writeNums (bytes, 64, i);
			else
				writeSymbols (bytes, 64, i);
			temp->wroteBytes ();
			myHandles.push_back (temp);
		}

		// now correctly write nums at the 0th position
		MyDB_PageHandle anotherDude = myMgr.getPage (table1, 0);
		bytes = (char *) anotherDude->getBytes ();
		writeSymbols (bytes, 64, 0);
		anotherDude->wroteBytes ();

		// now do 90 more pages, for which we forget the handle immediately
		for (int i = 10; i < 100; i++) {
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char *bytes = (char *) temp->getBytes ();
			writeNums (bytes, 64, i);
			temp->wroteBytes ();
		}

		// now forget all of the pinned pages we were remembering
		vector <MyDB_PageHandle> temp2;
		myHandles = temp2;

		// now get a pair of pages and write them
		for (int i = 0; i < 100; i++) {
			MyDB_PageHandle oneHandle = myMgr.getPinnedPage ();
			char *bytes = (char *) oneHandle->getBytes ();
			writeNums (bytes, 64, i);
			oneHandle->wroteBytes ();
			MyDB_PageHandle twoHandle = myMgr.getPinnedPage ();
			writeNums (bytes, 64, i);
			twoHandle->wroteBytes ();
		}

		// make a second table
		MyDB_TablePtr table2 = make_shared <MyDB_Table> ("tempTable2", "barfoo");
		for (int i = 0; i < 100; i++) {
			MyDB_PageHandle temp = myMgr.getPage (table2, i);
			char *bytes = (char *) temp->getBytes ();
			writeLetters (bytes, 64, i);
			temp->wroteBytes ();
		}
	}

	// UNIT TEST 2
	{
		MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");

		// look up all of the pages, and make sure they have the correct numbers
		for (int i = 0; i < 100; i++) {
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char answer[64];
			if (i < 8)
				writeSymbols (answer, 64, i);
			else
				writeNums (answer, 64, i);
			char *bytes = (char *) temp->getBytes ();
			QUNIT_IS_EQUAL (string (answer), string (bytes));
		}
	}

    // UNIT TEST 3
    // anonymous page
    {
        MyDB_BufferManager myMgr(64, 2, "tempDSFSD");
        MyDB_TablePtr table1 = make_shared<MyDB_Table>("tempTable", "foobar");

        MyDB_PageHandle ph1 = myMgr.getPinnedPage();
        char *bytes1 = (char *) ph1->getBytes();
        writeNums(bytes1, 64, 0);
        ph1->wroteBytes();

        char ph1Answer[64];
        writeNums(ph1Answer, 64, 0);
        QUNIT_IS_EQUAL (string (ph1Answer), string (bytes1));

        MyDB_PageHandle ph2 = myMgr.getPage();
        char *bytes2 = (char *) ph2->getBytes();
        writeNums(bytes2, 64, 1);
        ph2->wroteBytes();

        char ph2Answer[64];
        writeNums(ph2Answer, 64, 1);
        QUNIT_IS_EQUAL (string (ph2Answer), string (bytes2));

        MyDB_PageHandle ph3 = myMgr.getPage();
        char *bytes3 = (char *) ph3->getBytes();
        writeNums(bytes3, 64, 2);
        ph3->wroteBytes();

        char ph3Answer[64];
        writeNums(ph3Answer, 64, 2);
        QUNIT_IS_EQUAL (string (ph3Answer), string (bytes3));

        bytes1 = (char *) ph1->getBytes();
        QUNIT_IS_EQUAL (string (ph1Answer), string (bytes1));

        bytes2 = (char *) ph2->getBytes();
        QUNIT_IS_EQUAL (string (ph2Answer), string (bytes2));

        bytes3 = (char *) ph3->getBytes();
        QUNIT_IS_EQUAL (string (ph3Answer), string (bytes3));
    }

      //UNIT TEST 4
      //anonymous page
    {
        MyDB_BufferManager myMgr(64, 5, "tempDSFSD");
        MyDB_TablePtr table1 = make_shared<MyDB_Table>("tempTable", "foobarfoo");
        vector<MyDB_PageHandle> temp;
        for (int i = 0; i < 8; i++) {
            MyDB_PageHandle ph;
            if (i % 2 == 0) {
                ph = myMgr.getPage();
            } else {
                ph = myMgr.getPinnedPage();
            }
            char *bytes = (char *) ph->getBytes();
            writeNums(bytes, 64, i + 1);
            ph->wroteBytes();
            temp.push_back(ph);

            int j = 0;
            char answer[64];
            for (vector<MyDB_PageHandle>::iterator it = temp.begin(); it != temp.end(); ++it) {

                if (i == j) {
                    writeNums(answer, 64, j + 1);
                } else {
                    writeNums(answer, 64, j);
                }
                j++;
                MyDB_PageHandle ph1 = *it;
                QUNIT_IS_EQUAL (string((char *) ph1->getBytes()), string (answer));
            }

            writeNums(bytes, 64, i);
            ph->wroteBytes();
        }
    }

    //  UNIT TEST 5
    //  anonymous page

    {
        MyDB_BufferManager myMgr(64, 5, "tempDSFSD");
        MyDB_TablePtr table1 = make_shared<MyDB_Table>("tempTable", "foobarfoo");
        vector<MyDB_PageHandle> temp;
        for (int i = 0; i < 8; i++) {
            MyDB_PageHandle ph;
            if (i % 2 == 0) {
                ph = myMgr.getPage();
            } else {
                ph = myMgr.getPinnedPage();
            }
            char *bytes = (char *) ph->getBytes();
            writeNums(bytes, 64, i);
            ph->wroteBytes();
            temp.push_back(ph);

            int j = 0;
            char answer[64];
            for (vector<MyDB_PageHandle>::iterator it = temp.begin(); it != temp.end(); ++it) {

                writeNums(answer, 64, j);
                j++;
                MyDB_PageHandle ph1 = *it;
                QUNIT_IS_EQUAL (string((char *) ph1->getBytes()), string (answer));
            }
        }
    }

    // UNIT TEST 6
    // non-anonymous page & anonynmous page
    {
        MyDB_BufferManager myMgr(64, 5, "tempDSFSD");
        MyDB_TablePtr table = make_shared<MyDB_Table>("tempTable", "foobarfoo");
        vector<MyDB_PageHandle> temp;
        for (int i = 0; i < 8; i++) {
            MyDB_PageHandle ph;
            if (i % 2 == 0) {
                ph = myMgr.getPinnedPage(table, i);
            } else {
                ph = myMgr.getPage();
            }
            char *bytes = (char *) ph->getBytes();
            writeNums(bytes, 64, i+1);
            ph->wroteBytes();
            temp.push_back(ph);

            int j = 0;
            char answer[64];
            for (vector<MyDB_PageHandle>::iterator it = temp.begin(); it != temp.end(); ++it) {

                if (i == j) {
                    writeNums(answer, 64, j + 1);
                } else {
                    writeNums(answer, 64, j);
                }
                j++;

                MyDB_PageHandle ph1 = *it;
                QUNIT_IS_EQUAL (string((char *) ph1->getBytes()), string (answer));
            }

            bytes = (char *) ph->getBytes();
            writeNums(bytes, 64, i);
            ph->wroteBytes();
        }
    }

    // UNIT TEST 7
    // non-anonymous page
    {
        MyDB_BufferManager myMgr(64, 5, "tempDSFSD");
        MyDB_TablePtr table = make_shared<MyDB_Table>("tempTable", "foobarfoo");
        vector<MyDB_PageHandle> temp;
        for (int i = 0; i < 8; i++) {
            MyDB_PageHandle ph;
            if (i % 2 == 0) {
                ph = myMgr.getPinnedPage(table, i);
            } else {
                ph = myMgr.getPage(table, i);
                char* bytes = (char*)ph->getBytes();
                writeNums(bytes, 64, 0);
                ph->wroteBytes();
            }

            char answer[64];

            if (i % 2 == 0) {
                writeNums(answer, 64, i);
            } else {
                writeNums(answer, 64, 0);
            }

            QUNIT_IS_EQUAL (string((char *) ph->getBytes()), string (answer));
        }
    }
// UNIT TEST 8
    // non-anonymous page
    {
        MyDB_BufferManager myMgr(64, 1, "tempDSFSD");
        MyDB_TablePtr table = make_shared<MyDB_Table>("tempTable", "foobarfoo");

        for (int i = 0; i < 8; i++) {
            MyDB_PageHandle ph;
            if (i % 2 == 0) {
                ph = myMgr.getPage(table, i);
            } else {
                ph = myMgr.getPage(table, i);
                char* bytes = (char*)ph->getBytes();
                writeNums(bytes, 64, i);
                ph->wroteBytes();
            }

            char answer[64];
            writeNums(answer, 64, i);

            QUNIT_IS_EQUAL (string((char *) ph->getBytes()), string (answer));
        }

        MyDB_BufferManager myMgr2(64, 1, "tempDSFSD2");
        for (int i = 0; i < 8; i++) {
            MyDB_PageHandle ph;
            ph = myMgr.getPage(table, i);
            char answer[64];
            writeNums(answer, 64, i);

            QUNIT_IS_EQUAL (string((char *) ph->getBytes()), string (answer));
        }
    }
}

#endif
