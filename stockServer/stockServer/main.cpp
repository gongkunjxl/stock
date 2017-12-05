

#include "ApiHandle.h"
#include "ClientResponse.h"
#include "SqlHandle.h"
#include <iostream>
#include <fstream>
#include <string>    //iostream
#include <stdio.h>
#include <cstdio>
#include <vector>
//#include <Windows.h>

//Net
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPCredentials.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"

//Util
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <Poco/Util/Application.h>
#include "Poco/Format.h"
#include "Poco/StringTokenizer.h"

//json
#include <stdint.h>
#include <sstream>
//#include "Poco/JSON/Parser.h"
//#include "Poco/JSON/ParseHandler.h"
//#include "Poco/JSON/JSONException.h"
//#include "Poco/StreamCopier.h"
//#include "Poco/Dynamic/Var.h"
//#include "Poco/JSON/Query.h"
//#include "Poco/JSON/PrintHandler.h"
//
////json
//using namespace Poco::Dynamic;
//using namespace Poco;
using std::string;


using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPCredentials;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using namespace std;

//global var
CSHZdMarketApi *apiHandle;
CSHZdTraderApi* apiTrade;
ClientResponse *repHandle;

vector<conWeb> cliWebsocket;	//用于维护连接的队列 否则报错
//void ChangeStatus(WebSocket* conWS, string type, char status, string exCode, string conCode, string kType,vector<pair<string,string>> exCon,int flags);
DWORD WINAPI handleRequest(LPVOID lpparentet);   //用于处理

//===================================================================================
string IntToStr(int index)
{
	char temp[64];
	string str;
	printf(temp, "%d", index);
	str = string(temp);
	return str;
}


