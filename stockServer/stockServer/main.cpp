

#include "ApiHandle.h"
#include "ClientResponse.h"
#include <iostream>
#include <fstream>
#include <string>    //iostream
#include <stdio.h>
#include <cstdio>
#include <vector>
//#include <Windows.h>
//
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
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Format.h"

//json
#include <stdint.h>
#include <sstream>
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

CSHZdMarketApi *apiHandle;
CSHZdTraderApi* apiTrade;
ClientResponse *repHandle;

vector<conWeb> cliWebsocket;	//用于维护连接的队列 否则报错
void changeStatus(WebSocket* conWS, string type, char status, string exCode, string conCode, string kType);
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
				apiHandle->ReqUserLogin(&field, 1); \
					cout << "login : yes" << endl;

				//Sleep(2000);
				////ofstream fout;
				////fout.open("output.txt");
				//string strday(apiHandle->GetTradingDay());
				//cout << "trade day--->" << strday << endl;
				//
				//fout << strday << "\n";
				//fout.close();

				apiHandle->RegisterFront("protocol://222.73.123.120:9001");
				//apiHandle->RegisterFront("protocol://222.73.105.170:9002");

				//apiHandle->RegisterLoginFront("protocol://222.73.123.120:9001");
				//cout << "register towice" << endl;

				char *ppInstrumentID[4];
				string temp1 = "HKEX,HHI1702;HKEX,HHI1703;HKEX,HHI1704;HKEX,HHI1705;HKEX,HHI1706";
				string temp2 = "HKEX,HHI1707;HKEX,HHI1708;HKEX,HHI1709;HKEX,HHI1710";
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
			else if (selectchar.compare("f") == 0)
			{
				apiHandle->GetTradingDay();
				char *ppInstrumentID[4];
				string temp1 = "HKEX,HHI1702;HKEX,HHI1703;HKEX,HHI1704;HKEX,HHI1705;HKEX,HHI1706";
				string temp2 = "HKEX,HHI1707;HKEX,HHI1708;HKEX,HHI1709;HKEX,HHI1710";
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
				apiTrade->ReqUserLogin(&field, 100);
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
void JsonGet()
{
	string jsonString = "[1,2,3]";
	JSON::Parser parser;
	Dynamic::Var result;
	parser.reset();

	result = parser.parse(jsonString);
	JSON::Object::Ptr pObj = result.extract<JSON::Object::Ptr>();
	//Dynamic::Var ret = pObj->get("name1");
	Dynamic::Var ret = pObj->begin();
	if (ret.isEmpty())
	{
		std::cout << "is null" << std::endl;
	}
	else
	{
		std::cout << ret.toString() << std::endl;
	}
}

//在　json 里加入数组。
std::string JsonArry(void)
{
	JSON::Object jsnObj;
	JSON::Array jsnArry;
	JSON::Object subObj1;
	JSON::Object subObj2;

	//jsnObj.set("command", "createuser");

	subObj1.set("name", "yuhaiyang");
	subObj1.set("pass", "123");

	subObj2.set("name", "cj");
	subObj2.set("pass", "456");

//	jsnArry.add(subObj1);
	//jsnArry.add(subObj2);
	Dynamic::Var ret = "first";
	Dynamic::Var ret1 = "second";
	Dynamic::Var ret2 = "third";
	jsnArry.add(ret);
	jsnArry.add(ret1);
	jsnArry.add(ret2);

	

	jsnObj.set("data", jsnArry);
	std::stringstream  jsnString;
	jsnObj.stringify(jsnString, 3);
	std::cout << jsnString.str() << std::endl;

	string testStr = "{\"data\":[\"A\",\"B\",\"C\",\"D\"]}";

	JSON::Parser parse;
	Dynamic::Var json = parse.parse(testStr);
	JSON::Object::Ptr pObj = json.extract<JSON::Object::Ptr>();
	JSON::Array::Ptr pArry = pObj->getArray("data");

	int size = pArry->size();
	for (int i = 0; i < size; i++)
	{
		cout <<i<<"---->" << pArry->get(i).toString()<< endl;
	}

	//JSON::Array::ConstIterator it = pArry->begin();
	////把数组里的所有内容打印出来
	////当然也可以把每个对象拿出来用。
	//for (; it != pArry->end(); it++)
	//{
	//	std::cout << it->toString() << std::endl;
	//}

	return jsnString.str();

}


//解析数据
void JsonGetArry(void)
{
	/*-------------------构造包含Array的JSON（string类型）--------------------------------*/
	JSON::Object jsnObj;
	JSON::Array jsnArry;
	JSON::Object subObj1;
	JSON::Object subObj2;

	jsnObj.set("command", "createuser");

	subObj1.set("name", "yuhaiyang");
	subObj1.set("pass", "123");

	subObj2.set("name", "cj");
	subObj2.set("pass", "456");

	jsnArry.add(subObj1);
	jsnArry.add(subObj2);

	jsnObj.set("userinfo", jsnArry);

	std::stringstream  jsnOstream;
	jsnObj.stringify(jsnOstream, 3);

	string jsnStr = jsnOstream.str();

	std::cout << "原数据:\n" << jsnStr << "\n\n\n" << std::endl;


	//--------------------------解析数组-----------------------------
	JSON::Parser parse;
	Dynamic::Var json = parse.parse(jsnStr);
	JSON::Object::Ptr pObj = json.extract<JSON::Object::Ptr>();
	JSON::Array::Ptr pArry = pObj->getArray("userinfo");

	JSON::Array::ConstIterator it = pArry->begin();
	//把数组里的所有内容打印出来
	//当然也可以把每个对象拿出来用。
	for (; it != pArry->end(); it++)
	{
		std::cout << it->toString() << std::endl;
	}

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
			string token = "";
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

			WebSocket *ws  = new WebSocket(request, response);

			//vector<struct MM* > *ve = new vector<struct MM* >[12];
			//MM *bb;
			//ve->push_back(bb);


			//将连接加入到数组中 返回合约接口
			//time_t timep;
			//time(&timep);
			conWeb newWeb(ws,"","CON",0,'A',"","","ONE");
			/*newWeb.conWS = ws;
			newWeb.oldData = "NIL";
			newWeb.status = 'A';
			newWeb.type = "NIL";
			newWeb.exTrade = "NIL";
			newWeb.conCode = "NIL";
			time_t timep;
			time(&timep);
			newWeb.forTime = timep;*/
		
			cliWebsocket.push_back(newWeb);



			app.logger().information("WebSocket connection established.");

			char buffer[1024];
			int flags;
			int n=0;

			
			//ws->setReceiveTimeout(Poco::Timespan(0, 0, 0, 0, 0));
			while (true) {
				//添加定时器 day h m s mic
				if(ws->poll(Poco::Timespan(0, 0, 0, 30, 0), Poco::Net::WebSocket::SELECT_READ) == false) {
					cout << "timeout" << endl;
					break;
				}
				else {
					n = ws->receiveFrame(buffer, sizeof(buffer), flags);
					if (n > 0) {
						string rev = buffer; 
						string param[10];
						string type,exCode,conCode,kType;
						//获取请求的类型 [type,exCode,conCode,kType] 下面切分出参数
						
						type = "HET";
						//心跳
						if (type == "HET") {
							continue;
						}//contract
						else if (type == "CON") {
							changeStatus(ws, type, 'A', "", "", "");
							continue;
						}//market data
						else if (type == "MAR") {
							//exCode=parma[1];
							exCode = "ALL";
							changeStatus(ws, type, 'A', "ALL", "", "");
						}//k line
						else if(type=="KLN")
						{
							//exCode=parma[1]; conCode=parma[2];kType=parma[3];
							changeStatus(ws, type, 'A', exCode, conCode, kType);
						}
						else {
							break;
						}

						buffer[n] = '\0';
						std::cout << "Receive: " << buffer << std::endl;

					}
					Sleep(2000);
				}
			}


			//char buffer[1024];
			//int flags;
			//int n;
			//do
			//{
			//	n = ws.receiveFrame(buffer, sizeof(buffer), flags);
			//	buffer[n] = '\0';
			//	//                app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));
			//	std::cout << std::endl;
			//	std::cout << "Recive: " << buffer << std::endl;

			//	n = ws.receiveFrame(buffer, sizeof(buffer), flags);
			//	buffer[n] = '\0';
			//	std::cout << std::endl;
			//	std::cout << "Recive: " << buffer << std::endl;


			//	//发送消息更改
			//	std::string replay;
			//	//                replay="I love you";
			//	replay = JsonArry();

			//	ws.sendFrame(replay.data(), (int)replay.size(), WebSocket::FRAME_TEXT);
			//} while (n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);

			app.logger().information("WebSocket connection closed.");
			ws->shutdown();
		}
		catch (WebSocketException& exc)
		{
			cout << "exception" << endl;
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



int main(int argc, char* argv[])
{
	//发送的线程
	//CreateThread(NULL, 0, handleRequest, NULL, 0, NULL);

	//testFutur();
	//WebSocketServer app;
	//return app.run(argc, argv);
	//json test
	 //   JsonGet();
	    JsonArry();
	//    JsonGetArry();
	//    
		/*time_t timep;
		time(&timep);
		cout << "time: " << timep << endl;*/
	system("PAUSE");
	return 0;
}

//change status
void changeStatus(WebSocket* conWS, string type, time_t forTime, char status, string exCode, string conCode, string kType)
{
	int i;
	int size = cliWebsocket.size();
	for (i = 0; i < size; i++)
	{
		if (cliWebsocket[i].conWS == conWS) {
			cliWebsocket[i].type = type;
			cliWebsocket[i].status = status;
			if (!exCode.empty()) {
				cliWebsocket[i].exCode = exCode;
			}
			if (!conCode.empty()) {
				cliWebsocket[i].conCode = conCode;
			}
			if (!kType.empty()) {
				cliWebsocket[i].kType = kType;
			}
			break;
		}
		else { continue; }
	}
}

//用于处理所有的请求
DWORD WINAPI handleRequest(LPVOID lpparentet)
{
	int i;
	vector<conWeb>::iterator iter;
	string type, exCode, conCode, kType,jsonResult,oldData;
	char status;
	time_t oldTime,newTime;
	repHandle = new ClientResponse();
	while (true)
	{
		if (cliWebsocket.size() > 0) //hava element
		{
			for (iter = cliWebsocket.begin(); iter != cliWebsocket.end(); ) {
				status = (*iter).status;
				oldData = (*iter).oldData;
				if (status == 'A') {
					type = (*iter).type;

					if (type == "CON") {
						jsonResult = repHandle->handleCON();
						if (repHandle->judgeData(oldData, jsonResult)) {
							//send to client
						}
						continue;
					}
					else if (type == "MAR") {
						exCode = (*iter).exCode;
						jsonResult = repHandle->handleMAR(exCode);
						if (repHandle->judgeData(oldData, jsonResult)) {
							//send to client
						}
						continue;
					}
					else if (type == "KLN") {
						oldTime = (*iter).forTime;
						exCode = (*iter).exCode;
						conCode = (*iter).conCode;
						kType = (*iter).kType;
						time(&newTime);
						int val = repHandle->getTtime(kType);
						if ((newTime - oldTime) > val) {
							jsonResult = repHandle->handleKLN(exCode, conCode, kType);
								//send to client
						}
						continue;
					}
					else {
						continue;
					}

					++iter;
				}//心跳停止
				else {
					iter = cliWebsocket.erase(iter);
				}

			}
		}
		else {
			cout << "no client connect" << endl;
			continue;
		}
	//	Sleep(5000);
	}
	return 0;
}
