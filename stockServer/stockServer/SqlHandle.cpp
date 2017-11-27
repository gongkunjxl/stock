//
//  SqlHandle.cpp
//  webServer
//
//  Created by gongkun on 2017/10/12.
//  Copyright © 2017 gongkun. All rights reserved.
//


#include "SqlHandle.h"
#include "Poco/StringTokenizer.h"
#include "Poco/MongoDB/UpdateRequest.h"
#include <fstream>

#include <stdint.h>
#include <sstream>
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/StreamCopier.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/PrintHandler.h"

using namespace std;
using namespace Poco::Dynamic;

SqlHandle::SqlHandle() : ip("127.0.0.1"), \
						port(27017), \
						dbName("stock"), \
						instrumentCollectionName("instrument"), \
						marketCollectionName("market"), \
						exangeCollectionName("exchange"), \
						filledDataCollectionName("filled_data"), \
						minKlineCollectionName("min_Kline"), \
						hourKlineCollectionName("hour_Kline"), \
						dayKlineCollectionName("day_Kline"), \
						monKlineCollectionName("mon_Kline"), \
						lastKlineCollectionName("last_Kline")
{
	connect = new Connection(ip, port);
	db = new Database(dbName);
	
	// auth

}

SqlHandle::SqlHandle(char* dbName, char* ip, int port) : \
						ip(ip), \
						port(port), \
						dbName(dbName), \
						instrumentCollectionName("instrument"), \
						marketCollectionName("market"), \
						exangeCollectionName("exchange"), \
						filledDataCollectionName("filled_data"), \
						minKlineCollectionName("min_Kline"), \
						hourKlineCollectionName("hour_Kline"), \
						dayKlineCollectionName("day_Kline"), \
						monKlineCollectionName("mon_Kline"), \
						lastKlineCollectionName("last_Kline")
{
	connect = new Connection(ip, port);
	db = new Database(dbName);
	
	// auth
}

SqlHandle::~SqlHandle()
{
	connect->disconnect();
	delete connect;
	delete db;
}