void testFutur()
{
	string selectchar = "";
	selectchar = getchar();
	int index = 0;

	if (selectchar.compare("m") == 0)
	{
		cout << "input m" << endl;
		selectchar = getchar();
		selectchar = getchar();
		while (selectchar.compare("e") != 0) {
			if (selectchar.compare("a") == 0)
			{

				//cout << "come m_e_a start" << endl;
				apiHandle = CSHZdMarketApi::CreateSHZdMarketApi("..\\marketLog", false);
				apiHandle->RegisterSpi(new MarketSpi);
				apiHandle->Init();
				apiHandle->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
				
				//apiHandle->RegisterLoginFront("protocol://222.73.119.230:7003");
				//apiHandle->RegisterFront("protocol://222.73.105.170:9002");
				apiHandle->RegisterLoginFront("protocol://120.132.7.159:7993");
				
				//cout << "register first: " << endl;
				//Sleep(2000);
				CTShZdReqUserLoginField field;
				memset(&field, 0, sizeof(CTShZdReqUserLoginField));
				memcpy(field.UserID, "91000001", 16);
				memcpy(field.Password, "111111", 41);
				apiHandle->ReqUserLogin(&field, 1);
				cout << "login : yes" << endl;

				//Sleep(2000);
				////ofstream fout;
				////fout.open("output.txt");
				string strday(apiHandle->GetTradingDay());
				cout << "trade day--->" << strday << endl;
				//
				//fout << strday << "\n";
				//fout.close();
				
				apiHandle->RegisterFront("protocol://222.73.123.120:9001");
				//apiHandle->RegisterFront("protocol://222.73.105.170:9002");
				
				//apiHandle->RegisterLoginFront("protocol://222.73.123.120:9001");
				//cout << "register towice" << endl;

				char *ppInstrumentID[1];
				string temp1 = "HKEX,HHI1710;HKEX,HHI1711;HKEX,HHI1712";
				//string temp2 = "HKEX,HHI1707;HKEX,HHI1708;HKEX,HHI1709;HKEX,HHI1710";
				//string temp3 = "HKEX,HSI1702;HKEX,HSI1703;HKEX,HSI1704;HKEX,HSI1705;HKEX,HSI1706";
				//string temp4 = "HKEX,HSI1707;HKEX,HSI1708;HKEX,HSI1709;HKEX,HSI1710";
				//string temp5 = "ICE,BRN1704;ICE,BRN1705;ICE,BRN1706;ICE,BRN1707;ICE,BRN1708";
				///string temp6 = "ICE,BRN1709;ICE,BRN1710;ICE,BRN1711;ICE,BRN1712";
				//string temp7 = "LME,AA3M;LME,AH3M";
				//string temp8 = "LME,CA3M";
				ppInstrumentID[0] = (char*)temp1.c_str();
			/*	ppInstrumentID[1] = (char*)temp2.c_str();
				ppInstrumentID[2] = (char*)temp3.c_str();
				ppInstrumentID[3] = (char*)temp4.c_str();
			*/	//ppInstrumentID[4] = (char*)temp5.c_str();
				//ppInstrumentID[5] = (char*)temp6.c_str();
				//ppInstrumentID[6] = (char*)temp7.c_str();
				//ppInstrumentID[7] = (char*)temp8.c_str();
				cout << "start ding yue" << endl;
				apiHandle->SubscribeMarketData(ppInstrumentID, 1);

				cout << "ding yue wan bi" << endl;

			}
			else if (selectchar.compare("f") == 0)
			{
				apiHandle->GetTradingDay();
				char *ppInstrumentID[4];
				string temp1 = "HKEX,HHI1702;HKEX,HHI1703;HKEX,HHI1704;HKEX,HHI1705;HKEX,HHI1706";
				string temp2 = "HKEX,HHI1707;HKEX,HHI1708;HKEX,HHI1709;HKEX,HHI1711";
				string temp3 = "HKEX,HSI1702;HKEX,HSI1703;HKEX,HSI1704;HKEX,HSI1705;HKEX,HSI1706";
				string temp4 = "HKEX,HSI1707;HKEX,HSI1708;HKEX,HSI1709;HKEX,HSI1710";
				//string temp5 = "ICE,BRN1704;ICE,BRN1705;ICE,BRN1706;ICE,BRN1707;ICE,BRN1708";
				///string temp6 = "ICE,BRN1709;ICE,BRN1710;ICE,BRN1711;ICE,BRN1712";
				//string temp7 = "LME,AA3M;LME,AH3M";
				//string temp8 = "LME,CA3M";
				ppInstrumentID[0] = (char*)temp1.c_str();
				ppInstrumentID[1] = (char*)temp2.c_str();
				ppInstrumentID[2] = (char*)temp3.c_str();
				ppInstrumentID[3] = (char*)temp4.c_str();
				//ppInstrumentID[4] = (char*)temp5.c_str();
				//ppInstrumentID[5] = (char*)temp6.c_str();
				//ppInstrumentID[6] = (char*)temp7.c_str();
				//ppInstrumentID[7] = (char*)temp8.c_str();
				cout << "start ding yue" << endl;
				apiHandle->SubscribeMarketData(ppInstrumentID, 4);

				cout << "ding yue wan bi" << endl;
				
			}
			else if (selectchar.compare("r") == 0)
			{
				apiHandle->Release();
			}
			else if (selectchar.compare("k") == 0)
			{
				char *ppInstrumentID[2];
				string temp = "CME,6B1702;CME,GC1701;CME,6J1702;CME,GCK1702";
				ppInstrumentID[0] = (char*)temp.c_str();
				string temp1 = "HKEX,HSI1703;HKEX,HSI1703";
				ppInstrumentID[1] = (char*)temp1.c_str();
				apiHandle->SubscribeMarketData(ppInstrumentID, 2);
			
			}
			selectchar = getchar();
		}
	}
	else if (selectchar.compare("t") == 0)
	{
		selectchar = getchar();
		selectchar = getchar();
		while (selectchar.compare("e") != 0)
		{
			cout << "come t_X_b" << endl;
			if (selectchar.compare("a") == 0)
			{
				apiTrade = CSHZdTraderApi::CreateSHZdTraderApi("..\\tradeLog", false);
				apiTrade->RegisterSpi(new TradeSpi);
				apiTrade->Init();
				apiTrade->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
				apiTrade->RegisterFront("protocol://222.73.119.230:7003");// 222.73.119.230:7003      
			}
			else if (selectchar.compare("l") == 0)
			{ //登陆
				CTShZdReqUserLoginField field;
				memset(&field, 0, sizeof(CTShZdReqUserLoginField));
				memcpy(field.UserID, "MN000301", 16);//f000202
				memcpy(field.Password, "888888", 41);
				apiTrade->ReqUserLogin(&field, 300);
				Sleep(500);
				apiTrade->GetTradingDay();
			}
			else if (selectchar.compare("u") == 0)
			{//用户登出
				CTShZdUserLogoutField pLogout;
				memset(&pLogout, 0, sizeof(CTShZdUserLogoutField));
				memcpy(pLogout.UserID, "MN000301", 16);
				apiTrade->ReqUserLogout(&pLogout, 0);
			}
			else if (selectchar.compare("r") == 0)
			{ //退出dll
				apiTrade->Release();
			}
			else if (selectchar.compare("p") == 0)
			{//修改密码
				CTShZdUserPasswordUpdateField pPwdUpdate;
				memset(&pPwdUpdate, 0, sizeof(CTShZdUserPasswordUpdateField));
				memcpy(&pPwdUpdate.UserID, "MN000301", 16);
				memcpy(&pPwdUpdate.OldPassword, "666666", 41);
				memcpy(&pPwdUpdate.NewPassword, "888888", 41);
				apiTrade->ReqUserPasswordUpdate(&pPwdUpdate, 0);
			}
			else if (selectchar.compare("b") == 0)
			{
				CTShZdInputOrderField pOrder;
				memset(&pOrder, 0, sizeof(CTShZdInputOrderField));

				memcpy(pOrder.OrderLocalID, "20161128141700", 14);
				memcpy(pOrder.ExchangeID, "CME", 11);//交易所
				memcpy(pOrder.InvestorID, "MN00000903", 13);//资金帐号

				memcpy(pOrder.InstrumentID, "CL1703", 31);
				memcpy(pOrder.UserID, "MN000301", 16);
				pOrder.Direction = '1';
				pOrder.VolumeTotalOriginal = 3;
				pOrder.LimitPrice = atof("45");
				pOrder.OrderPriceType = '2';
				pOrder.OrderType = 'P';
				apiTrade->ReqOrderInsert(&pOrder, 0);
			}
			else if (selectchar.compare("s") == 0)
			{
				CTShZdInputOrderField pOrder;
				memset(&pOrder, 0, sizeof(CTShZdInputOrderField));
				memcpy(pOrder.OrderLocalID, "20161128141700", 14);
				memcpy(pOrder.ExchangeID, "CME", 11);//交易所
				memcpy(pOrder.InvestorID, "MN00000903", 13);//资金帐号				
				memcpy(pOrder.InstrumentID, "CL1703", 31);
				memcpy(pOrder.UserID, "TJ000771", 16);
				pOrder.Direction = '2';
				pOrder.VolumeTotalOriginal = 3;
				pOrder.LimitPrice = atof("60");
				pOrder.OrderPriceType = '1';
				pOrder.OrderType = 'P';
				apiTrade->ReqOrderInsert(&pOrder, 0);

			}
			else if (selectchar.compare("c") == 0)
			{//撤单
				CTShZdOrderActionField pCancel;
				memset(&pCancel, 0, sizeof(CTShZdInputOrderActionField));

				printf("依次输入系统号，订单号(空格分开)\n");
				//	scanf("%s%s", pCancel.OrderRef, pCancel.OrderSysID);
				cin >> pCancel.OrderRef >> pCancel.OrderSysID;
				pCancel.ActionFlag = '0';//0是撤单
				pCancel.OrderType = 'P';
				apiTrade->ReqOrderAction(&pCancel, 0);
			}
			else if (selectchar.compare("m") == 0)
			{//改单
				CTShZdOrderActionField pModify;
				memset(&pModify, 0, sizeof(CTShZdInputOrderActionField));

				printf("依次输入订单号，系统号（空格分开）\n");
				//	scanf("%s%s", pModify.OrderSysID, pModify.OrderRef);
				cin >> pModify.OrderSysID >> pModify.OrderRef;
				pModify.VolumeChange = 4;//改单后的数量
				pModify.LimitPrice = 46.00;//改单价格
										   //pModify.ModifyTriggerPrice = atof("0.00");
				pModify.ActionFlag = '3';//3是改单
				pModify.OrderType = 'P';
				apiTrade->ReqOrderAction(&pModify, 0);
			}
			else if (selectchar.compare("5") == 0)
			{//委托查询
				CTShZdQryOrderField pQryOrder;
				memset(&pQryOrder, 0, sizeof(CTShZdQryOrderField));
				memcpy(pQryOrder.UserID, "MN000301", 13);
				apiTrade->ReqQryOrder(&pQryOrder, 5);
			}
			else if (selectchar.compare("6") == 0)
			{//成交查询
				CTShZdQryTradeField pQryTrade;
				memset(&pQryTrade, 0, sizeof(CTShZdQryTradeField));
				memcpy(pQryTrade.UserID, "MN000301", 13);
				apiTrade->ReqQryTrade(&pQryTrade, 6);
			}
			else if (selectchar.compare("7") == 0)
			{//资金查询
				CTShZdQryTradingAccountField pQryTradingAccount;
				memset(&pQryTradingAccount, 0, sizeof(CTShZdQryTradingAccountField));
				memcpy(pQryTradingAccount.UserID, "MN000301", 13);
				apiTrade->ReqQryTradingAccount(&pQryTradingAccount, 7);
			}
			else if (selectchar.compare("8") == 0)
			{//交易所查询
				CTShZdQryExchangeField pQryExchange;
				memset(&pQryExchange, 0, sizeof(CTShZdQryExchangeField));
				memcpy(pQryExchange.ExchangeID, "", 9);
				apiTrade->ReqQryExchange(&pQryExchange, 8);
			}
			else if (selectchar.compare("9") == 0)
			{//合约查询
				index++;
				CTShZdQryInstrumentField pQryInstrument;
				memset(&pQryInstrument, 0, sizeof(CTShZdQryInstrumentField));
				memcpy(pQryInstrument.ExchangeID, "", 9);
				memcpy(pQryInstrument.InsertTimeStart, "20170101", 10);
				memcpy(pQryInstrument.ProductID, "", 9);
				pQryInstrument.Index = index * 500; //
				apiTrade->ReqQryInstrument(&pQryInstrument, 9);
			}
			else if (selectchar.compare("z") == 0)
			{//持仓明细查询
				CTShZdQryInvestorPositionDetailField pQryInvestorPositionDetail;
				memset(&pQryInvestorPositionDetail, 0, sizeof(CTShZdQryInvestorPositionDetailField));
				memcpy(pQryInvestorPositionDetail.UserID, "MN000301", 13);
				apiTrade->ReqQryInvestorPositionDetail(&pQryInvestorPositionDetail, 11);
			}
			//the chi cang hui zong
			else if (selectchar.compare("y") == 0)
			{
				CTShZdQryInvestorPositionField pQryInvestorPosition;
				memset(&pQryInvestorPosition, 0, sizeof(CTShZdQryInvestorPositionField));
				memcpy(pQryInvestorPosition.UserID, "MN000301", 13);
				memcpy(pQryInvestorPosition.InvestorID, "MN00000903", 18);
				apiTrade->ReqQryInvestorPosition(&pQryInvestorPosition, 12);
			}
			else if (selectchar.compare("x") == 0) //直达开收盘时间
			{
				CTShZdReqOpenCloseTimeField OpenCloseTime;
				memcpy(OpenCloseTime.UserID, "MN000301", 13);
				apiTrade->ReqQueryOpenCloseTime(&OpenCloseTime, 13);
			}
			else if (selectchar.compare("w") == 0)//chu ru jin
			{
				CTShZdQryTransferSerialField pQryTransferSerial;
				memset(&pQryTransferSerial, 0, sizeof(CTShZdQryTransferSerialField));
				memcpy(pQryTransferSerial.UserID, "MN000301", 13);
				pQryTransferSerial.InOutMedth = 'K';
				pQryTransferSerial.InOutMoneyType = 'I';
				pQryTransferSerial.Amount = 6000;
				pQryTransferSerial.InOutMoneyUsing = 'F';
				memcpy(pQryTransferSerial.CurrencyID, "USD", 5);
				apiTrade->ReqQryTransferSerial(&pQryTransferSerial, 14);
			}
			else if (selectchar.compare("j") == 0) // zhi da jiao yi xia dong ling shi
			{
				CTShZdReqMarketOpenCloseTimeField OpenCloseTime;
				memset(&OpenCloseTime, 0, sizeof(CTShZdReqMarketOpenCloseTimeField));
				memcpy(OpenCloseTime.UserID, "demo000177", 16);
				apiTrade->ReqQueryMarketOpenCloseTime(&OpenCloseTime, 15);
			}
			else if (selectchar.compare("k") == 0)
			{ //产品的开收盘时间
				CTShZdReqCommonOpenCloseTimeField comTime;
				memset(&comTime, 0, sizeof(CTShZdReqCommonOpenCloseTimeField));
				memcpy(comTime.UserID, "demo000177", 16);
				memcpy(comTime.ExchangeID, "CME", 9);
				apiTrade->ReqQueryCommonOpenCloseTime(&comTime, 15);
			}
			else if (selectchar.compare("f") == 0)
			{//循环报单
				int i = 0;
				while (i<50)
				{
					i++;
					int j = i % 2;
					CTShZdInputOrderField pOrder;
					memset(&pOrder, 0, sizeof(CTShZdInputOrderField));
					string tem = "2016112814000" + IntToStr(i);
					memcpy(pOrder.OrderLocalID, tem.c_str(), 14);
					memcpy(pOrder.ExchangeID, "CME", 11);//交易所
					memcpy(pOrder.InvestorID, "MN00000901", 13);//资金帐号				
					memcpy(pOrder.InstrumentID, "CL1703", 31);
					memcpy(pOrder.UserID, "MN000301", 16);
					pOrder.VolumeTotalOriginal = 3;
					pOrder.LimitPrice = atof("60");
					pOrder.OrderPriceType = '2';
					if (j == 0)
					{
						pOrder.Direction = '2';
					}
					else
					{
						pOrder.Direction = '1';
					}
					apiTrade->ReqOrderInsert(&pOrder, 500 + i);
					Sleep(200);
				}
			}
			else  if (selectchar.compare("q") == 0)
			{//汇率
				CTShZdReqMoneyRatioField reqRation;
				memset(&reqRation, 0, sizeof(CTShZdReqMoneyRatioField));
				memcpy(reqRation.UserID, "MN000301", 9);
				apiTrade->ReqQueryMoneyRatio(&reqRation, 20);
			}
			selectchar = getchar();
		}
		selectchar = getchar();
	}
}

