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
						exangeCollectionName("exchange")
{
	connect = new Connection(ip, port);
	db = new Database(dbName);
}

SqlHandle::SqlHandle(char* dbName, char* ip, int port) : \
						ip(ip), \
						port(port), \
						dbName(dbName), \
						instrumentCollectionName("instrument"), \
						marketCollectionName("market"), \
						exangeCollectionName("exchange")
{
	connect = new Connection(ip, port);
	db = new Database(dbName);
}

SqlHandle::~SqlHandle()
{
	connect->disconnect();
	delete connect;
	delete db;
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
	doc.add<string>("InstrumentID", *(InstrumentIDToken.begin()));
	///交易所代码  直达
	doc.add<string>("ExchangeID", field->ExchangeID);
	///合约名称  直达
	Poco::StringTokenizer InstrumentNameToken(field->InstrumentName, " ", Poco::StringTokenizer::TOK_TRIM);
	doc.add<string>("InstrumentName", *(InstrumentNameToken.begin()));
	///合约在交易所的代码  直达
	Poco::StringTokenizer ExchangeInstIDToken(field->ExchangeInstID, " ", Poco::StringTokenizer::TOK_TRIM);
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

int SqlHandle::insertMarketData(CTShZdDepthMarketDataField* field) {
	std::cout << "*** INSERT MATKETS ***" << std::endl;
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
		std::cout << "Last Error: " << db->getLastError(*connect) << std::endl;
		return -1;
	}
	return 0;
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
			if (!sel_tmp.empty()) {
				//std::cout << "--->" << sel_tmp << endl;
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
			if (!sel_tmp.empty()) {
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
			if (!sel_tmp.empty()) {
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