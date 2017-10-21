#include <iostream>

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Exception.h>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Exception;

using namespace std;

int main() {
    char buffer[1024];
    int flags;
    int n;
    std::string payload;
    
    try {
		cout << "client server" << endl;
        HTTPClientSession cs("127.0.0.1", 8080);
        HTTPRequest request(HTTPRequest::HTTP_GET, "/", "HTTP/1.1");
        HTTPResponse response;
        std::string cmd;
        
        WebSocket * ws = new WebSocket(cs, request, response); // Causes the timeout
        
       /* payload = "SGClient: Hello World!";
        cout << "Send: SGClient: Hello World!" << endl;
        ws->sendFrame(payload.data(), (int)payload.size(), WebSocket::FRAME_TEXT);
        n = ws->receiveFrame(buffer, sizeof(buffer), flags);
        buffer[n] = '\0';
        cout << "Received: " << buffer << endl;
        
        while (cmd != "exit") {
            cmd = "";
            cout << "Please input[exit]:";
            std::cin >> cmd;
            ws->sendFrame(cmd.data(), (int)cmd.size(), WebSocket::FRAME_TEXT);
            n = ws->receiveFrame(buffer, sizeof(buffer), flags);
            buffer[n] = '\0';
            if (n > 0) {
                std::cout << "Receive: " << buffer << std::endl;
            }
        }
        
        ws->shutdown();
   */ } catch (Exception ex) {
		cout << "error client" << endl;
        cout << ex.displayText() << endl;
        cout << ex.what() << endl;
        return -1;
    }
		system("pause");
}