//json
string JsonGet()
{
	/*JSON::Object jsnObj;
    JSON::Array jsnArry;
    JSON::Object subObj1;
    JSON::Object subObj2;

    jsnObj.set( "command", "createuser" );

    subObj1.set( "name", "yuhaiyang");
    subObj1.set( "pass", "123" );

    subObj2.set( "name", "cj" );
    subObj2.set( "pass", "456" );

    jsnArry.add( subObj1 );
    jsnArry.add( subObj2 );

    jsnObj.set( "userinfo", jsnArry );

    std::stringstream  jsnString;
    jsnObj.stringify( jsnString, 3 );
    std::cout << jsnString.str() << std::endl;
*/


//	JSON::Object jsnObj;
	JSON::Object jsnObj1;
	JSON::Object jsnObj2;
	JSON::Array jsnArry1;
	JSON::Array jsnArry2;
	JSON::Array jsnArry3;
//	/*JSON::Object subObj1;
//	JSON::Object subObj2;*/
//
//	//jsnObj.set("command", "createuser");
//
//	/*subObj1.set("name", "yuhaiyang");
//	subObj1.set("pass", "123");
//
//	subObj2.set("name", "cj");
//	subObj2.set("pass", "456");*/
//
//	//	jsnArry.add(subObj1);
//	//jsnArry.add(subObj2);
	string fir = "first";
	Dynamic::Var ret = fir;
	Dynamic::Var ret1 = "second";
	Dynamic::Var ret2 = "third";
	jsnArry1.add(ret);
	jsnArry1.add(ret1);
	jsnArry1.add(ret2);

	jsnArry2.add(ret);
	jsnArry2.add(ret1);
	jsnArry2.add(ret2);

	jsnArry3.add(jsnArry1);
	jsnArry3.add(jsnArry2);
//
//	//jsnObj.set("data1", jsnArry1);
//	//jsnObj.set("data2", jsnArry1);
//
//	//jsnObj2.set("data", jsnObj);
	jsnObj2.set("data", jsnArry3);
	jsnObj2.set("type", "MAR");
//
	std::stringstream  jsnString;
	jsnObj2.stringify(jsnString, 3);
	std::cout << jsnString.str() << std::endl;
//
//
//	JSON::Parser parse;
//	Dynamic::Var json = parse.parse(jsnString.str());
//	JSON::Object::Ptr pObj = json.extract<JSON::Object::Ptr>();
//	JSON::Array::Ptr pArry = pObj->getArray("data");
//	string tmp = pObj->get("type").toString();
//	cout << "type:  " << tmp << endl;
//
//	int size = pArry->size();
//	for (int i = 0; i < size; i++)
//	{
//		JSON::Array::Ptr mArry = pArry->getArray(i);
//		for (int j = 0; j < mArry->size(); j++) {
//			cout << i << "---" << j << "--->"<<mArry->get(j).toString()<<endl;
//		}
//		//cout << i << "---->" << pArry->get(i).toString() << endl;
//	}
//
//	//string testStr = "{\"data\":[\"A\",\"B\",\"C\",\"D\"]}";
//
//	/*JSON::Parser parse;
//	Dynamic::Var json = parse.parse(jsnString.str());
//	JSON::Object::Ptr pObj = json.extract<JSON::Object::Ptr>();
//	JSON::Object::Ptr ppObj = pObj->getObject("data");
//
//	JSON::Array::Ptr pArry = ppObj->getArray("data1");
//
//	int size = pArry->size();
//	for (int i = 0; i < size; i++)
//	{
//		cout << i << "---->" << pArry->get(i).toString() << endl;
//	}*/
//	return jsnString.str();
	string hel="thlads";
	return hel;
}

