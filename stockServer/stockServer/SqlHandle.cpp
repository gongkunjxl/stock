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
	std::cout << "*** INSERT INSTRUMENTS ***" << std::endl;
	SharedPtr<InsertRequest> insertRequest = 
		db->createInsertRequest(instrumentCollectionName);
	Document& doc = insertRequest->addNewDocument();
	///合约代码  直达
	doc.add<string>("InstrumentID", field->InstrumentID);;
	///交易所代码  直达
	doc.add<string>("ExchangeID", field->ExchangeID);;
	///合约名称  直达
	doc.add<string>("InstrumentName", field->InstrumentName);;
	///合约在交易所的代码  直达
	doc.add<string>("ExchangeInstID", field->ExchangeInstID);;
	///交易所名称  直达
	doc.add<string>("ExchangeName", field->ExchangeName);;
	///产品代码  直达
	doc.add<string>("ProductID", field->ProductID);;
	///产品名称  直达
	doc.add<string>("ProductName", field->ProductName);;
	///产品类型  F期货 O期权  直达
	doc.add<int>("ProductClass", field->ProductClass);;
	///合约货币代码  直达
	doc.add<string>("CurrencyNo", field->CurrencyNo);;
	///货币名称  直达
	doc.add<string>("CurrencyName", field->CurrencyName);;	
	///行情小数为数 直达
	doc.add<int>("MarketDot", field->MarketDot);;
	///行情进阶单位 10进制 32进制  64进制等 直达
	doc.add<int>("MarketUnit", field->MarketUnit);;
	///调期小时点位数  直达
	doc.add<double>("ChangeMarketDot", field->ChangeMarketDot);;
	///合约数量乘数  点值（一个最小跳点的价值） 直达
	doc.add<double>("VolumeMultiple", field->VolumeMultiple);;
	///调期最小变动单位  直达
	doc.add<double>("ChangeMultiple", field->ChangeMultiple);;
	///最小变动价位  直达
	doc.add<double>("PriceTick", field->PriceTick);;	
	///交割月日  直达
	doc.add<string>("StartDelivDate", field->StartDelivDate);;
	///最后更新日  直达
	doc.add<string>("LastUpdateDate", field->LastUpdateDate);;
	///首次通知日 直达
	doc.add<string>("ExpireDate", field->ExpireDate);;
	///最后交易日  直达
	doc.add<string>("EndTradeDate", field->EndTradeDate);;	
	///当前是否交易
	doc.add<int>("IsTrading", field->IsTrading);;
	///期权类型
	doc.add<int>("OptionType", field->OptionType);;
	///期权年月  直达
	doc.add<string>("OptionDate", field->OptionDate);;
	///保证金率  直达
	doc.add<double>("MarginRatio", field->MarginRatio);;
	///固定保证金  直达
	doc.add<double>("MarginValue", field->MarginValue);;
	///手续费率  直达
	doc.add<double>("FreeRatio", field->FreeRatio);;
	///固定手续费  直达
	doc.add<double>("FreeValue", field->FreeValue);;
	///现货商品昨结算价  直达
	doc.add<double>("SpotYesSetPrice", field->SpotYesSetPrice);;
	///现货商品点值  直达
	doc.add<double>("SpotMultiple", field->SpotMultiple);;
	///现货商品最小变动单位  直达
	doc.add<double>("SpotTick", field->SpotTick);;
	///期权临界价格  直达
	doc.add<double>("OptionTickPrice", field->OptionTickPrice);;
	///期权临界价格以下最小跳点  直达
	doc.add<double>("OptionTick", field->OptionTick);;
	///期权执行价  直达
	doc.add<double>("OptionPrice", field->OptionPrice);;
	///期权对应期货的商品代码 直达
	doc.add<string>("OptionCommodityNo", field->OptionCommodityNo);;
	///期权对应期货的合约代码 直达
	doc.add<string>("OptionContractNo", field->OptionContractNo);;

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
