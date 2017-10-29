//
//  ClientResponse.cpp
//  webServer
//
//  Created by gongku on 2017/10/12.
//  Copyright © 2017 gongku. All rights reserved.
//

#include <stdio.h>
#include "ClientResponse.h"

ClientResponse::ClientResponse() {

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

//CON request json data
string ClientResponse::handleCON()
{
	string result = "this is CON request";
	return result;
}

//MAR request json data
string ClientResponse::handleMAR(string exCode) {
	string result = "this is MAR request";
	return result;
}

//K line request json data
string ClientResponse::handleKLN(string exCode, string conCode, string kType)
{
	string result = "this is k line request";
	return result;
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

int ClientResponse::getTtime(string kType)
{
	int result = 0;
	if (kType == "ONE") {
		result = 60;
	}
	else if (kType == "THD") {
		result = 180;
	}
	else if (kType == "FIV") {
		result = 300;
	}
	else if (kType == "TEN") {
		result = 600;
	}
	else if (kType == "HAF") {
		result = 1800;
	}
	else if (kType == "SIT") {
		result = 3600;
	}
	else if (kType == "FOH") {
		result = 14400;
	}
	else if (kType == "DAY") {
		result = 86400;
	}
	else {
		result = 0;
	}

	return result;
}


ClientResponse::~ClientResponse()
{
}































