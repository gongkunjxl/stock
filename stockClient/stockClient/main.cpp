#include <iostream>
#include <fstream>

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/HTTPCredentials.h>
#include <Poco/Exception.h>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPCredentials;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Exception;
using namespace std;

//create a thread to use receive data
WebSocket *webs;
DWORD WINAPI handleRequest(LPVOID lpparentet);   //用于receive

int main() {
	webs = NULL;
	CreateThread(NULL, 0, handleRequest, NULL, 0, NULL);

    char buffer[1024];
    int flags;
    int n;
    std::string payload;
    
    try {
		cout << "client server" << endl;
        HTTPClientSession cs("127.0.0.1", 8080);
		HTTPRequest request(HTTPRequest::HTTP_GET, "token=helloworld", "HTTP/1.1");
        HTTPResponse response;
        std::string cmd;

		//验证
	//	HTTPCredentials cred("user", "fuck");
	//	request.setCredentials("token", "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

        WebSocket * ws = new WebSocket(cs, request, response); // Causes the timeout
		webs = ws;
		string req_str = "{\
				\"type\":\"MAR\",\
				\"data\":[]	\
			}";
        
        while (cmd != "exit") {
            cmd = "";
            cout << "Please input[exit]:";
            std::cin >> cmd;
            ws->sendFrame(req_str.data(), (int)req_str.size(), WebSocket::FRAME_TEXT);
        }
        ws->shutdown();

      } catch (Exception ex) {
		cout << "error client" << endl;
        cout << ex.displayText() << endl;
        cout << ex.what() << endl;
        return -1;
    }
	system("pause");
	return 0;
}
//用于处理receive
DWORD WINAPI handleRequest(LPVOID lpparentet)
{
	ofstream fout;
	fout.open("receive.txt",'w');
	int n=1;
	char *buffer=(char*)malloc(sizeof(char)*1024*1024);
	int flags=0;
	while(true)
	{
		//cout<<"wait the return data"<<endl;
		if (webs != NULL) {
			if (webs->poll(Poco::Timespan(0, 0, 3, 0, 0), Poco::Net::WebSocket::SELECT_READ) == false) {
				cout << "timeout" << endl;
				break;
			}
			try{
				n = webs->receiveFrame(buffer,1024*1024, flags);
				if(n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE)
				{
					buffer[n] = '\0';
				//	fout<<buffer<<endl;
				}else{
					webs=NULL;
					cout<<"close the client"<<endl;
					break;
				}
			}catch(Exception &exc)
			{
				std::cerr << exc.displayText()<<endl;
			}
			//cout << "Received: " << buffer << endl;
		}
		else {
			//cout << "no connections" << endl;
		}
	//	Sleep(500);
	}
	fout.close();
	free(buffer);
	return 0;
}
















