//
//  FutuHandle.cpp
//  webServer
//
//  Created by 龚坤 on 2017/10/12.
//  Copyright © 2017年 龚坤. All rights reserved.
 //第三方的api请求
//

#include "ApiHandle.h"
#include <iostream>
#include <fstream>
#include <string>   
#include <stdio.h>

#include "SqlHandle.h"

using namespace std;

MarketSpi::MarketSpi() {
	sqlhandle = new SqlHandle();
}

MarketSpi::~MarketSpi() {
	delete sqlhandle;
}

void MarketSpi::OnFrontConnected()
{
	cout << "Info connected sucessed!" << endl;
}

void MarketSpi::OnFrontDisconnected(int nReason)
{
	cout << "Info Disconnected!" << nReason << endl;
}

void MarketSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cout << "the heart beat" << endl;

}
void MarketSpi::OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (bIsLast == false)
		cout << "login:" << pRspUserLogin->UserID << " " << pRspUserLogin->AccountID << " " << pRspUserLogin->CurrencyNo << endl;
	else
		cout << "login over yes!" << endl;
}

void MarketSpi::OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "logout " << endl;
}

void MarketSpi::OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "error:" << pRspInfo->ErrorID << "===" << pRspInfo->ErrorMsg << endl;
}
void MarketSpi::OnRspSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	/*ofstream fout;
	fout.open("output.txt");
	string strday = "fuck";	
	fout << strday << "\n";
	fout.close();

	cout << "come here you ding yue " << endl;*/
}
void MarketSpi::OnRspUnSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "cancle the market data" << endl;
}

//market data
void MarketSpi::OnRtnDepthMarketData(CTShZdDepthMarketDataField *pDepthMarketData)
{
	cout << "M:" << pDepthMarketData->ExchangeID << " " << pDepthMarketData->InstrumentID << " "
		<< pDepthMarketData->TradingDay << " " << pDepthMarketData->AskPrice1 << " " << pDepthMarketData->UpdateTime << endl;
	/*try {
		sqlhandle->insertDeptMarketData(pDepthMarketData);
	}
	catch (Poco::Exception& exc) {
		cout <<"error while inserting DeptMarketData: "<< exc.displayText() << endl;
	}*/

	

	ofstream fout;
	fout.open("market.txt", 'w');
	
	fout<<"TradingDay: "<<pDepthMarketData->TradingDay<<"  InstrumentID: "<<pDepthMarketData->InstrumentID
		<<" ExchangeID: "<<pDepthMarketData->ExchangeID<<" ExchangeInstID: "<<pDepthMarketData->ExchangeInstID
		<<" LastPrice: "<<pDepthMarketData->LastPrice<<" PreSettlementPrice: "<<pDepthMarketData->PreSettlementPrice
		<<" PreClosePrice: "<<pDepthMarketData->PreClosePrice<<" PreOpenInterest: "<<pDepthMarketData->PreOpenInterest
		<<" OpenPrice: "<<pDepthMarketData->OpenPrice<<" HighestPrice: "<<pDepthMarketData->HighestPrice
		<<" LowestPrice: "<<pDepthMarketData->LowestPrice<<" Volume: "<<pDepthMarketData->Volume
		<<" Turnover: "<<pDepthMarketData->Turnover<<" OpenInterest: "<<pDepthMarketData->OpenInterest
		<<" ClosePrice: "<<pDepthMarketData->ClosePrice<<" SettlementPrice: "<<pDepthMarketData->SettlementPrice
		<<" UpperLimitPrice: "<<pDepthMarketData->UpperLimitPrice<<" LowerLimitPrice: "<<pDepthMarketData->LowerLimitPrice
		<<" PreDelta: "<<pDepthMarketData->PreDelta<<" UpdateTime: "<<pDepthMarketData->UpdateTime
		<<" BidPrice1: "<<pDepthMarketData->BidPrice1<<" BidVolume1: "<<pDepthMarketData->BidVolume1
		<<" AveragePrice: "<<pDepthMarketData->AveragePrice<<" TotalVolume: "<<pDepthMarketData->TotalVolume<<endl;

	/*fout << pDepthMarketData->ExchangeID << "  " << pDepthMarketData->InstrumentID << "  " << pDepthMarketData->LastPrice
		<< "  " << pDepthMarketData->AveragePrice << "  " <<pDepthMarketData->LowestPrice<<"  "
		<<pDepthMarketData->HighestPrice<<"  "<< pDepthMarketData->TradingDay << endl;*/
	fout.close();
}
//filled data
void MarketSpi::OnRtnFilledMarketData(CTShZdFilledDataField* pFilledMarketData)
{
	cout << "F:" << pFilledMarketData->ExchangeID << " " << pFilledMarketData->InstrumentID << " "
		<< pFilledMarketData->Volume << " " << pFilledMarketData->LastPrice << " " << pFilledMarketData->FilledVolume
		<< " " << pFilledMarketData->UpdateTime << endl;
	try {
		sqlhandle->insertFilledData(pFilledMarketData);
	}
	catch (Poco::Exception& exc) {
		cout <<"error while inserting FilledData: "<< exc.displayText() << endl;
	}
}


