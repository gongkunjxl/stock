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
	//CreateThread(NULL, 0, handleRequest, NULL, 0, NULL);
	ofstream fout;
	fout.open("receive.txt",'w');

    char *buffer=(char*)malloc(sizeof(char)*1024*1024);
    int flags;
    int n;
    std::string payload;
    
    try {
		cout << "client server" << endl;
        HTTPClientSession cs("101.5.213.227", 8080);
		//HTTPClientSession cs("127.0.0.1", 8080);
		HTTPRequest request(HTTPRequest::HTTP_GET, "token=helloworld", "HTTP/1.1");
        HTTPResponse response;
        std::string cmd;

		//验证
	//	HTTPCredentials cred("user", "fuck");
	//	request.setCredentials("token", "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

        WebSocket * ws = new WebSocket(cs, request, response); // Causes the timeout
		webs = ws;
		//分别是1:HET 2:CON 3:EMR 4:MAR 5:KLE 6:SUB
		string het_str = "{\
				\"type\":\"HET\",\
				\"data\":[]	\
			}";
		string con_str = "{\
				\"type\":\"CON\",\
				\"data\":[]	\
			}";
        
		string emr_str = "{\
				\"type\":\"EMR\",\
				\"data\":[\"KRX\"]	\
			}";
		string mar_str = "{\
				\"type\":\"MAR\",\
				\"data\":[[\"KRX\",\"201MC302\"],[\"KRX\",\"301N3295\"],[\"KRX\",\"301N3310\"]]	\
			}";
		string kle_str = "{\
				\"type\":\"KLE\",\
				\"data\":[\"KRX\",\"201MC302\",\"ONE\"]	\
			}";
		string sub_str = "{\
				\"type\":\"SUB\",\
				\"data\":[[\"KRX\",\"201MC302\"],[\"KRX\",\"301N3295\"],[\"KRX\",\"301N3310\"]]	\
			}";

        while (cmd != "exit") {
            cmd = "";
            cout << "Please input[exit]:";
            std::cin >> cmd;
			if(cmd.compare("exit")==0){
				break;
			}else{

				int num=atoi(cmd.data());
				cout<<num<<endl;
				switch (num)
				{
				case 1:
					 ws->sendFrame(het_str.data(), (int)het_str.size(), WebSocket::FRAME_TEXT);
					 break;
				case 2:
					 ws->sendFrame(con_str.data(), (int)con_str.size(), WebSocket::FRAME_TEXT);
					 break;
				case 3:
					 ws->sendFrame(emr_str.data(), (int)emr_str.size(), WebSocket::FRAME_TEXT);
					 break;
				case 4:
					 ws->sendFrame(mar_str.data(), (int)mar_str.size(), WebSocket::FRAME_TEXT);
					 break;
				case 5:
					 ws->sendFrame(kle_str.data(), (int)kle_str.size(), WebSocket::FRAME_TEXT);
					 break;
				case 6:
					 ws->sendFrame(sub_str.data(), (int)sub_str.size(), WebSocket::FRAME_TEXT);
					 break;
				default:
					break;
				}
			}

			if (ws->poll(Poco::Timespan(0, 0, 3, 0, 0), Poco::Net::WebSocket::SELECT_READ) == false) {
				cout << "timeout" << endl;
				break;
			}
			try{
				n = ws->receiveFrame(buffer,1024*1024, flags);
				if(n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE)
				{
					buffer[n] = '\0';
					cout<<buffer<<endl;
					fout<<buffer<<endl;
				}else{
					cout<<"close the client"<<endl;
					break;
				}
			}catch(Exception &exc)
			{
				std::cerr << exc.displayText()<<endl;
			}
        }
        ws->shutdown();
		fout.close();

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
//DWORD WINAPI handleRequest(LPVOID lpparentet)
//{
//	ofstream fout;
//	fout.open("receive.txt",'w');
//	int n=1;
//	char *buffer=(char*)malloc(sizeof(char)*1024*1024);
//	int flags=0;
//	while(true)
//	{
//		//cout<<"wait the return data"<<endl;
//		if (webs != NULL) {
//			if (webs->poll(Poco::Timespan(0, 0, 3, 0, 0), Poco::Net::WebSocket::SELECT_READ) == false) {
//				cout << "timeout" << endl;
//				break;
//			}
//			try{
//				n = webs->receiveFrame(buffer,1024*1024, flags);
//				if(n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE)
//				{
//					buffer[n] = '\0';
//				//	fout<<buffer<<endl;
//				}else{
//					webs=NULL;
//					cout<<"close the client"<<endl;
//					break;
//				}
//			}catch(Exception &exc)
//			{
//				std::cerr << exc.displayText()<<endl;
//			}
//			//cout << "Received: " << buffer << endl;
//		}
//		else {
//			//cout << "no connections" << endl;
//		}
//	//	Sleep(500);
//	}
//	fout.close();
//	free(buffer);
//	return 0;
//}
















