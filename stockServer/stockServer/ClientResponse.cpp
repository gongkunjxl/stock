//
//  ClientResponse.cpp
//  webServer
//
//  Created by gongku on 2017/10/12.
//  Copyright © 2017 gongku. All rights reserved.
//

#include <stdio.h>
#include <fstream>
#include "global.h"
#include "ClientResponse.h"

ClientResponse::ClientResponse() {
	sqlhandle = new SqlHandle();
}

//单个切分
//字符串切分
void ClientResponse::SplitSingle(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//多分隔符的字符串切分
vector<string> ClientResponse::SplitString(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}
		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

//HET nothing to do
string ClientResponse::handleHET()
{
	JSON::Object  result;
	result.set("status",STATUS_SUCCESS);
	result.set("errorCode",REQ_SUCCESS);
	result.set("errorMsg","NO Error");

	std::stringstream  jsnString;
	result.stringify(jsnString, 0);

	return jsnString.str();
}

//CON request json data(excode instrument product)
string ClientResponse::handleCON()
{
	//cout<<"handle CON request"<<endl;
	vector<string> exResult = sqlhandle->queryExchanges();
	vector<string>::iterator exIter = exResult.begin();
	vector<string> proResult;
	vector<string>::iterator proIter;
	struct tm *now_date;
	time_t tt;
	tt = time(NULL);
	now_date = gmtime(&tt);
	char date_str[20];
	strftime(date_str, 20, "%Y%m%d", now_date);
	//cout << "start time:--->" << date_str << endl;

	//json
	JSON::Object proObj;
	JSON::Object exObj;
	JSON::Object result;
	JSON::Array proArr;

	for (; exIter != exResult.end(); exIter++) {
	//	cout << "--------------------  " << *exIter << "  ---------------------------" << endl;
		proResult = sqlhandle->queryProduct((*exIter).data());
		if (proResult.size() > 0) {
			proIter = proResult.begin();
			for (; proIter != proResult.end(); proIter++) {
				//cout <<*exIter<< "***************" << *proIter << "*****************"<<endl;
				//	product obj
				proArr = sqlhandle->queryInsts((*exIter).data(), (*proIter).data(), date_str);
				proObj.set(*proIter, proArr);
			}
			//exchange obj
			exObj.set(*exIter, proObj);
		}
	}

	result.set("data", exObj);
	result.set("status",STATUS_SUCCESS);
	result.set("errorCode",REQ_SUCCESS);
	result.set("errorMsg","NO Error");
	//print
	//std::ofstream fout;
	//fout.open("MAR.txt", ios::app);
	std::stringstream  jsnString;
	result.stringify(jsnString, 0);

	//fout.close();
	return jsnString.str();
}

//EMR request json data
string ClientResponse::handleEMR(string exCode)
{
	//cout<<"handle EMR request data"<<endl;
	string conCode;
	struct tm *now_date;
	time_t tt;
	tt = time(NULL);
	now_date = gmtime(&tt);
	char date_str[20];
	strftime(date_str, 20, "%Y%m%d", now_date);
	//cout << "start time:--->" << date_str << endl;
//	cout << "exResultsize--->" << exResult.size() << endl;
	
	//json
	JSON::Array proArr;
	JSON::Array mar,mar_tmp;
	JSON::Object result;
				
	proArr = sqlhandle->queryInsts(exCode.data(), "", date_str);
	//解析json array
	int size = proArr.size();
	for (int i = 1; i < size; i++) {
		conCode=proArr.get(i).toString();
		//get the market data
		mar_tmp=sqlhandle->queryMarketEMR(exCode.data(),conCode.data());
		mar.add(mar_tmp);
		//cout <<"exchangeID"<< exCode << "   instrumentID----->" << proArr.get(i).toString() << endl;
	}
	result.set("data",mar);
	result.set("status",STATUS_SUCCESS);
	result.set("errorCode",REQ_SUCCESS);
	result.set("errorMsg","NO Error");

	std::stringstream  jsnString;
	result.stringify(jsnString, 0);

	return jsnString.str();
}

//MAR request json data
string ClientResponse::handleMAR(vector<pair<string, string>> exCon) {
	
	//cout<<"handle MAR request"<<endl;
	string exCode,conCode;
	int size=exCon.size();
	int i;

//	//json
	JSON::Object result;
	JSON::Array proArr;
	JSON::Array tmp;
	for(i=0;i<size;i++)
	{
		if((exCon[i].first.length()>0) && (exCon[i].second.length()>0) ){
			exCode=exCon[i].first;
			conCode=exCon[i].second;

			//get the data
			tmp=sqlhandle->queryMarket(exCode.data(),conCode.data());
			proArr.add(tmp);
		}
	}
	result.set("data",proArr);
	result.set("status",STATUS_SUCCESS);
	result.set("errorCode",REQ_SUCCESS);
	result.set("errorMsg","NO Error");

	std::stringstream  jsnString;
	result.stringify(jsnString, 0);
	//std::cout << jsnString.str() << std::endl;

	return jsnString.str();
}

//SUB request json data
string ClientResponse::handleSUB(vector<pair<string, string>> exCon) {
	
	//cout<<"handle SUB request"<<endl;
	string exCode,conCode;
	int size=exCon.size();
	int i;

//	//json
	JSON::Object result;
	JSON::Array proArr;
	JSON::Array tmp;
	for(i=0;i<size;i++)
	{
		if((exCon[i].first.length()>0) && (exCon[i].second.length()>0) ){
			exCode=exCon[i].first;
			conCode=exCon[i].second;
			//get the data
			tmp=sqlhandle->queryMarketEMR(exCode.data(),conCode.data());
			proArr.add(tmp);
		}
	}
	result.set("data",proArr);
	result.set("status",STATUS_SUCCESS);
	result.set("errorCode",REQ_SUCCESS);
	result.set("errorMsg","NO Error");

	std::stringstream  jsnString;
	result.stringify(jsnString, 0);
//	std::cout << jsnString.str() << std::endl;
	return jsnString.str();
}

//K line request json data
string ClientResponse::handleKLN(string exCode, string conCode, string kType)
{
	//cout << "handle k line request" << endl;
	//cout << conCode << endl;
	time_t endTime = time(0);
	time_t begTime = endTime - getTtime(kType)*300;
	//cout << "time: " << begTime << " " << endTime << endl;
	vector<JSON::Object> res = sqlhandle->queryKLE(kType, conCode, begTime, endTime);
	JSON::Object result;
	JSON::Array klnArray;
	cout << "queryKLE: " << res.size() << endl;
	for (int i=0; i < res.size(); i ++) {
		klnArray.add(res[i]);
	//	cout << res[i].getValue<TShZdPriceType>("OpenPrice") << endl;
	}
	result.set("data", klnArray);
	result.set("status",STATUS_SUCCESS);
	result.set("errorCode",REQ_SUCCESS);
	result.set("errorMsg","NO Error");

	std::stringstream  jsnString;
	result.stringify(jsnString, 0);	
	//std::cout << jsnString.str() << std::endl;
	return jsnString.str();
}

//judge the data changes
bool ClientResponse::judgeData(string oldData, string newData) 
{
	if (oldData == newData) {
		return false;
	}
	else {
		return true;
	}
}
//get time 
int ClientResponse::getTtime(string kType)
{
	int result = 0;
	if (kType.compare("ONE")==0) {
		result = 60;
	}
	else if (kType.compare("THR")==0) {
		result = 180;
	}
	else if (kType.compare("FIV")==0) {
		result = 300;
	}
	else if (kType.compare("TEN")==0) {
		result = 600;
	}
	else if( kType.compare("HAF")==0) {
		result = 1800;
	}
	else if (kType.compare("SIT")==0) {
		result = 3600;
	}
	else if (kType.compare("FOH")==0) {
		result = 14400;
	}
	else if (kType.compare("DAY")==0) {
		result = 86400;
	}
	else if (kType.compare("WEK")==0) {
		result = 604800;
	}
	else if (kType.compare("MON")==0) {
		result = 2592000;
	}
	else {
		result = 0;
	}

	return result;
}

//get the sub market data format  (exchage-->product-->istrument then sub format)
vector<string>ClientResponse::getSubMarket()
{
	vector<string> result;
	vector<string> exResult = sqlhandle->queryExchanges();
	vector<string>::iterator exIter = exResult.begin();
	vector<string> proResult;
	vector<string>::iterator proIter;
	struct tm *now_date;
	time_t tt;
	tt = time(NULL);
	now_date = gmtime(&tt);
	char date_str[20];
	strftime(date_str, 20, "%Y%m%d", now_date);
	//cout << "start time:--->" << date_str << endl;
//	cout << "exResultsize--->" << exResult.size() << endl;
	//json
	JSON::Array proArr;
	string sub_str;
	for (; exIter != exResult.end(); exIter++) {
	//	cout << "--------------------  " << *exIter << "  ---------------------------" << endl;
	/*	if ((*exIter).compare("NYBOY") == 0 || (*exIter).compare("eCBOT") == 0)
			continue;*/

		proResult = sqlhandle->queryProduct((*exIter).data());
		if (proResult.size() > 0) {
			sub_str = "";
			proIter = proResult.begin();
			for (; proIter != proResult.end(); proIter++) {
				//cout <<*exIter<< "***************" << *proIter << "*****************"<<endl;
				//	product obj
				proArr = sqlhandle->queryInsts((*exIter).data(), (*proIter).data(), date_str);
				//解析json array
				int size = proArr.size();
				for (int i = 1; i < size; i++) {
					sub_str = sub_str+(*exIter) + "," + proArr.get(i).toString() + ";";
					//cout << *exIter << "----" << *proIter << "----->" << proArr.get(i).toString() << endl;
				}
			}
			//delete the last ;
			int len = sub_str.length();
			if (len > 0) {
				sub_str = sub_str.substr(0, len - 1);
				result.push_back(sub_str);
				//cout << sub_str << endl;
			}
		}
	}
	return result;
}


ClientResponse::~ClientResponse()
{
	delete sqlhandle;
}