// update the exCode 
void UpdateExchange()
{
	//register
	cout<<"----register--"<<endl;
	CSHZdTraderApi* tmpTrade;
	tmpTrade = CSHZdTraderApi::CreateSHZdTraderApi("..\\tradeLog", false);
	tmpTrade->RegisterSpi(new TradeSpi);
	tmpTrade->Init();
	tmpTrade->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
	tmpTrade->RegisterFront("protocol://222.73.119.230:7003");// 222.73.119.230:7003     

	//login
	cout<<"----login--"<<endl;
	Sleep(1000);
	CTShZdReqUserLoginField field;
	memset(&field, 0, sizeof(CTShZdReqUserLoginField));
	memcpy(field.UserID, "MN000301", 16);//f000202
	memcpy(field.Password, "888888", 41);
	tmpTrade->ReqUserLogin(&field, 300);
	
	//update the exchange collection code
	cout<<"----update--"<<endl;
	Sleep(1000);
	tmpTrade->GetTradingDay();
	CTShZdQryExchangeField pQryExchange;
	memset(&pQryExchange, 0, sizeof(CTShZdQryExchangeField));
	memcpy(pQryExchange.ExchangeID, "", 9);
	tmpTrade->ReqQryExchange(&pQryExchange, 8);


}

//update the instrument
void UpdateInstrument()
{
	//register
	cout<<"----register--"<<endl;
	CSHZdTraderApi* tmpTrade;
	tmpTrade = CSHZdTraderApi::CreateSHZdTraderApi("..\\tradeLog", false);
	tmpTrade->RegisterSpi(new TradeSpi);
	tmpTrade->Init();
	tmpTrade->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
	tmpTrade->RegisterFront("protocol://222.73.119.230:7003");// 222.73.119.230:7003     
	
	//login 
	cout<<"----login--"<<endl;
	Sleep(1000);
	CTShZdReqUserLoginField field;
	memset(&field, 0, sizeof(CTShZdReqUserLoginField));
	memcpy(field.UserID, "MN000301", 16);//f000202
	memcpy(field.Password, "888888", 41);
	tmpTrade->ReqUserLogin(&field, 300);
	//
	Sleep(1000);
	tmpTrade->GetTradingDay();
	//first time should not set time
	struct tm *now_date;
	time_t tt;
	tt = time(NULL);
	now_date = gmtime(&tt);
	char date_str[20];
	strftime(date_str, 20, "%Y%m%d", now_date);
	cout << "start time:--->" << date_str << endl;


	SqlHandle* sqlhandle = new SqlHandle();
	vector<string> exResult = sqlhandle->queryExchanges();
	vector<string>::iterator exIter = exResult.begin();
	for (; exIter != exResult.end(); exIter++) {
		//cout << "---->" << *exIter << endl;
		if ((*exIter).compare("SHFE")!=0) {
			int index = 1;
			CTShZdQryInstrumentField pQryInstrument;
			memset(&pQryInstrument, 0, sizeof(CTShZdQryInstrumentField));
			strcpy(pQryInstrument.ExchangeID, (*exIter).data());
			// if the first time,must note this line
			strcpy(pQryInstrument.InsertTimeStart, date_str);
			//strcpy(pQryInstrument.ExchangeID, "SHFE");

			memcpy(pQryInstrument.InsertTimeStart, "", 10);
			memcpy(pQryInstrument.ProductID, "", 9);
			pQryInstrument.Index = index * 500; //
			tmpTrade->ReqQryInstrument(&pQryInstrument, 9);
		}
	}
	delete sqlhandle;
}

