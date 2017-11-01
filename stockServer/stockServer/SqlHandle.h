//
//  SqlHandle.hpp
//  webServer
//
//  Created by gongkun on 2017/10/12.
//  Copyright © 2017 gongkun. All rights reserved.
//
/*
 * sql operation
 */


#ifndef SqlHandle_h
#define SqlHandle_h

#include <stdio.h>
#include <string>

//mongodb
#include "Poco/MongoDB/MongoDB.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/Database.h"
#include "Poco/MongoDB/Cursor.h"
#include "Poco/MongoDB/Array.h"

#include "ShZdFutureUserApiStruct.h"

using Poco::MongoDB::Connection;
using Poco::MongoDB::Database;
using Poco::MongoDB::InsertRequest;
using Poco::SharedPtr;
using Poco::MongoDB::Cursor;
using Poco::MongoDB::ResponseMessage;
using Poco::MongoDB::Document;

class SqlHandle{
private:
	char* dbName;
	Connection* connect;
	Database* db;
	const char* exangeCollectionName;
	const char* instrumentCollectionName;
	const char* marketCollectionName;
public:
    SqlHandle(char* dbName, char* ip, int port);
    ~SqlHandle();
    int insert(const char* collection, const char** keys, const char** values, int num);
	int insertExanges(CTShZdExchangeField*);
	int insertInstruments(CTShZdInstrumentField*);
	int insertMarketData(CTShZdDepthMarketDataField*);
	int select(const char* collection, const char** keys, int num);
	
	//if the exCode exist
	bool checkExchange(const char* exchangeID);

};

#endif /* SqlHandle_hpp */


















