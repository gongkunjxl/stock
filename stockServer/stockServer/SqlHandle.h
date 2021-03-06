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
#include "Poco/Timer.h"
#include "global.h"
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
using Poco::Timer;
using Poco::TimerCallback;
using std::vector;
using std::string;

class SqlHandle{
private:
	const char* ip;
	const int port;
	const char* dbName;
	Connection* connect;
	Database* db;
	const char* exangeCollectionName;
	const char* instrumentCollectionName;
	const char* marketCollectionName;
	const char* filledDataCollectionName;
	const char* minKlineCollectionName;
	const char* hourKlineCollectionName;
	const char* dayKlineCollectionName;
	const char* monKlineCollectionName;
	const char* lastKlineCollectionName;
	const char* lastMarketCollectionName;
public:
	SqlHandle();
    SqlHandle(char* dbName, char* ip, int port);
    ~SqlHandle();
	void updateKline(Timer& timer);
	void updateHourKline(time_t);
	void updateDayKline(time_t);
	void updateMonKline(time_t);
    int insert(const char* collection, const char** keys, const char** values, int num);
	int insertExanges(CTShZdExchangeField*);
	int insertInstruments(CTShZdInstrumentField*);
	int insertDeptMarketData(CTShZdDepthMarketDataField*);
	int insertFilledData(CTShZdFilledDataField*);
	//int insertKline(CTShZdDepthMarketDataField*);
	int insertKline(string, vector<JSON::Object>);
	int query(const char* collection, const char** keys, int num);
	int queryInstruments();
	TShZdPriceType quertLastMinPrice(string InstrumentID);
	
	vector<JSON::Object> queryKLE(string type, string InstrumentID, time_t begTime, time_t endTime);
	vector<JSON::Object> query_latest_1min_market(time_t);
	vector<JSON::Object> query_latest_1hour_minkline(time_t);
	vector<JSON::Object> query_latest_1day_hourkline(time_t);
	vector<JSON::Object> query_latest_1mon_daykline(time_t);
	//query the exchanges
	vector<string> queryExchanges();
	//query the products
	vector<string> queryProduct(const char* exchangeID);
	//query the instrument
	JSON::Array queryInsts(const char *exchangeID,const char* productID,const char* end_time);
	//query data frmo market
	JSON::Array queryMarket(const char*exchangeID,const char* instrumentID);

	//query market EMR query
	JSON::Array SqlHandle::queryMarketEMR(const char *exchangeID,const char* instrumentID);

	//if the exCode exist
	bool checkExchange(const char* exchangeID);
	//delete market data before time_t
	int deleteMarketBefore(time_t);

};

#endif /* SqlHandle_hpp */

