//Sub market data
void SubMarketData()
{
	//get the submarket
	//regist login front
	cout << "-----regist front -----" << endl;
	apiHandle = CSHZdMarketApi::CreateSHZdMarketApi("..\\marketLog", false);
	apiHandle->RegisterSpi(new MarketSpi);
	apiHandle->Init();
	apiHandle->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
	apiHandle->RegisterLoginFront("protocol://120.132.7.159:7993");

	//login
	Sleep(1000);
	cout << "-----login  -------" << endl;
	CTShZdReqUserLoginField field;
	memset(&field, 0, sizeof(CTShZdReqUserLoginField));
	memcpy(field.UserID, "91000001", 16);
	memcpy(field.Password, "111111", 41);
	apiHandle->ReqUserLogin(&field, 1);

	//regist 
	cout << "-----regist ---------" << endl;
	apiHandle->RegisterFront("protocol://222.73.123.120:9001");

	//sub the market data
	Sleep(20000);
	cout << "-----start market data -----" << endl;
	apiHandle->GetTradingDay();
	//Sleep(10000);
	/*char *ppInstrumentID[4];
	string temp1 = "HKEX,HHI1702;HKEX,HHI1703;HKEX,HHI1704;HKEX,HHI1705;HKEX,HHI1706";
	string temp2 = "HKEX,HHI1707;HKEX,HHI1708;HKEX,HHI1709;HKEX,HHI1711";
	string temp3 = "HKEX,HSI1702;HKEX,HSI1703;HKEX,HSI1704;HKEX,HSI1705;HKEX,HSI1706";
	string temp4 = "HKEX,HSI1707;HKEX,HSI1708;HKEX,HSI1709;HKEX,HSI1710";
	ppInstrumentID[0] = (char*)temp1.c_str();
	ppInstrumentID[1] = (char*)temp2.c_str();
	ppInstrumentID[2] = (char*)temp3.c_str();
	ppInstrumentID[3] = (char*)temp4.c_str();
	apiHandle->SubscribeMarketData(ppInstrumentID, 4);*/

	//get the string data
	//repHandle = new ClientResponse();
	vector<string> result = repHandle->getSubMarket();
	int size = result.size();
	int i;
	cout << "result size---->" << size << endl;
	//	char **ppInstrumentID = new char*[size];
	char *tmp[1];
	for (i = 0; i < size; i++) {
		//ppInstrumentID[i] = new char[result[i].length()];
		//strcpy(ppInstrumentID[i],result[i].data());
		//cout<< ppInstrumentID[i]<<endl;
		//apiHandle->SubscribeMarketData(ppInstrumentID, 1);
		tmp[0] = new char[result[i].length()];
		strcpy(tmp[0], result[i].data());
		//cout<< tmp[0]<<endl;
		//cout<<"sub the i---> "<<i<<endl;
		apiHandle->SubscribeMarketData(tmp, 1);
	}
	//the Sub market

	//release memeory
	//delete[] ppInstrumentID;
	//ppInstrumentID = NULL;
}

