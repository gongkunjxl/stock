#ifndef ApiHandle_h
#define ApiHandle_h

#include "ShZdFutureMarketApi.h"
#include "ShZdFutureUserApiDataType.h"
#include "ShZdFutureUserApiStruct.h"
#include "ShZdFutureTradeApi.h"

#include <fstream>
#include <io.h>


//#include <windows.h>

//the MarketSpi
class MarketSpi:public CSHZdMarketSpi
{
public:
	//构造和析构函数
	//MarketSpi();
	//~MarketSpi();

	void OnFrontConnected();
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);
	void OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRspUnSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRtnDepthMarketData(CTShZdDepthMarketDataField *pDepthMarketData);
	void OnRtnFilledMarketData(CTShZdFilledDataField* pFilledMarketData);
};

class TradeSpi :public CSHZdTraderSpi
{
public:
	void OnFrontConnected();
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);
	void OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspOrderAction(CTShZdInputOrderActionField *pInputOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);;
	void OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryOrder(CTShZdOrderField *pOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnTrade(CTShZdTradeField *pTrade);
	void OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryExchange(CTShZdExchangeField *pExchange, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryInstrument(CTShZdInstrumentField *pInstrument, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//14
	void OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnOrder(CTShZdOrderField *pOrder);
	void OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney);
	void OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo);
	void OnRtnErrorConditionalOrder(CTShZdErrorConditionalOrderField *pErrorConditionalOrder);
	void OnRspQryParkedOrder(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast);
	void OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast);
	void OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime, int nRequestID, bool bIsLast);
	void OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation, int nRequestID, bool bIsLast);
};




#endif /* FutuHandle_hpp */
