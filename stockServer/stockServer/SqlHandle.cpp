//
//  SqlHandle.cpp
//  webServer
//
//  Created by gongkun on 2017/10/12.
//  Copyright © 2017 gongkun. All rights reserved.
//


#include "SqlHandle.h"
using namespace std;

SqlHandle::SqlHandle(char* dbName, char* ip, int port)
{
	int nameLen = strlen(dbName);
	this->dbName = new char[nameLen + 1];
	memcpy(this->dbName, dbName, nameLen);
	this->dbName[nameLen] = 0;
	connect = new Connection(ip, port);
	db = new Database(dbName);
}

SqlHandle::~SqlHandle()
{
	connect->disconnect();
	delete [] dbName;
	delete connect;
	delete db;
}

int SqlHandle::insert(const char* collection, const char** keys, const char** values, int num)
{
	std::cout << "*** INSERT ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(collection);

	// With one insert request, we can add multiple documents
	Document& doc = insertRequest->addNewDocument();
	for (int i=0; i < num; i++) {
			doc.add(keys[i], (const char*)values[i]);
	}

	std::cout << insertRequest->documents().size() << std::endl;

	connect->sendRequest(*insertRequest);
	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Last Error: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
}

int SqlHandle::select(const char* collection, const char** keys, int num)
{
	std::cout << "*** SELECT ***" << std::endl;
	Cursor cursor(dbName, collection);	
	for (int i=0; i < num; i ++) {
		cout << keys[i] << endl;
		cursor.query().returnFieldSelector().add(keys[i], 1);
	}

	ResponseMessage& response = cursor.next(*connect);
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; ++it)
		{
			for (int i=0; i < num; i ++)
				std::cout << (*it)->get<std::string>(keys[i]) << " ";
			cout << std::endl;
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	}
	return 0;
}