//用于处理网页的请求
class PageRequestHandler : public HTTPRequestHandler
	/// Return a HTML document with some JavaScript creating
	/// a WebSocket connection.
{
public:
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/html");
		std::ostream& ostr = response.send();

		ostr << "<html>";
		ostr << "<head>";
		ostr << "<title>WebSocketServer</title>";
		ostr << "<script type=\"text/javascript\">";
		ostr << "function WebSocketTest()";
		ostr << "{";
		ostr << "  if (\"WebSocket\" in window)";
		ostr << "  {";
		ostr << "    var ws = new WebSocket(\"ws://127.0.0.1:8080?token=helloworld\");";
		//ostr << "    var ws = new WebSocket(\"ws://127.0.0.1:8080\",[\"token\",\"helloworld\"] );";
		//	ostr << "    var ws = new WebSocket(\"ws://" << request.serverAddress().toString() << "token=helloworld\");";
		ostr << "    ws.onopen = function()";
		ostr << "      {";
		ostr << "        ws.send(\"Hello, world!\");";
		ostr << "      };";
		ostr << "    ws.onmessage = function(evt)";
		ostr << "      { ";
		ostr << "        var msg = evt.data;";
		ostr << "        alert(\"Message received: \" + msg);";
		//        ostr << "        ws.close();";
		ostr << "      };";
		ostr << "    ws.onclose = function()";
		ostr << "      { ";
		ostr << "        alert(\"WebSocket closed.\");";
		ostr << "      };";
		ostr << "  }";
		ostr << "  else";
		ostr << "  {";
		ostr << "     alert(\"This browser does not support WebSockets.\");";
		ostr << "  }";
		ostr << "}";
		ostr << "</script>";
		ostr << "</head>";
		ostr << "<body>";
		ostr << "  <h1>WebSocket Server</h1>";
		ostr << "  <p><a href=\"javascript:WebSocketTest()\">Run WebSocket Script</a></p>";
		ostr << "</body>";
		ostr << "</html>";
	}
};

