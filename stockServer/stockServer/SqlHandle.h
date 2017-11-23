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
#include <vector>

//mongodb
#include "Poco/MongoDB/MongoDB.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/Database.h"
#include "Poco/MongoDB/Cursor.h"
#include "Poco/MongoDB/Array.h"

#include "ShZdFutureUserApiStruct.h"

#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/StreamCopier.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/PrintHandler.h"

//json
using namespace Poco::Dynamic;
using namespace Poco;


using Poco::MongoDB::Connection;
using Poco::MongoDB::Database;
using Poco::MongoDB::InsertRequest;
using Poco::SharedPtr;
using Poco::MongoDB::Cursor;
using Poco::MongoDB::ResponseMessage;
using Poco::MongoDB::Document;
using std::vector;
using std::string;

class SqlHandle{
private:
	const char* ip;
	const int port;
	const char* dbName;
	Connection* connect;
	Database* db;
public:
	const char* exangeCollectionName;
	const char* instrumentCollectionName;
	const char* marketCollectionName;

	SqlHandle();
    SqlHandle(char* dbName, char* ip, int port);
    ~SqlHandle();
    int insert(const char* collection, const char** keys, const char** values, int num);
	int insertExanges(CTShZdExchangeField*);
	int insertInstruments(CTShZdInstrumentField*);
	int insertMarketData(CTShZdDepthMarketDataField*);
	int query(const char* collection, const char** keys, int num);
	int queryInstruments();
	int queryKLE();
	
	//query the exchanges
	vector<string> queryExchanges();
	//query the products
	vector<string> queryProduct(const char* exchangeID);
	//query the instrument
	JSON::Array queryInsts(const char *exchangeID,const char* productID,const char* end_time);
	//if the exCode exist
	bool checkExchange(const char* exchangeID);

};

#endif /* SqlHandle_hpp */


















