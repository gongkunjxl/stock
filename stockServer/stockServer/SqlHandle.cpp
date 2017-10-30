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

	this->instrumentCollectionName = "instrument";
	this->marketCollectionName = "market";
}

SqlHandle::~SqlHandle()
{
	connect->disconnect();
	delete [] dbName;
	delete connect;
	delete db;
}

int SqlHandle::insertExanges(CTShZdExchangeField* field) {
	std::cout << "*** INSERT EXANGES ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(exangeCollectionName);
	Document& doc = insertRequest->addNewDocument();
	///交易所代码  直达
	doc.add("ExchangeID", field->ExchangeID);
	///交易所名称  直达
	doc.add("ExchangeName", field->ExchangeName);
	///交易所属性
	doc.add("ExchangeProperty", field->ExchangeProperty);

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
	std::cout << "*** INSERT INSTRUMENTS ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(instrumentCollectionName);
	Document& doc = insertRequest->addNewDocument();
	///合约代码  直达
	doc.add("InstrumentID", field->InstrumentID);;
	///交易所代码  直达
	doc.add("ExchangeID", field->ExchangeID);;
	///合约名称  直达
	doc.add("InstrumentName", field->InstrumentName);;
	///合约在交易所的代码  直达
	doc.add("ExchangeInstID", field->ExchangeInstID);;
	///交易所名称  直达
	doc.add("ExchangeName", field->ExchangeName);;
	///产品代码  直达
	doc.add("ProductID", field->ProductID);;
	///产品名称  直达
	doc.add("ProductName", field->ProductName);;
	///产品类型  F期货 O期权  直达
	doc.add("ProductClass", field->ProductClass);;
	///合约货币代码  直达
	doc.add("CurrencyNo", field->CurrencyNo);;
	///货币名称  直达
	doc.add("CurrencyName", field->CurrencyName);;	
	///行情小数为数 直达
	doc.add("MarketDot", field->MarketDot);;
	///行情进阶单位 10进制 32进制  64进制等 直达
	doc.add("MarketUnit", field->MarketUnit);;
	///调期小时点位数  直达
	doc.add("ChangeMarketDot", field->ChangeMarketDot);;
	///合约数量乘数  点值（一个最小跳点的价值） 直达
	doc.add("VolumeMultiple", field->VolumeMultiple);;
	///调期最小变动单位  直达
	doc.add("ChangeMultiple", field->ChangeMultiple);;
	///最小变动价位  直达
	doc.add("PriceTick", field->PriceTick);;	
	///交割月日  直达
	doc.add("StartDelivDate", field->StartDelivDate);;
	///最后更新日  直达
	doc.add("LastUpdateDate", field->LastUpdateDate);;
	///首次通知日 直达
	doc.add("ExpireDate", field->ExpireDate);;
	///最后交易日  直达
	doc.add("EndTradeDate", field->EndTradeDate);;	
	///当前是否交易
	doc.add("IsTrading", field->IsTrading);;
	///期权类型
	doc.add("OptionType", field->OptionType);;
	///期权年月  直达
	doc.add("OptionDate", field->OptionDate);;
	///保证金率  直达
	doc.add("MarginRatio", field->MarginRatio);;
	///固定保证金  直达
	doc.add("MarginValue", field->MarginValue);;
	///手续费率  直达
	doc.add("FreeRatio", field->FreeRatio);;
	///固定手续费  直达
	doc.add("FreeValue", field->FreeValue);;
	///现货商品昨结算价  直达
	doc.add("SpotYesSetPrice", field->SpotYesSetPrice);;
	///现货商品点值  直达
	doc.add("SpotMultiple", field->SpotMultiple);;
	///现货商品最小变动单位  直达
	doc.add("SpotTick", field->SpotTick);;
	///期权临界价格  直达
	doc.add("OptionTickPrice", field->OptionTickPrice);;
	///期权临界价格以下最小跳点  直达
	doc.add("OptionTick", field->OptionTick);;
	///期权执行价  直达
	doc.add("OptionPrice", field->OptionPrice);;
	///期权对应期货的商品代码 直达
	doc.add("OptionCommodityNo", field->OptionCommodityNo);;
	///期权对应期货的合约代码 直达
	doc.add("OptionContractNo", field->OptionContractNo);;

	connect->sendRequest(*insertRequest);
	std::string lastError = db->getLastError(*connect);
	if (!lastError.empty())
	{
		std::cout << "Last Error: " << db->getLastError(*connect) << std::endl;
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
	doc.add("TradingDay", field->TradingDay);
	///合约代码  直达
	doc.add("InstrumentID", field->InstrumentID);
	///交易所代码   直达
	doc.add("ExchangeID", field->ExchangeID);
	///合约在交易所的代码  
	doc.add("ExchangeInstID", field->ExchangeInstID);
	///最新价  直达
	doc.add("LastPrice", field->LastPrice);
	///上次结算价  直达
	doc.add("PreSettlementPrice", field->PreSettlementPrice);
	///昨收盘  直达
	doc.add("PreClosePrice", field->PreClosePrice);
	///昨持仓量 直达
	doc.add("PreOpenInterest", field->PreOpenInterest);
	///今开盘  直达
	doc.add("OpenPrice", field->OpenPrice);
	///最高价  直达
	doc.add("HighestPrice", field->HighestPrice);
	///最低价  直达
	doc.add("LowestPrice", field->LowestPrice);
	///数量  直达
	doc.add("Volume", field->Volume);
	///成交金额
	doc.add("Turnover", field->Turnover);
	///持仓量  直达
	doc.add("OpenInterest", field->OpenInterest);
	///今收盘  直达
	doc.add("ClosePrice", field->ClosePrice);
	///本次结算价
	doc.add("SettlementPrice", field->SettlementPrice);
	///涨停板价
	doc.add("UpperLimitPrice", field->UpperLimitPrice);
	///跌停板价
	doc.add("LowerLimitPrice", field->LowerLimitPrice);
	///昨虚实度
	doc.add("PreDelta", field->PreDelta);
	///今虚实度
	doc.add("CurrDelta", field->CurrDelta);
	///最后修改时间  直达
	doc.add("UpdateTime", field->UpdateTime);
	///最后修改毫秒  直达
	doc.add("UpdateMillisec", field->UpdateMillisec);
	///申买价一  直达
	doc.add("BidPrice1", field->BidPrice1);
	///申买量一  直达
	doc.add("BidVolume1", field->BidVolume1);
	///申卖价一  直达
	doc.add("AskPrice1", field->AskPrice1);
	///申卖量一  直达
	doc.add("AskVolume1", field->AskVolume1);
	///申买价二  直达
	doc.add("BidPrice2", field->BidPrice2);
	///申买量二  直达
	doc.add("BidVolume2", field->BidVolume2);
	///申卖价二  直达
	doc.add("AskPrice2", field->AskPrice2);
	///申卖量二  直达
	doc.add("AskVolume2", field->AskVolume2);
	///申买价三  直达
	doc.add("BidPrice3", field->BidPrice3);
	///申买量三  直达
	doc.add("BidVolume3", field->BidVolume3);
	///申卖价三  直达
	doc.add("AskPrice3", field->AskPrice3);
	///申卖量三  直达
	doc.add("AskVolume3", field->AskVolume3);
	///申买价四  直达
	doc.add("BidPrice4", field->BidPrice4);
	///申买量四  直达
	doc.add("BidVolume4", field->BidVolume4);
	///申卖价四  直达
	doc.add("AskPrice4", field->AskPrice4);
	///申卖量四  直达
	doc.add("AskVolume4", field->AskVolume4);
	///申买价五  直达
	doc.add("BidPrice5", field->BidPrice5);
	///申买量五  直达
	doc.add("BidVolume5", field->BidVolume5);
	///申卖价五  直达
	doc.add("AskPrice5", field->AskPrice5);
	///申卖量五  直达
	doc.add("AskVolume5", field->AskVolume5);
	///当日均价  直达
	doc.add("AveragePrice", field->AveragePrice);
	///成交总数量  直达
	doc.add("TotalVolume", field->TotalVolume);
	
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