class WebSocketRequestHandler : public HTTPRequestHandler
	/// Handle a WebSocket connection.
{
public:
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		Application& app = Application::instance();
		try
		{
			//HTTPCredentials cred("user", "fuck");
			//request.setCredentials("user","fuck");
			//response.requireAuthentication("hello");
			//string str, auth;
			//request.getCredentials(str,auth);
			//cout << "str->" << str << "   auth--->" << auth << endl;
	/*		string token;
			request.hasToken("token", token);
			cout << token << endl;*/
		/*	if (request.hasToken("token", "helloworld")) {
				cout << "yes has token";
			}
			else {
				cout << "no token" << endl;
			}*/
			//提取token字段
			cout << "------------------------------------------" << endl;
			string str = request.getURI();
			cout << str << endl;

			vector<string> urlV;
			//SplitString(str, urlV, "=");
			urlV = repHandle->SplitString(str, "?=");
			string token="";
			for (vector<string>::size_type i = 0; i != urlV.size(); ++i)
			{
				cout << i << "     " << urlV[i] << endl;
				if (urlV[i] == "token") {
					token = urlV[i + 1];
					break;
				}
			}
			cout << "token--->" << token << endl;
			//	cout << str[i] << " ";
			WebSocket *ws = new WebSocket(request, response);

			//将连接加入到数组中 返回合约接口
	
			/*conWeb newWeb = { ws, "", "HET", 0, 'A', "", "", "ONE",exCon,0 };
			cliWebsocket.push_back(newWeb);
			cout << "first type-->" << newWeb.type << endl;*/
			app.logger().information("WebSocket connection established.");
/*
			Sleep(2000);
			string tmp_str = "return data";
			int flags;
			ws->sendFrame(tmp_str.data(),tmp_str.length(), flags);*/

			char buffer[1024];
			int flags;
			int n = 0;
			string type, exCode, conCode, kType,ret_str;
			char status = 'A';
			string revStr;
			vector<pair<string, string>> exCon;


			//ws->setReceiveTimeout(Poco::Timespan(0, 0, 0, 0, 0));
			do {
				//init thr parameters
				type = ""; exCode = ""; conCode = ""; kType = "";
				exCon.clear();	ret_str="";

				//添加定时器 day h m s mic
				if (ws->poll(Poco::Timespan(0, 0, 2, 0, 0), Poco::Net::WebSocket::SELECT_READ) == false) {
					//status = 'D';
					//ChangeStatus(ws, type, status, exCode, conCode, kType, exCon,flags);
					cout << "timeout" << endl;
					break;
				}
				else {

					n = ws->receiveFrame(buffer, sizeof(buffer), flags);
					if (n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE) {
						buffer[n] = '\0';
						std::cout << "Receive-->" << buffer <<  std::endl;
					//	std::cout << "flags--->"<<flags <<std::endl;

					//	JSON the request
						revStr = buffer;
						JSON::Parser parse;
						Dynamic::Var json = parse.parse(revStr);
						JSON::Object::Ptr pObj = json.extract<JSON::Object::Ptr>();
						type = pObj->get("type").toString();

						//get the request paramters
						if (type == "HET") {
							try{
								ret_str = repHandle->handleHET();
								ws->sendFrame(ret_str.data(),ret_str.length(), flags);
							}catch(Exception &exc){
								std::cerr << exc.displayText()<<endl;;
							}
						}else if(type=="CON"){  //return data no update
							ret_str=repHandle->handleCON();
							cout <<"--------->>"<< ret_str.length() << endl;
							try{
								//string ret_str = "This is MAR appication ";
								ws->sendFrame(ret_str.data(),ret_str.length(), flags);
							}catch(Exception &exc){
								std::cerr << exc.displayText()<<endl;;
							}
						}
						else if(type == "EMR"){
							JSON::Array::Ptr pArry = pObj->getArray("data");
							exCode = pArry->get(0).toString();
							
							ret_str=repHandle->handleEMR(exCode);
							cout <<"--------->>"<< ret_str.length() << endl;
							try{
								//string ret_str = "This is EMR appication ";
								ws->sendFrame(ret_str.data(),ret_str.length(), flags);
							}catch(Exception &exc){
								std::cerr << exc.displayText()<<endl;;
							}
						}
						else if (type == "MAR") {  	//market data
							exCon.clear();
							// 注意此处空间是否回收
							cout<<"exCon size--"<<exCon.size()<<endl;

							JSON::Array::Ptr pArry = pObj->getArray("data");
							int size = pArry->size();
							for (int i = 0; i < size; i++)
							{
								JSON::Array::Ptr mArry = pArry->getArray(i);
								exCon.push_back(make_pair(mArry->get(0).toString(),mArry->get(1).toString()));
							}
							ret_str=repHandle->handleMAR(exCon);
							cout <<"--------->>"<< ret_str.length() << endl;
							try{
								//string ret_str = "This is CON appication ";
								ws->sendFrame(ret_str.data(),ret_str.length(), flags);
							}catch(Exception &exc){
								std::cerr << exc.displayText()<<endl;;
							}
						}
						else if (type == "SUB") ////get sub intruments
						{
							exCon.clear();

							JSON::Array::Ptr pArry = pObj->getArray("data");
							int size = pArry->size();
							for (int i = 0; i < size; i++)
							{
								JSON::Array::Ptr mArry = pArry->getArray(i);
								exCon.push_back(make_pair(mArry->get(0).toString(),mArry->get(1).toString()));
							}
							ret_str = repHandle->handleSUB(exCon);
							cout <<"--------->>"<< ret_str.length() << endl;
							try{
								//string ret_str = "This is SUB appication ";
								ws->sendFrame(ret_str.data(),ret_str.length(), flags);
							}catch(Exception &exc){
								std::cerr << exc.displayText()<<endl;;
							}
						}
						else if (type == "KLE")//k line
						{
							JSON::Array::Ptr pArry = pObj->getArray("data");
							//int size = pArry->size();
							//if(size>0)
							//{
								//JSON::Array::Ptr mArry = pArry->getArray(0);
								exCode = pArry->get(0).toString();
								conCode = pArry->get(1).toString();
								kType = pArry->get(2).toString();
							//}
							ret_str=repHandle->handleKLN(exCode,conCode,kType);
							cout <<"--------->>"<< ret_str.length() << endl;
							try{
								//string ret_str = "This is KLE appication ";
								ws->sendFrame(ret_str.data(),ret_str.length(), flags);
							}catch(Exception &exc){
								std::cerr << exc.displayText()<<endl;;
							}
						}
						else {
							break;
						}
				
						//find and update the websocket parameters
						//cout<<"1 change flags---->"<<flags<<endl;
						//ChangeStatus(ws, type, status, exCode, conCode, kType, exCon,flags);
					
					}else{ //客户端申请退出
					//	status = 'D';
						//ChangeStatus(ws, type, status, exCode, conCode, kType, exCon,flags);
						//Sleep(10);
						app.logger().information("WebSocket connection closed.");
						//ws->shutdown();
					}
				//	Sleep(2000);
				}
			}while (n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
		}
		catch (WebSocketException& exc)
		{
			cout<<"exception "<<endl;
			app.logger().log(exc);
			switch (exc.code())
			{
			case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
				response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
				// fallthrough
			case WebSocket::WS_ERR_NO_HANDSHAKE:
			case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
			case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
				response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
				response.setContentLength(0);
				response.send();
				break;
			}
		}
	}
};

class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
		Application& app = Application::instance();

		app.logger().information("Request from "
			+ request.clientAddress().toString()
			+ ": "
			+ request.getMethod()
			+ " "
			+ request.getURI()
			+ " "
			+ request.getVersion());

		for (HTTPServerRequest::ConstIterator it = request.begin(); it != request.end(); ++it)
		{
			app.logger().information(it->first + ": " + it->second);
		}

		if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
			return new WebSocketRequestHandler;
		else
			return new PageRequestHandler;
	}
};

class WebSocketServer : public Poco::Util::ServerApplication
	/// The main application class.
	///
	/// This class handles command-line arguments and
	/// configuration files.
	/// Start the WebSocketServer executable with the help
	/// option (/help on Windows, --help on Unix) for
	/// the available command line options.
	///
	/// To use the sample configuration file (WebSocketServer.properties),
	/// copy the file to the directory where the WebSocketServer executable
	/// resides. If you start the debug version of the WebSocketServer
	/// (WebSocketServerd[.exe]), you must also create a copy of the configuration
	/// file named WebSocketServerd.properties. In the configuration file, you
	/// can specify the port on which the server is listening (default
	/// 9980) and the format of the date/time string sent back to the client.
	///
	/// To test the WebSocketServer you can use any web browser (http://localhost:9980/).
{
public:
	WebSocketServer() : _helpRequested(false)
	{
	}

	~WebSocketServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false));
	}

	//参数的处理
	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("A sample HTTP server supporting the WebSocket protocol.");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		if (_helpRequested)
		{
			displayHelp();
		}
		else
		{
			// get parameters from configuration file
			unsigned short port = (unsigned short)config().getInt("WebSocketServer.port", 8080);

			// set-up a server socket
			ServerSocket svs(port);
			// set-up a HTTPServer instance
			HTTPServer srv(new RequestHandlerFactory, svs, new HTTPServerParams);
			// start the HTTPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the HTTPServer
			srv.stop();
		}
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};