int mon2day(int year, int mon) {
	int mon_day[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	if ((year % 4 == 0) && (year % 100 != 0)) 
		mon_day[2] = 29;
	return mon_day[mon];
}

void SqlHandle::updateKline(Timer& timer)
{
	cout << "update Min Kline" << endl;
	//query the depth market data in the past 1 min
	time_t now = time(0);
	//time_t now = 1510145901;
	vector<JSON::Object> mars = query_latest_1min_market(now);
	cout << mars.size() << endl;
	
	//split based on InstrumentID
	map<string, vector<JSON::Object>> instMap;
	for (int n=0; n < mars.size(); n ++) {
		string InstrumentID = mars[n].getValue<string>("InstrumentID");
		map<string, vector<JSON::Object>>::iterator iter = instMap.find(InstrumentID);
		if (iter != instMap.end()) {
			iter->second.push_back(mars[n]);
		} else {
			vector<JSON::Object> vec;
			vec.push_back(mars[n]);
			instMap.insert(map<string, vector<JSON::Object>>:: value_type(InstrumentID, vec));
		}
	}

	//calc min Kline of each instrument
	vector<JSON::Object> minKline_nodes;
	map<string, vector<JSON::Object>>::iterator iter = instMap.begin();
	map<string, vector<JSON::Object>>::iterator end = instMap.end();
	for (; iter != end; iter ++) {
		string InstrumentID = iter->first;
		
		//get the last min kline 
		Poco::SharedPtr<Poco::MongoDB::QueryRequest> queryPtr = db->createQueryRequest(lastKlineCollectionName);
		queryPtr->setNumberToReturn(1);
		queryPtr->selector().add("InstrumentID", InstrumentID);
		Poco::MongoDB::ResponseMessage response;
		connect->sendRequest(*queryPtr, response);
		TShZdPriceType lastClosePrice = 0;
		TShZdVolumeType lastTotalVolume = 0;
		if (response.hasDocuments())
		{
			lastClosePrice = response.documents()[0]->get<TShZdPriceType>("ClosePrice");
			lastTotalVolume = response.documents()[0]->get<TShZdVolumeType>("TotalVolume");
			cout << lastClosePrice << " " << lastTotalVolume << endl;
		}
		
		//cout << InstrumentID << endl;
		TShZdPriceType OpenPrice = -1.0;
		TShZdPriceType ClosePrice = -1.0;
		TShZdPriceType HighestPrice = -1.0;
		TShZdPriceType LowestPrice = numeric_limits<double>::max();
		TShZdVolumeType TotalVolume = 0;
		vector<JSON::Object> mars = iter->second;
		for (int i=0; i < mars.size(); i++) {
			TShZdPriceType price = mars[i].getValue<TShZdPriceType>("LastPrice");
			//cout << price << endl;
			if (OpenPrice < 0)
				OpenPrice = price;
			ClosePrice = price;
			if (price > HighestPrice)
				HighestPrice = price;
			if (price < LowestPrice)
				LowestPrice = price;
			TotalVolume = mars[i].getValue<TShZdVolumeType>("TotalVolume");
		}
		//cout << OpenPrice << " " << HighestPrice << " " << " " << LowestPrice << " " << ClosePrice << " " << TotalVolume << endl;
		JSON::Object minKline_node;
		minKline_node.set("KlineTime", (now-60)*1000.0);
		minKline_node.set("InstrumentID", InstrumentID);
		minKline_node.set("OpenPrice", OpenPrice);
		minKline_node.set("HighestPrice", HighestPrice);
		minKline_node.set("LowestPrice", LowestPrice);
		minKline_node.set("ClosePrice", ClosePrice);
		minKline_node.set("PriceChange", ClosePrice-lastClosePrice);
		minKline_node.set("PriceChangeRatio", (ClosePrice-lastClosePrice)/lastClosePrice);
		minKline_node.set("TradingVolume", TotalVolume-lastTotalVolume);
		minKline_node.set("LastClosePrice", lastClosePrice);
		minKline_nodes.push_back(minKline_node);
		
		//save the last kline data
		Poco::SharedPtr<Poco::MongoDB::UpdateRequest> request = db->createUpdateRequest(lastKlineCollectionName);
		request->flags(request->UPDATE_UPSERT);
		request->selector().add("InstrumentID", InstrumentID);
		request->update().add("ClosePrice", ClosePrice);
		request->update().add("TotalVolume", TotalVolume);
		connect->sendRequest(*request);
		Poco::MongoDB::Document::Ptr lastError = db->getLastErrorDoc(*connect);
		std::cout << "LastError: " << lastError->toString(2) << std::endl;
	}

	//insert into minKlineCollectionName
	insertKline(minKlineCollectionName, minKline_nodes);

	tm *ltm = localtime(&now);
	//check if should update Hour Kline
	if (ltm->tm_min == 0)
		updateHourKline(now);
	//check if should update Day Kline
	if ((ltm->tm_hour == 0) && (ltm->tm_min == 0))
		updateDayKline(now);
	//check if should update Month Kline
	if ((ltm->tm_mday == 1) && (ltm->tm_hour == 0) && (ltm->tm_min == 0))
		updateMonKline(now);

}

void SqlHandle::updateHourKline(time_t now) {
	cout << "update Hour Kline" << endl;
	//query the min Kline in the past 1 hour
	//time_t now = 1510145842;
	vector<JSON::Object> minKline_nodes = query_latest_1hour_minkline(now);
	cout << minKline_nodes.size() << endl;
	
	//split based on InstrumentID
	map<string, vector<JSON::Object>> instMap;
	for (int n=0; n < minKline_nodes.size(); n ++) {
		string InstrumentID = minKline_nodes[n].getValue<string>("InstrumentID");
		map<string, vector<JSON::Object>>::iterator iter = instMap.find(InstrumentID);
		if (iter != instMap.end()) {
			iter->second.push_back(minKline_nodes[n]);
		} else {
			vector<JSON::Object> vec;
			vec.push_back(minKline_nodes[n]);
			instMap.insert(map<string, vector<JSON::Object>>:: value_type(InstrumentID, vec));
		}
	}

	//calc hour Kline of each instrument
	vector<JSON::Object> hourKline_nodes;
	map<string, vector<JSON::Object>>::iterator iter = instMap.begin();
	map<string, vector<JSON::Object>>::iterator end = instMap.end();
	for (; iter != end; iter ++) {
		string InstrumentID = iter->first;
		//cout << InstrumentID << endl;
		TShZdPriceType OpenPrice = -1.0;
		TShZdPriceType ClosePrice = -1.0;
		TShZdPriceType HighestPrice = -1.0;
		TShZdPriceType LastClosePrice = -1.0;
		TShZdPriceType LowestPrice = numeric_limits<double>::max();
		/*TShZdVolumeType TotalVolume = 0;*/
		TShZdVolumeType TradingVolume = 0;
		vector<JSON::Object> nodes = iter->second;
		for (int i=0; i < nodes.size(); i++) {
			TShZdPriceType oPrice = nodes[i].getValue<TShZdPriceType>("OpenPrice");
			TShZdPriceType cPrice = nodes[i].getValue<TShZdPriceType>("ClosePrice");
			TShZdPriceType hPrice = nodes[i].getValue<TShZdPriceType>("HighestPrice");
			TShZdPriceType lPrice = nodes[i].getValue<TShZdPriceType>("LowestPrice");
			//cout << price << endl;
			if (OpenPrice < 0)
				OpenPrice = oPrice;
			if (LastClosePrice < 0)
				LastClosePrice = nodes[i].getValue<TShZdPriceType>("LastClosePrice");
			ClosePrice = cPrice;
			if (hPrice > HighestPrice)
				HighestPrice = hPrice;
			if (lPrice < LowestPrice)
				LowestPrice = lPrice;
			/*TotalVolume = nodes[i].getValue<TShZdVolumeType>("TotalVolume");*/
			TradingVolume += nodes[i].getValue<TShZdVolumeType>("TradingVolume");
		}
		//cout << OpenPrice << " " << HighestPrice << " " << " " << LowestPrice << " " << ClosePrice << " " << TotalVolume << endl;
		JSON::Object hourKline_node;
		hourKline_node.set("KlineTime", (now-3600)*1000.0);
		hourKline_node.set("InstrumentID", InstrumentID);
		hourKline_node.set("OpenPrice", OpenPrice);
		hourKline_node.set("HighestPrice", HighestPrice);
		hourKline_node.set("LowestPrice", LowestPrice);
		hourKline_node.set("ClosePrice", ClosePrice);
		hourKline_node.set("PriceChange", ClosePrice-LastClosePrice);
		hourKline_node.set("PriceChangeRatio", (ClosePrice-LastClosePrice)/LastClosePrice);
		hourKline_node.set("TradingVolume", TradingVolume);
		hourKline_node.set("LastClosePrice", LastClosePrice);
		hourKline_nodes.push_back(hourKline_node);
	}

	//insert into hourKlineCollectionName
	insertKline(hourKlineCollectionName, hourKline_nodes);
}

void SqlHandle::updateDayKline(time_t now) {
	cout << "update Day Kline" << endl;
	//query the hour Kline in the past 1 day
	//time_t now = 1510142243;
	vector<JSON::Object> hourKline_nodes = query_latest_1day_hourkline(now);
	cout << hourKline_nodes.size() << endl;
	
	//split based on InstrumentID
	map<string, vector<JSON::Object>> instMap;
	for (int n=0; n < hourKline_nodes.size(); n ++) {
		string InstrumentID = hourKline_nodes[n].getValue<string>("InstrumentID");
		map<string, vector<JSON::Object>>::iterator iter = instMap.find(InstrumentID);
		if (iter != instMap.end()) {
			iter->second.push_back(hourKline_nodes[n]);
		} else {
			vector<JSON::Object> vec;
			vec.push_back(hourKline_nodes[n]);
			instMap.insert(map<string, vector<JSON::Object>>:: value_type(InstrumentID, vec));
		}
	}

	//calc day Kline of each instrument
	vector<JSON::Object> dayKline_nodes;
	map<string, vector<JSON::Object>>::iterator iter = instMap.begin();
	map<string, vector<JSON::Object>>::iterator end = instMap.end();
	for (; iter != end; iter ++) {
		string InstrumentID = iter->first;
		//cout << InstrumentID << endl;
		TShZdPriceType OpenPrice = -1.0;
		TShZdPriceType ClosePrice = -1.0;
		TShZdPriceType HighestPrice = -1.0;
		TShZdPriceType LastClosePrice = -1.0;
		TShZdPriceType LowestPrice = numeric_limits<double>::max();
		TShZdVolumeType TradingVolume = 0;
		vector<JSON::Object> nodes = iter->second;
		for (int i=0; i < nodes.size(); i++) {
			TShZdPriceType oPrice = nodes[i].getValue<TShZdPriceType>("OpenPrice");
			TShZdPriceType cPrice = nodes[i].getValue<TShZdPriceType>("ClosePrice");
			TShZdPriceType hPrice = nodes[i].getValue<TShZdPriceType>("HighestPrice");
			TShZdPriceType lPrice = nodes[i].getValue<TShZdPriceType>("LowestPrice");
			//cout << price << endl;
			if (OpenPrice < 0)
				OpenPrice = oPrice;
			if (LastClosePrice < 0)
				LastClosePrice = nodes[i].getValue<TShZdPriceType>("LastClosePrice");
			ClosePrice = cPrice;
			if (hPrice > HighestPrice)
				HighestPrice = hPrice;
			if (lPrice < LowestPrice)
				LowestPrice = lPrice;
			TradingVolume += nodes[i].getValue<TShZdVolumeType>("TradingVolume");
		}
		//cout << OpenPrice << " " << HighestPrice << " " << " " << LowestPrice << " " << ClosePrice << " " << TotalVolume << endl;
		JSON::Object dayKline_node;
		dayKline_node.set("KlineTime", (now-24*3600)*1000.0);
		dayKline_node.set("InstrumentID", InstrumentID);
		dayKline_node.set("OpenPrice", OpenPrice);
		dayKline_node.set("HighestPrice", HighestPrice);
		dayKline_node.set("LowestPrice", LowestPrice);
		dayKline_node.set("ClosePrice", ClosePrice);
		dayKline_node.set("PriceChange", ClosePrice-LastClosePrice);
		dayKline_node.set("PriceChangeRatio", (ClosePrice-LastClosePrice)/LastClosePrice);
		dayKline_node.set("LastClosePrice", LastClosePrice);
		dayKline_node.set("TradingVolume", TradingVolume);
		dayKline_nodes.push_back(dayKline_node);
	}

	//insert into dayKlineCollectionName
	insertKline(dayKlineCollectionName, dayKline_nodes);
}

void SqlHandle::updateMonKline(time_t now) {
	cout << "update Mon Kline" << endl;
	//query the hour Kline in the past 1 day
	//time_t now = 1510142243;
	tm *ltm = localtime(&now);
	int days = 31;
	if (ltm->tm_mon+1 == 1)
		days = mon2day(1900+ltm->tm_year-1, 12);
	else
		days = mon2day(ltm->tm_year, ltm->tm_mon);
	vector<JSON::Object> dayKline_nodes = query_latest_1mon_daykline(now);
	cout << dayKline_nodes.size() << endl;
	
	//split based on InstrumentID
	map<string, vector<JSON::Object>> instMap;
	for (int n=0; n < dayKline_nodes.size(); n ++) {
		string InstrumentID = dayKline_nodes[n].getValue<string>("InstrumentID");
		map<string, vector<JSON::Object>>::iterator iter = instMap.find(InstrumentID);
		if (iter != instMap.end()) {
			iter->second.push_back(dayKline_nodes[n]);
		} else {
			vector<JSON::Object> vec;
			vec.push_back(dayKline_nodes[n]);
			instMap.insert(map<string, vector<JSON::Object>>:: value_type(InstrumentID, vec));
		}
	}

	//calc day Kline of each instrument
	vector<JSON::Object> monKline_nodes;
	map<string, vector<JSON::Object>>::iterator iter = instMap.begin();
	map<string, vector<JSON::Object>>::iterator end = instMap.end();
	for (; iter != end; iter ++) {
		string InstrumentID = iter->first;
		//cout << InstrumentID << endl;
		TShZdPriceType OpenPrice = -1.0;
		TShZdPriceType ClosePrice = -1.0;
		TShZdPriceType HighestPrice = -1.0;
		TShZdPriceType LastClosePrice = -1.0;
		TShZdPriceType LowestPrice = numeric_limits<double>::max();
		TShZdVolumeType TradingVolume = 0;
		vector<JSON::Object> nodes = iter->second;
		for (int i=0; i < nodes.size(); i++) {
			TShZdPriceType oPrice = nodes[i].getValue<TShZdPriceType>("OpenPrice");
			TShZdPriceType cPrice = nodes[i].getValue<TShZdPriceType>("ClosePrice");
			TShZdPriceType hPrice = nodes[i].getValue<TShZdPriceType>("HighestPrice");
			TShZdPriceType lPrice = nodes[i].getValue<TShZdPriceType>("LowestPrice");
			//cout << price << endl;
			if (OpenPrice < 0)
				OpenPrice = oPrice;
			if (LastClosePrice < 0)
				LastClosePrice = nodes[i].getValue<TShZdPriceType>("LastClosePrice");
			ClosePrice = cPrice;
			if (hPrice > HighestPrice)
				HighestPrice = hPrice;
			if (lPrice < LowestPrice)
				LowestPrice = lPrice;
			TradingVolume += nodes[i].getValue<TShZdVolumeType>("TradingVolume");
		}
		//cout << OpenPrice << " " << HighestPrice << " " << " " << LowestPrice << " " << ClosePrice << " " << TotalVolume << endl;
		JSON::Object monKline_node;
		monKline_node.set("KlineTime", (now-24*3600*days)*1000.0);
		monKline_node.set("InstrumentID", InstrumentID);
		monKline_node.set("OpenPrice", OpenPrice);
		monKline_node.set("HighestPrice", HighestPrice);
		monKline_node.set("LowestPrice", LowestPrice);
		monKline_node.set("ClosePrice", ClosePrice);
		monKline_node.set("PriceChange", ClosePrice-LastClosePrice);
		monKline_node.set("PriceChangeRatio", (ClosePrice-LastClosePrice)/LastClosePrice);
		monKline_node.set("LastClosePrice", LastClosePrice);
		monKline_node.set("TradingVolume", TradingVolume);
		monKline_nodes.push_back(monKline_node);
	}

	//insert into monKlineCollectionName
	insertKline(monKlineCollectionName, monKline_nodes);
}

vector<JSON::Object> SqlHandle::query_latest_1min_market(time_t now)
{
	Cursor cursor(dbName, marketCollectionName);
	cursor.query().returnFieldSelector().add("InstrumentID", 1);
	cursor.query().returnFieldSelector().add("LastPrice", 1);
	cursor.query().returnFieldSelector().add("OpenPrice", 1);
	cursor.query().returnFieldSelector().add("ClosePrice", 1);
	cursor.query().returnFieldSelector().add("TotalVolume", 1);
	// get the data of last 1min
	time_t beg_t_t = now-60;
	time_t end_t_t = now;
	tm *ltm = localtime(&beg_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << " - " ;
	ltm = localtime(&end_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << endl;
	double beg_time = beg_t_t * 1000.0;
	double end_time = end_t_t * 1000.0;
	cursor.query().selector().addNewDocument("UpdateMillisec")
		.add("$gte", beg_time)
		.add("$lt", end_time);

	ResponseMessage& response = cursor.next(*connect);
	vector<JSON::Object> mars;
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; it++) {
			JSON::Object mar;
			mar.set("InstrumentID", (*it)->get<string>("InstrumentID"));
			mar.set("LastPrice", (*it)->get<TShZdPriceType>("LastPrice"));
			//mar.set("OpenPrice", (*it)->get<TShZdPriceType>("OpenPrice"));
			//mar.set("ClosePrice", (*it)->get<TShZdPriceType>("ClosePrice"));
			mar.set("TotalVolume", (*it)->get<TShZdVolumeType>("TotalVolume"));
			mars.push_back(mar);
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};
	return mars;
}

vector<JSON::Object> SqlHandle::query_latest_1hour_minkline(time_t now) {
	Cursor cursor(dbName, minKlineCollectionName);
	cursor.query().returnFieldSelector().add("InstrumentID", 1);
	cursor.query().returnFieldSelector().add("HighestPrice", 1);
	cursor.query().returnFieldSelector().add("LowestPrice", 1);
	cursor.query().returnFieldSelector().add("OpenPrice", 1);
	cursor.query().returnFieldSelector().add("ClosePrice", 1);
	cursor.query().returnFieldSelector().add("LastClosePrice", 1);
	cursor.query().returnFieldSelector().add("TradingVolume", 1);
	// get the data of last 1 hour
	time_t beg_t_t = now-3600;
	time_t end_t_t = now;
	//cout << beg_t_t << " " << end_t_t << endl;
	tm *ltm = localtime(&beg_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << " - " ;
	ltm = localtime(&end_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << endl;
	double beg_time = beg_t_t * 1000.0;
	double end_time = end_t_t * 1000.0;
	cursor.query().selector().addNewDocument("KlineTime")
		.add("$gte", beg_time)
		.add("$lt", end_time);

	ResponseMessage& response = cursor.next(*connect);
	vector<JSON::Object> Kline_nodes;
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; it++) {
			JSON::Object node;
			node.set("InstrumentID", (*it)->get<string>("InstrumentID"));
			node.set("HighestPrice", (*it)->get<TShZdPriceType>("HighestPrice"));
			node.set("LowestPrice", (*it)->get<TShZdPriceType>("LowestPrice"));
			node.set("OpenPrice", (*it)->get<TShZdPriceType>("OpenPrice"));
			node.set("ClosePrice", (*it)->get<TShZdPriceType>("ClosePrice"));
			node.set("LastClosePrice", (*it)->get<TShZdPriceType>("LastClosePrice"));
			node.set("TradingVolume", (*it)->get<TShZdVolumeType>("TradingVolume"));
			Kline_nodes.push_back(node);
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};
	return Kline_nodes;
}

vector<JSON::Object> SqlHandle::query_latest_1day_hourkline(time_t now) {
	Cursor cursor(dbName, hourKlineCollectionName);
	cursor.query().returnFieldSelector().add("InstrumentID", 1);
	cursor.query().returnFieldSelector().add("HighestPrice", 1);
	cursor.query().returnFieldSelector().add("LowestPrice", 1);
	cursor.query().returnFieldSelector().add("OpenPrice", 1);
	cursor.query().returnFieldSelector().add("ClosePrice", 1);
	cursor.query().returnFieldSelector().add("LastClosePrice", 1);
	cursor.query().returnFieldSelector().add("TradingVolume", 1);
	// get the data of last 1 day
	time_t beg_t_t = now-3600*24;
	time_t end_t_t = now;
	//cout << beg_t_t << " " << end_t_t << endl;
	tm *ltm = localtime(&beg_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << " - " ;
	ltm = localtime(&end_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << endl;
	double beg_time = beg_t_t * 1000.0;
	double end_time = end_t_t * 1000.0;
	cursor.query().selector().addNewDocument("KlineTime")
		.add("$gte", beg_time)
		.add("$lt", end_time);

	ResponseMessage& response = cursor.next(*connect);
	vector<JSON::Object> Kline_nodes;
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; it++) {
			JSON::Object node;
			node.set("InstrumentID", (*it)->get<string>("InstrumentID"));
			node.set("HighestPrice", (*it)->get<TShZdPriceType>("HighestPrice"));
			node.set("LowestPrice", (*it)->get<TShZdPriceType>("LowestPrice"));
			node.set("OpenPrice", (*it)->get<TShZdPriceType>("OpenPrice"));
			node.set("ClosePrice", (*it)->get<TShZdPriceType>("ClosePrice"));
			node.set("LastClosePrice", (*it)->get<TShZdPriceType>("LastClosePrice"));
			node.set("TradingVolume", (*it)->get<TShZdVolumeType>("TradingVolume"));
			Kline_nodes.push_back(node);
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};
	return Kline_nodes;
}

vector<JSON::Object> SqlHandle::query_latest_1mon_daykline(time_t now) {
	Cursor cursor(dbName, dayKlineCollectionName);
	cursor.query().returnFieldSelector().add("InstrumentID", 1);
	cursor.query().returnFieldSelector().add("HighestPrice", 1);
	cursor.query().returnFieldSelector().add("LowestPrice", 1);
	cursor.query().returnFieldSelector().add("OpenPrice", 1);
	cursor.query().returnFieldSelector().add("ClosePrice", 1);
	cursor.query().returnFieldSelector().add("LastClosePrice", 1);
	cursor.query().returnFieldSelector().add("TradingVolume", 1);
	// get the data of last 1 mon
	tm *ltm = localtime(&now);
	int days = 31;
	if (ltm->tm_mon+1 == 1)
		days = mon2day(1900+ltm->tm_year-1, 12);
	else
		days = mon2day(ltm->tm_year, ltm->tm_mon);
	time_t beg_t_t = now-3600*24*days;
	time_t end_t_t = now;
	//cout << beg_t_t << " " << end_t_t << endl;
	ltm = localtime(&beg_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << " - " ;
	ltm = localtime(&end_t_t);
	cout << 1900+ltm->tm_year << "-" << \
			ltm->tm_mon+1 << "-" << \
			ltm->tm_mday << " " << \
			ltm->tm_hour << ":" << \
			ltm->tm_min << ":" << \
			ltm->tm_sec << endl;
	double beg_time = beg_t_t * 1000.0;
	double end_time = end_t_t * 1000.0;
	cursor.query().selector().addNewDocument("KlineTime")
		.add("$gte", beg_time)
		.add("$lt", end_time);

	ResponseMessage& response = cursor.next(*connect);
	vector<JSON::Object> Kline_nodes;
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; it++) {
			JSON::Object node;
			node.set("InstrumentID", (*it)->get<string>("InstrumentID"));
			node.set("HighestPrice", (*it)->get<TShZdPriceType>("HighestPrice"));
			node.set("LowestPrice", (*it)->get<TShZdPriceType>("LowestPrice"));
			node.set("OpenPrice", (*it)->get<TShZdPriceType>("OpenPrice"));
			node.set("ClosePrice", (*it)->get<TShZdPriceType>("ClosePrice"));
			node.set("LastClosePrice", (*it)->get<TShZdPriceType>("LastClosePrice"));
			node.set("TradingVolume", (*it)->get<TShZdVolumeType>("TradingVolume"));
			Kline_nodes.push_back(node);
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};
	return Kline_nodes;
}

vector<JSON::Object> SqlHandle::queryKLE(string type, string InstrumentID, time_t begTime, time_t endTime) 
{
	vector<JSON::Object> Kline_nodes;
	string collection;
	int intval = 60000;
	int intvalnum = 1;
	if (type=="ONE" || type=="THR" || type=="FIV" || type=="TEN" || type=="HAF"){
		collection = minKlineCollectionName;
		//set the time to be the minute
		begTime -= (begTime % 60);
		endTime -= (endTime % 60);
		intval = 60000;
		if (type == "ONE") {
			intvalnum = 1;
		}
		else if (type == "THR") {
			intvalnum = 3;
		}
		else if (type == "FIV") {
			intvalnum = 5;
		}
		else if (type == "TEN") {
			intvalnum = 10;
		}
		else if (type == "HAF") {
			intvalnum = 30;
		}
	}
	else if (type=="SIT" || type=="FOH") {
		collection = hourKlineCollectionName;
		//set the time to be the hour
		begTime -= (begTime % 3600);
		endTime -= (endTime % 3600);
		intval = 60*60000;
		if (type == "SIT") {
			intvalnum = 1;
		}
		else if (type == "FOH") {
			intvalnum = 4;
		}
	}
	else if (type=="DAY" || type=="WEK") {
		collection = dayKlineCollectionName;
		//set the time to be the day
		begTime -= (begTime % 24*3600);
		endTime -= (endTime % 24*3600);
		intval = 24*60*60000;
		if (type == "DAY") {
			intvalnum = 1;
		}
		else if (type == "WEK") {
			intvalnum = 7;
		}
	}
	else if (type == "MON") {
		collection = monKlineCollectionName;
		intval = 30*24*60*60000;
		//set the time to be the month
		tm* ltm = localtime(&begTime);
		begTime -= 24*3600*(ltm->tm_mday-1)+3600*ltm->tm_hour+60*ltm->tm_min+ltm->tm_sec;
		endTime -= 24*3600*(ltm->tm_mday-1)+3600*ltm->tm_hour+60*ltm->tm_min+ltm->tm_sec;
		intvalnum = 1;
	}
	else {
		cout << "type error" << endl;
		return Kline_nodes;
	}
	Cursor cursor(dbName, collection);
	cursor.query().returnFieldSelector().add("InstrumentID", 1);
	cursor.query().returnFieldSelector().add("KlineTime", 1);
	cursor.query().returnFieldSelector().add("OpenPrice", 1);
	cursor.query().returnFieldSelector().add("ClosePrice", 1);
	cursor.query().returnFieldSelector().add("HighestPrice", 1);
	cursor.query().returnFieldSelector().add("LowestPrice", 1);
	cursor.query().returnFieldSelector().add("PriceChange", 1);
	cursor.query().returnFieldSelector().add("PriceChangeRatio", 1);
	cursor.query().returnFieldSelector().add("TradingVolume", 1);
	
	double beg_time = begTime * 1000.0;
	double end_time = endTime * 1000.0;
	cursor.query().selector().add("InstrumentID", InstrumentID);
	cursor.query().selector().addNewDocument("KlineTime")
		.add("$gte", beg_time)
		.add("$lt", end_time);

	ResponseMessage& response = cursor.next(*connect);
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; it++) {
			JSON::Object Kline_node;
			Kline_node.set("InstrumentID", (*it)->get<string>("InstrumentID"));
			Kline_node.set("OpenPrice", (*it)->get<TShZdPriceType>("OpenPrice"));
			Kline_node.set("ClosePrice", (*it)->get<TShZdPriceType>("ClosePrice"));
			Kline_node.set("HighestPrice", (*it)->get<TShZdPriceType>("HighestPrice"));
			Kline_node.set("LowestPrice", (*it)->get<TShZdPriceType>("LowestPrice"));
			Kline_node.set("PriceChange", (*it)->get<TShZdPriceType>("PriceChange"));
			Kline_node.set("PriceChangeRatio", (*it)->get<double>("PriceChangeRatio"));
			Kline_node.set("TradingVolume", (*it)->get<TShZdVolumeType>("TradingVolume"));
			Kline_nodes.push_back(Kline_node);
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};

	//fill up the blank nodes, only for minKline, hourKline and dayKline
	vector<JSON::Object> filledKline_nodes;
	if (type=="ONE" || type=="THR" || type=="FIV" || type=="TEN" || type=="HAF" ||
		type=="SIT" || type=="FOH" || type=="DAY") {
		JSON::Object fillNode;
		if (Kline_nodes.size() > 0) {
			TShZdPriceType LastClosePrice = Kline_nodes[0].getValue<TShZdPriceType>("LastClosePrice");
			fillNode.set("OpenPrice", LastClosePrice);
			fillNode.set("ClosePrice", LastClosePrice);
			fillNode.set("HighestPrice", LastClosePrice);
			fillNode.set("LowestPrice", LastClosePrice);
			fillNode.set("PriceChange", 0);
			fillNode.set("PriceChangeRatio", 0.0);
			fillNode.set("TradingVolume", 0);
		}
		long long lastTime = beg_time;
		for (int i=0; i < Kline_nodes.size(); i ++) {
			long long KlineTime = Kline_nodes[i].getValue<long long>("KlineTime");
			for (int j=0; j < (KlineTime-lastTime)/intval; j ++) {
				fillNode.set("KlineTime", lastTime + intval*j);
				filledKline_nodes.push_back(fillNode);
			}
			filledKline_nodes.push_back(Kline_nodes[i]);
			lastTime = KlineTime + intval;

			TShZdPriceType ClosePrice = Kline_nodes[i].getValue<TShZdPriceType>("ClosePrice");
			fillNode.set("OpenPrice", ClosePrice);
			fillNode.set("ClosePrice", ClosePrice);
			fillNode.set("HighestPrice", ClosePrice);
			fillNode.set("LowestPrice", ClosePrice);
		}
		for (int j=0; j < ((long long)end_time-lastTime)/intval; j ++) {
			fillNode.set("KlineTime", lastTime + intval*j);
			filledKline_nodes.push_back(fillNode);
		}
	}
	
	vector<JSON::Object> resultKline_nodes;
	for (int i=0; i < filledKline_nodes.size(); i += intvalnum) {
		TShZdPriceType OpenPrice = -1.0;
		TShZdPriceType ClosePrice = -1.0;
		TShZdPriceType HighestPrice = -1.0;
		TShZdPriceType LastClosePrice = -1.0;
		TShZdPriceType LowestPrice = numeric_limits<double>::max();
		TShZdVolumeType TradingVolume = 0;
		time_t t = -1;
		for (int j=i; j < intvalnum; j ++) {
			TShZdPriceType oPrice = filledKline_nodes[j].getValue<TShZdPriceType>("OpenPrice");
			TShZdPriceType cPrice = filledKline_nodes[j].getValue<TShZdPriceType>("ClosePrice");
			TShZdPriceType hPrice = filledKline_nodes[j].getValue<TShZdPriceType>("HighestPrice");
			TShZdPriceType lPrice = filledKline_nodes[j].getValue<TShZdPriceType>("LowestPrice");
			if (t < 0)
				t = filledKline_nodes[j].getValue<time_t>("KlineTime");
			if (OpenPrice < 0)
				OpenPrice = oPrice;
			if (LastClosePrice < 0)
				LastClosePrice = filledKline_nodes[j].getValue<TShZdPriceType>("LastClosePrice");
			ClosePrice = cPrice;
			if (hPrice > HighestPrice)
				HighestPrice = hPrice;
			if (lPrice < LowestPrice)
				LowestPrice = lPrice;
			TradingVolume += filledKline_nodes[j].getValue<TShZdVolumeType>("TradingVolume");
		}
		JSON::Object resultKline_node;
		resultKline_node.set("KlineTime", t);
		resultKline_node.set("InstrumentID", InstrumentID);
		resultKline_node.set("OpenPrice", OpenPrice);
		resultKline_node.set("HighestPrice", HighestPrice);
		resultKline_node.set("LowestPrice", LowestPrice);
		resultKline_node.set("ClosePrice", ClosePrice);
		resultKline_node.set("PriceChange", ClosePrice-LastClosePrice);
		resultKline_node.set("PriceChangeRatio", (ClosePrice-LastClosePrice)/LastClosePrice);
		resultKline_node.set("TradingVolume", TradingVolume);
		resultKline_node.set("LastClosePrice", LastClosePrice);
		resultKline_nodes.push_back(resultKline_node);
	}
	
	return resultKline_nodes;
}

TShZdPriceType SqlHandle::quertLastMinPrice(string InstrumentID) {
	Poco::MongoDB::Cursor cursor(dbName, minKlineCollectionName);
	cursor.query().returnFieldSelector().add("ClosePrice", 1);
	cursor.query().selector().add("InstrumentID", InstrumentID);
	Poco::MongoDB::ResponseMessage& response = cursor.next(*connect);
	Poco::MongoDB::Document::Vector::const_iterator it = response.documents().end() - 1;
	for (;;)
	{
		it = response.documents().end() - 1;
		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};
	return (*it)->get<TShZdPriceType>("ClosePrice");
}

int SqlHandle::insertKline(string collectionName, vector<JSON::Object> minKline_nodes) {
	cout << "*** INSERT Kline ***" << endl;
	if (minKline_nodes.size() == 0)
		return 0;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(collectionName);
	for (int i=0; i < minKline_nodes.size(); i ++) {
		JSON::Object node = minKline_nodes[i];
		insertRequest->addNewDocument()
			.add("InstrumentID", node.getValue<string>("InstrumentID"))
			.add("KlineTime", node.getValue<time_t>("KlineTime"))
			.add("OpenPrice", node.getValue<TShZdPriceType>("OpenPrice"))
			.add("HighestPrice", node.getValue<TShZdPriceType>("HighestPrice"))
			.add("LowestPrice", node.getValue<TShZdPriceType>("LowestPrice"))
			.add("ClosePrice", node.getValue<TShZdPriceType>("ClosePrice"))
			.add("TotalVolume", node.getValue<TShZdVolumeType>("TotalVolume"));
	}
	
	connect->sendRequest(*insertRequest);
	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Last Error: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
}

int SqlHandle::insertExanges(CTShZdExchangeField* field) {
	std::cout << "*** INSERT EXANGES ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(exangeCollectionName);
	Document& doc = insertRequest->addNewDocument();
	///交易所代码  直达
	doc.add<string>("ExchangeID", field->ExchangeID);
	///交易所名称  直达
	doc.add<string>("ExchangeName", field->ExchangeName);
	///交易所属性
	doc.add<int>("ExchangeProperty", field->ExchangeProperty);

	connect->sendRequest(*insertRequest);
	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Last Error: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
}

int SqlHandle::insertInstruments(CTShZdInstrumentField* field) {
	//std::cout << "*** INSERT INSTRUMENTS ***" << std::endl;
	
	/*
	 * NOTE: dereplication: 
	 *
	 * "InstrumentID":"CUS_C1711 7.50"
	 * "InstrumentName":"NAME 7.50"
	 * "ExchangeInstID":"1711 7.50"
	 * "OptionPrice":750.0
	 *
	 *  => OptionPrices : [price1, price2, ......]
	 */

	/* update/insert Instrument collection */
	SharedPtr<Poco::MongoDB::UpdateRequest> updateRequest = 
		db->createUpdateRequest(instrumentCollectionName);
	updateRequest->flags(updateRequest->UPDATE_UPSERT);
	Document& doc = updateRequest->selector();

	///合约代码  直达
	Poco::StringTokenizer InstrumentIDToken(field->InstrumentID, " ", Poco::StringTokenizer::TOK_TRIM);
	if (InstrumentIDToken.count() > 0)
		doc.add<string>("InstrumentID", *(InstrumentIDToken.begin()));
	///交易所代码  直达
	doc.add<string>("ExchangeID", field->ExchangeID);
	///合约名称  直达
	Poco::StringTokenizer InstrumentNameToken(field->InstrumentName, " ", Poco::StringTokenizer::TOK_TRIM);
	if (InstrumentNameToken.count() > 0)
		doc.add<string>("InstrumentName", *(InstrumentNameToken.begin()));
	///合约在交易所的代码  直达
	Poco::StringTokenizer ExchangeInstIDToken(field->ExchangeInstID, " ", Poco::StringTokenizer::TOK_TRIM);
	if (ExchangeInstIDToken.count() > 0)
		doc.add<string>("ExchangeInstID", *(ExchangeInstIDToken.begin()));
	
	///交易所名称  直达
	doc.add<string>("ExchangeName", field->ExchangeName);
	///产品代码  直达
	doc.add<string>("ProductID", field->ProductID);
	///产品名称  直达
	doc.add<string>("ProductName", field->ProductName);
	///产品类型  F期货 O期权  直达
	doc.add<int>("ProductClass", field->ProductClass);
	///合约货币代码  直达
	doc.add<string>("CurrencyNo", field->CurrencyNo);
	///货币名称  直达
	doc.add<string>("CurrencyName", field->CurrencyName);	
	///行情小数为数 直达
	doc.add<int>("MarketDot", field->MarketDot);
	///行情进阶单位 10进制 32进制  64进制等 直达
	doc.add<int>("MarketUnit", field->MarketUnit);
	///调期小时点位数  直达
	doc.add<double>("ChangeMarketDot", field->ChangeMarketDot);
	///合约数量乘数  点值（一个最小跳点的价值） 直达
	doc.add<double>("VolumeMultiple", field->VolumeMultiple);
	///调期最小变动单位  直达
	doc.add<double>("ChangeMultiple", field->ChangeMultiple);
	///最小变动价位  直达
	doc.add<double>("PriceTick", field->PriceTick);	
	///交割月日  直达
	doc.add<string>("StartDelivDate", field->StartDelivDate);
	///最后更新日  直达
	doc.add<string>("LastUpdateDate", field->LastUpdateDate);
	///首次通知日 直达
	doc.add<string>("ExpireDate", field->ExpireDate);
	///最后交易日  直达
	doc.add<string>("EndTradeDate", field->EndTradeDate);	
	///当前是否交易
	doc.add<int>("IsTrading", field->IsTrading);
	///期权类型
	doc.add<int>("OptionType", field->OptionType);
	///期权年月  直达
	doc.add<string>("OptionDate", field->OptionDate);
	///保证金率  直达
	doc.add<double>("MarginRatio", field->MarginRatio);
	///固定保证金  直达
	doc.add<double>("MarginValue", field->MarginValue);
	///手续费率  直达
	doc.add<double>("FreeRatio", field->FreeRatio);
	///固定手续费  直达
	doc.add<double>("FreeValue", field->FreeValue);
	///现货商品昨结算价  直达
	doc.add<double>("SpotYesSetPrice", field->SpotYesSetPrice);
	///现货商品点值  直达
	doc.add<double>("SpotMultiple", field->SpotMultiple);
	///现货商品最小变动单位  直达
	doc.add<double>("SpotTick", field->SpotTick);
	///期权临界价格  直达
	doc.add<double>("OptionTickPrice", field->OptionTickPrice);
	///期权临界价格以下最小跳点  直达
	doc.add<double>("OptionTick", field->OptionTick);
	///期权执行价  直达
	//doc.add<double>("OptionPrice", field->OptionPrice);
	///期权对应期货的商品代码 直达
	doc.add<string>("OptionCommodityNo", field->OptionCommodityNo);
	///期权对应期货的合约代码 直达
	doc.add<string>("OptionContractNo", field->OptionContractNo);

	updateRequest->update().addNewDocument("$push").add("OptionPrices", field->OptionPrice);
	connect->sendRequest(*updateRequest);

	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Error in inserting Instruments: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
}

int SqlHandle::insertFilledData(CTShZdFilledDataField* field) {
	std::cout << "*** INSERT FILLED-DATA ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(filledDataCollectionName);
	Document& doc = insertRequest->addNewDocument();
	///交易日  直达
	doc.add<string>("TradingDay", field->TradingDay);
	///合约代码  直达
	doc.add<string>("InstrumentID", field->InstrumentID);
	///交易所代码   直达
	doc.add<string>("ExchangeID", field->ExchangeID);
	///合约在交易所的代码  
	doc.add<string>("ExchangeInstID", field->ExchangeInstID);
	///成交价  直达
	doc.add<TShZdPriceType>("LastPrice", field->LastPrice);
	///成交数量  直达
	doc.add<TShZdVolumeType>("Volume", field->Volume);
	///成交总数量  直达
	doc.add<TShZdVolumeType>("FilledVolume", field->FilledVolume);
	///最后修改时间  直达
	doc.add<string>("UpdateTime", field->UpdateTime);
	///最后修改毫秒  直达
	doc.add<TShZdMillisecType>("UpdateMillisec", field->UpdateMillisec);
	
	connect->sendRequest(*insertRequest);
	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Last Error while inserting FilledData: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
}

int SqlHandle::insertDeptMarketData(CTShZdDepthMarketDataField* field) {
	std::cout << "*** INSERT DEPT-MATKETS ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(marketCollectionName);
	Document& doc = insertRequest->addNewDocument();
	///交易日  直达
	doc.add<string>("TradingDay", field->TradingDay);
	///合约代码  直达
	doc.add<string>("InstrumentID", field->InstrumentID);
	///交易所代码   直达
	doc.add<string>("ExchangeID", field->ExchangeID);
	///合约在交易所的代码  
	doc.add<string>("ExchangeInstID", field->ExchangeInstID);
	///最新价  直达
	doc.add<double>("LastPrice", field->LastPrice);
	///上次结算价  直达
	doc.add<double>("PreSettlementPrice", field->PreSettlementPrice);
	///昨收盘  直达
	doc.add<double>("PreClosePrice", field->PreClosePrice);
	///昨持仓量 直达
	doc.add<double>("PreOpenInterest", field->PreOpenInterest);
	///今开盘  直达
	doc.add<double>("OpenPrice", field->OpenPrice);
	///最高价  直达
	doc.add<double>("HighestPrice", field->HighestPrice);
	///最低价  直达
	doc.add<double>("LowestPrice", field->LowestPrice);
	///数量  直达
	doc.add<double>("Volume", field->Volume);
	///成交金额
	doc.add<double>("Turnover", field->Turnover);
	///持仓量  直达
	doc.add<double>("OpenInterest", field->OpenInterest);
	///今收盘  直达
	doc.add<double>("ClosePrice", field->ClosePrice);
	///本次结算价
	doc.add<double>("SettlementPrice", field->SettlementPrice);
	///涨停板价
	doc.add<double>("UpperLimitPrice", field->UpperLimitPrice);
	///跌停板价
	doc.add<double>("LowerLimitPrice", field->LowerLimitPrice);
	///昨虚实度
	doc.add<double>("PreDelta", field->PreDelta);
	///今虚实度
	doc.add<double>("CurrDelta", field->CurrDelta);
	///最后修改时间  直达
	doc.add<string>("UpdateTime", field->UpdateTime);
	///最后修改毫秒  直达
	doc.add<double>("UpdateMillisec", field->UpdateMillisec);
	///申买价一  直达
	doc.add<double>("BidPrice1", field->BidPrice1);
	///申买量一  直达
	doc.add<double>("BidVolume1", field->BidVolume1);
	///申卖价一  直达
	doc.add<double>("AskPrice1", field->AskPrice1);
	///申卖量一  直达
	doc.add<double>("AskVolume1", field->AskVolume1);
	///申买价二  直达
	doc.add<double>("BidPrice2", field->BidPrice2);
	///申买量二  直达
	doc.add<double>("BidVolume2", field->BidVolume2);
	///申卖价二  直达
	doc.add<double>("AskPrice2", field->AskPrice2);
	///申卖量二  直达
	doc.add<double>("AskVolume2", field->AskVolume2);
	///申买价三  直达
	doc.add<double>("BidPrice3", field->BidPrice3);
	///申买量三  直达
	doc.add<double>("BidVolume3", field->BidVolume3);
	///申卖价三  直达
	doc.add<double>("AskPrice3", field->AskPrice3);
	///申卖量三  直达
	doc.add<double>("AskVolume3", field->AskVolume3);
	///申买价四  直达
	doc.add<double>("BidPrice4", field->BidPrice4);
	///申买量四  直达
	doc.add<double>("BidVolume4", field->BidVolume4);
	///申卖价四  直达
	doc.add<double>("AskPrice4", field->AskPrice4);
	///申卖量四  直达
	doc.add<double>("AskVolume4", field->AskVolume4);
	///申买价五  直达
	doc.add<double>("BidPrice5", field->BidPrice5);
	///申买量五  直达
	doc.add<double>("BidVolume5", field->BidVolume5);
	///申卖价五  直达
	doc.add<double>("AskPrice5", field->AskPrice5);
	///申卖量五  直达
	doc.add<double>("AskVolume5", field->AskVolume5);
	///当日均价  直达
	doc.add<double>("AveragePrice", field->AveragePrice);
	///成交总数量  直达
	doc.add<int>("TotalVolume", field->TotalVolume);
	connect->sendRequest(*insertRequest);
	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Last Error while inserting DeptMarketData: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
}

int SqlHandle::insertKline(CTShZdDepthMarketDataField* field)
{
	return 1;
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

int SqlHandle::queryInstruments() {
	std::cout << "*** QUERY INSTRUMENTS ***" << std::endl;
	const char* keys[] = {"ExchangeID", "InstrumentID", \
		"ExchangeInstID", "ProductName", "ProductID"};
	return query(instrumentCollectionName, keys, 5);
}

int SqlHandle::query(const char* collection, const char** keys, int num)
{
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
		//待确定
		for (; it < end-1; ++it)
		{
			for (int i = 0; i < num; i++) {
				std::cout << "--->" << (*it)->get<std::string>(keys[i]) << " ";
			}
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

//query the exchageID
vector<string> SqlHandle::queryExchanges()
{
	std::cout << "*** QUERY ExchangesID ***" << std::endl;
	vector<string> result;
	string sel_tmp;

	char* key = "ExchangeID";
	Cursor cursor(dbName, exangeCollectionName);
	cursor.query().returnFieldSelector().add(key, 1);

	ResponseMessage& response = cursor.next(*connect);
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		//待确定
		for (; it != end; ++it)
		{
			sel_tmp = (*it)->get<std::string>(key);
			if (sel_tmp.length()>0) {
				std::cout << "--->" << sel_tmp << endl;
				result.push_back(sel_tmp);
			}
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}
		// Get the next bunch of documents
		response = cursor.next(*connect);
	}
	return result;
}

//query the productID
vector<string> SqlHandle::queryProduct(const char* exchangeID)
{
	//std::cout << "*** QUERY productID ***" << std::endl;
	vector<string> result;
	string sel_tmp;
	string old_tmp = "";
	char* key = "ProductID";
	Cursor cursor(dbName, instrumentCollectionName);
	cursor.query().returnFieldSelector().add(key, 1);
	cursor.query().selector().add("ExchangeID", exchangeID);
	//cursor.query().setNumberToReturn(1); //get only one product ID

	ResponseMessage& response = cursor.next(*connect);
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		//	//待确定
		for (; it!=end; ++it)
		{
			sel_tmp = (*it)->get<std::string>(key);
			if (sel_tmp.length()>0) {
				if (sel_tmp.compare(old_tmp) == 0) {
					continue;
				}
				else {
					result.push_back(sel_tmp);
					old_tmp = sel_tmp;
				}
			}
		}

		//	// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}
		// Get the next bunch of documents
		response = cursor.next(*connect);
	}
	return result;
}

//query instruments by exchangeID and productID
JSON::Array SqlHandle::queryInsts(const char *exchangeID, const char* productID,const char* end_time)
{

	JSON::Array result;
	Dynamic::Var instrut;
	string sel_tmp,prodcutName;
	char* key = "InstrumentID";
	Cursor cursor(dbName, instrumentCollectionName);
	cursor.query().returnFieldSelector().add(key, 1);
	cursor.query().returnFieldSelector().add("ProductName", 1);
	cursor.query().returnFieldSelector().add("EndTradeDate", 1);
	//condition
	cursor.query().selector().add("ExchangeID", exchangeID);
	cursor.query().selector().add("ProductID", productID);
	cursor.query().selector().addNewDocument("EndTradeDate").add("$gt", end_time);
	//cursor.query().setNumberToReturn(1); //get only one product ID

	bool flag = true;
	ResponseMessage& response = cursor.next(*connect);
	for (;;)
	{
		Document::Vector::const_iterator it = response.documents().begin();
		Document::Vector::const_iterator end = response.documents().end();
		for (; it != end; ++it)
		{
			sel_tmp = (*it)->get<std::string>(key);
			string date_tmp = (*it)->get<std::string>("EndTradeDate");
			if (flag) {
				flag = false;
				prodcutName = (*it)->get<std::string>("ProductName");
				result.add(prodcutName);
			}
			if (sel_tmp.length()>0) {
				//std::cout << exchangeID << "---"<<productID<< "--->" << sel_tmp << "------>" << date_tmp << endl;
				instrut = sel_tmp;
				result.add(instrut);
			}
		}
		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0){
			break;
		}
		// Get the next bunch of documents
		response = cursor.next(*connect);
	}
	return result;
}

//judge the exCode existed
bool SqlHandle::checkExchange(const char* exchangeID)
{
	bool flag = false;
	std::cout << " if update the exchange " << std::endl;

	Poco::MongoDB::Cursor cursor(dbName, exangeCollectionName);
	cursor.query().selector().add("ExchangeID", exchangeID);

	Poco::MongoDB::ResponseMessage& response = cursor.next(*connect);
	for (;;)
	{
		for (Poco::MongoDB::Document::Vector::const_iterator it = response.documents().begin(); it != response.documents().end(); ++it)
		{
			flag = true;
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(*connect);
	};
	return flag;
}
