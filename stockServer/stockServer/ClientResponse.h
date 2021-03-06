//
//  ClientResponse.h
//  webServer
//
//  Created by gongkun on 2017/10/12.
//  Copyright © 2017 gongkun. All rights reserved.
/*
  * client operation
*/

#ifndef ClientResponse_h
#define ClientResponse_h

#include <iostream> 
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "SqlHandle.h"

//json
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

using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using namespace std;

class ClientResponse
{
public:
	SqlHandle* sqlhandle;

	ClientResponse();
	~ClientResponse();
	
	//单个切分
	void SplitSingle(const string& s, vector<string>& v, const string& c);

	//多个分隔符切分字符串
	vector<string> SplitString(const string &s, const string &seperator);

	//HET nothing
	string handleHET();

	//CON request get the excode product instrument
	string handleCON();

	//MAR request(exCode conCode)
	string handleMAR(vector<pair<string, string>> exCon);

	//EMR handle (exCode)
	string handleEMR(string exCode);

	//SUB handle (excode conCode)
	string handleSUB(vector<pair<string, string>> exCon);

	//K line request
	string handleKLN(string exCode, string conCode, string kType);

	//judge the data change
	bool judgeData(string oldData, string newData);
	//get the time (second)
	int getTtime(string kType);

	//get the sub market data
	vector<string>getSubMarket();
};

//连接的webSocket结构 一旦遇到dead  
typedef struct conWebsocket
{
	WebSocket* conWS;   //websocket
	string oldData;    //old data(use to judge the data change)
	string type;	   //request type
	time_t forTime;    //front send time
	char status;		//A: alive D:dead 
	string exCode;      //exchange code
	string conCode;		// contract code
	string kType;		//kType
	vector<pair<string, string>> exCon;	//exCode and instrument pair
	int flags;		//the send data flags

	/*conWebsocket(WebSocket* conWS,string oldData,string type,time_t forTime,char status,string exCode,string conCode,string kType,vector<pair<string,string>>exCon)
	{
		conWS = conWS;	kType = kType;
		oldData = oldData;	type = type;
		forTime = forTime;	status = status;
		exCode = exCode;	conCode = conCode;
		exCon = exCon;
	}*/
}conWeb;


#endif /* ClientResponse_h */