void periodicallyUpdateKline()
{
	SqlHandle sqlhandle;
	Timer timer(1000, 1000);
	timer.start(TimerCallback<SqlHandle>(sqlhandle, &SqlHandle::updateKline));
}

int main(int argc, char* argv[])
{
	repHandle = new ClientResponse();
	//testFutur();
	
	//update exchange
	//UpdateExchange();

	//update intruments
	//UpdateInstrument();

	//the sub market
	SubMarketData();

	//cannot put in function
	//periodicallyUpdateKline();
	
	SqlHandle sqlhandle;
	time_t now = time(0);
	tm *ltm = localtime(&now);
	Timer timer((60-ltm->tm_sec)*1000, 60000);
	//Timer timer(100, 60000);
	timer.start(TimerCallback<SqlHandle>(sqlhandle, &SqlHandle::updateKline));

	/*vector<JSON::Object> nodes = sqlhandle.queryKLE("ONE", "HHI1711", 1510145781, 1510145901);
	for (int i=0; i < nodes.size(); i ++) {
		cout << nodes[i].getValue<double>("OpenPrice") << " " <<
				nodes[i].getValue<double>("ClosePrice") << " " <<
				nodes[i].getValue<double>("HighestPrice") << " " << 
				nodes[i].getValue<double>("LowestPrice") << " " << 
				nodes[i].getValue<double>("PriceChange") << " " << 
				nodes[i].getValue<double>("PriceChangeRatio") << " " << 
				nodes[i].getValue<double>("TradingVolume") << " " << endl;
	}*/
	//Thread::sleep(100000);
	/*timer.stop();*/

	
	//UpdateInstrument();

	//SqlHandle sqlhandle;
	//time_t now = time(0);
	//tm *ltm = localtime(&now);
	//Timer timer((60-ltm->tm_sec)*1000, 60000);
	//timer.start(TimerCallback<SqlHandle>(sqlhandle, &SqlHandle::updateKline));

	//test CON
	//vector<pair<string,string>> tmp;
	//tmp.push_back(make_pair("KRX","201MC302"));
	//tmp.push_back(make_pair("KRX","301N3295"));
	//tmp.push_back(make_pair("KRX","301N3310"));
	//repHandle->handleCON(tmp);

	//test EMR
	/*string exCode="KRX";
	repHandle->handleEMR(exCode);
*/

	//test SUB
	/*vector<pair<string,string>> tmp;
	tmp.push_back(make_pair("KRX","201MC302"));
	tmp.push_back(make_pair("KRX","301N3295"));
	tmp.push_back(make_pair("KRX","301N3310"));
	repHandle->handleSUB(tmp);*/

	//test Kline

	string ret_str=repHandle->handleKLN("BMD","FKLI1806","ONE");
	cout<<"--------------------kData----------------"<<endl;


	//创建推送线程
	//CreateThread(NULL, 0, handleRequest, NULL, 0, NULL);

	/*WebSocketServer app;
	return app.run(argc, argv);*/

	/*string str=repHandle->handleMAR();
	ofstream fout;
	fout.open("MAR.txt",'w');
	
	fout<<str<<endl;
	fout.close();*/

	//json test
	//   JsonGet();
	//    JsonArry();
	//    JsonGetArry();

	delete repHandle;
	system("PAUSE");
	return 0;
}

//change the client status
//void ChangeStatus(WebSocket* conWS, string type,  char status, string exCode, string conCode, string kType,vector<pair<string,string>> exCon,int flags)
//{
//	int i;
//	int size = cliWebsocket.size();
//	for (i = 0; i < size; i++)
//	{
//		if (cliWebsocket[i].conWS == conWS) {
//			cliWebsocket[i].status = status;
//			//client is alive 
//			if (status == 'A') {
//				cliWebsocket[i].type = type;
//				cout<<"2 change flags--->"<<flags<<endl;
//				cliWebsocket[i].flags=flags;
//
//				if (!exCode.empty()) {
//					cliWebsocket[i].exCode = exCode;
//				}
//				if (!conCode.empty()) {
//					cliWebsocket[i].conCode = conCode;
//				}
//				if (!kType.empty()) {
//					cliWebsocket[i].kType = kType;
//				}
//				if (exCon.size() > 0) { //vector pair
//					cliWebsocket[i].exCon=exCon;
//				}
//			}// erase the websocket  dead
//			/*else {
//				cout << "delete client" << endl;
//				cliWebsocket.erase(cliWebsocket.begin()+i);
//			}*/
//			break;
//		}
//		else { continue; }
//	}
//}

//handle all request and put latest market data
DWORD WINAPI handleRequest(LPVOID lpparentet)
{
	int i, size,flags;
	string type, exCode, conCode, kType;
	while (true)
	{
	//	cout << "come here thread 2 put new data" << endl;
		size = cliWebsocket.size();
		//cout<<"come here thread 2 put new data  size---->"<<size<<endl;
		for (i = 0; i < size; i++)
		{
			char status = cliWebsocket[i].status;
			if(status=='A'){
				type = cliWebsocket[i].type;
				cout << "websocket[" << i << "] : request type is " << type << endl;
				//then get the request from mongoDB and send to client
				string ret_str="return data   \n";
				flags = cliWebsocket[i].flags;
				if(flags==0){
					cout<<"can not send data---> "<<flags<<endl;
				}else{
					cout<<"send-----"<<endl;
					try{
						cliWebsocket[i].conWS->sendFrame(ret_str.data(),ret_str.length(), flags);
					}
					catch(Exception &exc){
						std::cerr << exc.displayText()<<endl;
					}
				}
			}else{
				cliWebsocket.erase(cliWebsocket.begin()+i);
			}

		}
		//Sleep(1000);
	}
	return 0;
}