//trade
//trade
TradeSpi::TradeSpi() {
	sqlhandle = new SqlHandle();
}
TradeSpi::~TradeSpi() {
	delete sqlhandle;
}
void TradeSpi::OnFrontConnected()
{
	cout << "Info trade connected sucessed!" << endl;
}
void TradeSpi::OnFrontDisconnected(int nReason)
{
	cout << "Info trade dis connected sucessed!" << endl;
}
void TradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
}

void TradeSpi::OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!bIsLast) {
		cout << "login:" << pRspUserLogin->UserID << " " << pRspUserLogin->AccountID << " " << pRspUserLogin->CurrencyNo << " "
			<< pRspUserLogin->UserName << " " << nRequestID << endl;
		ifTrade = true;

		//if (nRequestID == 100) {  //request Mar and contract
		//	cout << "login successful" << endl;
		//}
		//else { //other things

		//}

	}
}
void TradeSpi::OnRspUserLogout(CTShZdUserLogoutField *pUserLogout,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "pwdUpdate:" << pUserPasswordUpdate->NewPassword << " " <<
		nRequestID << endl;
}

void TradeSpi::OnRspOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "sysNo:" << pInputOrder->OrderType << " " << pRspInfo->ErrorMsg << " " << nRequestID << endl;
}
void TradeSpi::OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspOrderAction(CTShZdInputOrderActionField *pInputOrderAction,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction->ActionFlag == '0')
	{
		cout << "cancel:" << pInputOrderAction->OrderRef << " " << pInputOrderAction->OrderSysID << " " << pRspInfo->ErrorMsg << endl;
	}
	else
	{
		cout << "modify:" << pInputOrderAction->OrderRef << " " << pInputOrderAction->OrderSysID << " " << pRspInfo->ErrorMsg << endl;
		cout << "改单后数量：" << pInputOrderAction->VolumeChange << " " << "改单后的价格：" << pInputOrderAction->LimitPrice << endl;
	}
}
void TradeSpi::OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspQryOrder(CTShZdOrderField *pOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "OrderList:" << pOrder->UserID << " " << pOrder->InstrumentID << " " << pOrder->OrderRef << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "OrderList: over" << endl;
}
void TradeSpi::OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "FilledList:" << pTrade->UserID << " " << pTrade->ExchangeID << " " << pTrade->InstrumentID << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "FilledList: over" << endl;
}
void TradeSpi::OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ //持仓汇总
	cout << "Position:" << pInvestorPosition->InstrumentID << " " << pInvestorPosition->PositionProfit << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "Position: over" << endl;
}
void TradeSpi::OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "Account:" << pTradingAccount->AccountID << " " << string(pTradingAccount->CurrencyNo).substr(0, 8).c_str() << " "
		<< pTradingAccount->CurrUse << " " << nRequestID << endl;
	if (bIsLast)
		cout << "Account:over" << endl;
}
//exchange jiao yi suo
void TradeSpi::OnRspQryExchange(CTShZdExchangeField *pExchange, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	//insert into mongoDB
	SqlHandle sqlhandle;
	try
	{
		//judge the ExchangeID exists
		if (!sqlhandle.checkExchange(pExchange->ExchangeID)) {
			sqlhandle.insertExanges(pExchange);
		}
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}

	ofstream fout;
	fout.open("exCode.txt",ios::app);
	//cout << "Exchange:" << pExchange->ExchangeID << "  " << pExchange->ExchangeName << " "
	//	<< nRequestID << endl;
	cout << "exCode: " << pExchange->ExchangeID << "	exName: " << pExchange->ExchangeName << "	"
		<< "exPt: " << pExchange->ExchangeProperty << "	reqID: "<<nRequestID<<endl;
	fout << "exCode: " << pExchange->ExchangeID << "	exName: " << pExchange->ExchangeName << "	"
		<< "exPt: " << pExchange->ExchangeProperty << "	reqID: " << nRequestID << endl;
	fout.close();
	if (bIsLast)
		cout << "Exchange: over!" << endl;
}
//heyue
void TradeSpi::OnRspQryInstrument(CTShZdInstrumentField *pInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "RspQryInstrument" << endl;
	if (pRspInfo->ErrorID != 0)
		return;
	//insert into mongoDB 
	if (strlen(pInstrument->InstrumentID)==0) {
		return;
	}
	cout << "ID:--->" << pInstrument->InstrumentID <<" exName: "<<pInstrument->ExchangeName<< endl;
	try {
		sqlhandle->insertInstruments(pInstrument);
	}
	catch (Poco::Exception& exc) {
		cout <<"error:"<< exc.displayText() << endl;
	}
	/*cout << "Instrument:" << pInstrument->ExchangeID << " " << pInstrument->InstrumentID << " "
		<< nRequestID << endl;*/
	
	//ofstream fout;
	//fout.open("conCode.txt",ios::app);

	////cout << "exCode: " << pInstrument->ExchangeID << "  conCode: " << pInstrument->InstrumentID << "  exconCode:" <<
	//	//pInstrument->ExchangeInstID << "  conName:" << pInstrument->ProductName << "  proID: " << pInstrument->ProductID << endl;
	//
	//fout << "ExchangeID: " << pInstrument->ExchangeID << "  InstrumentID: " << pInstrument->InstrumentID << "  ExchangeInstID:" <<
	//	pInstrument->ExchangeInstID << "  ProductName:" << pInstrument->ProductName << "  ProductID: " << pInstrument->ProductID << "  Expired: " << pInstrument->ExpireDate 
	//	<<"  InstrumentName: "<<pInstrument->InstrumentName<<" ExchangeName: "<<pInstrument->ExchangeName
	//	<<"  ProductClass: "<<pInstrument->ProductClass<<" CurrencyNo: "<<pInstrument->CurrencyNo
	//	<<"  CurrencyName: "<<pInstrument->CurrencyName
	//	<<"  isTrading: "<<pInstrument->IsTrading<<endl;

	//fout.close();

	if (bIsLast) {
		cout << "Instrument: over!" << endl;
	}
}

