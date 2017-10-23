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
using namespace std;

class ClientResponse
{
public:
	ClientResponse();
	~ClientResponse();
	
	//单个切分
	void SplitSingle(const string& s, vector<string>& v, const string& c);

	//多个分隔符切分字符串
	vector<string> SplitString(const string &s, const string &seperator);
};





#endif /* ClientResponse_h */