void TradeSpi::OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ //持仓明细
	cout << "PositionDetail:" << pInvestorPositionDetail->ExchangeID << "  " << pInvestorPositionDetail->Direction << " "
		<< pInvestorPositionDetail->Volume << " " << pInvestorPositionDetail->InstrumentID << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "PositionDetail:over" << endl;
}

void TradeSpi::OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "InOutMoney:" << pTransferSerial->UserID << " " << pTransferSerial->Amount << " " << pTransferSerial->InOutMedth <<
		" " << nRequestID << endl;
	if (bIsLast)
		cout << "InOutMoney:over" << endl;
}

void TradeSpi::OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "ErrorInfo:" << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
	if (bIsLast)
		cout << "ErrorInfo:over" << endl;
}

void TradeSpi::OnRtnOrder(CTShZdOrderField *pOrder)
{
	cout << "orderback:" << pOrder->OrderRef << " " << pOrder->OrderSubmitStatus << " " << pOrder->OrderSysID << " " << pOrder->LimitPrice << endl;
}
void TradeSpi::OnRtnTrade(CTShZdTradeField *pTrade)
{
	cout << "tradeback:" << pTrade->OrderSysID << " " << pTrade->TradeID << " " << pTrade->OrderRef << " " << pTrade->Price << " " << pTrade->Volume << endl;
}
void TradeSpi::OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo)
{
	cout << "Order Error" << pInputOrder->OrderLocalID << " " << pInputOrder->OrderSysID << " " << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
}

void TradeSpi::OnRtnErrorConditionalOrder(CTShZdErrorConditionalOrderField *pErrorConditionalOrder)
{
}

void TradeSpi::OnRspQryParkedOrder(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspQryParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

void TradeSpi::OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast)
{
	cout << "OpenCloseTime:" << openCloseTime->TradeDate << " " << openCloseTime->OpenTime << " " << openCloseTime->CloseTime << endl;
}

void TradeSpi::OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast)
{
	cout << "OpenCloseSumWinTime:" << openCloseTime->CurrYear << " " << openCloseTime->ExchangeID << " " << openCloseTime->SummeyBeginTime
		<< " " << openCloseTime->WinterBeginTime << " " << nRequestID << endl;
}

///直达支持产品的开收盘时间
void TradeSpi::OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime, int nRequestID, bool bIsLast)
{
	cout << "OpenCloseSumWin:" << commonTime->ProductID << " " << commonTime->NomalSummerWinter << " " << commonTime->OpenTime
		<< " " << commonTime->CloseTime << " " << nRequestID << endl;
}

void TradeSpi::OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney)
{
	cout << "OnRtnTradeMoney:" << pTradeMoney->UserID << " " << pTradeMoney->AccountID << " " << pTradeMoney->FrozenDeposit
		<< " " << pTradeMoney->Commission << endl;
}

void TradeSpi::OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation, int nRequestID, bool bIsLast)
{
	cout << "Ratio:" << rspRation->CurrencyID << " " << rspRation->Ratio << " " << nRequestID << endl;
}
