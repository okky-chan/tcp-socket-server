#pragma once

#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MAX_BUFFER_SIZE (49152)

//Declaration of class
class CTcpListener;

//Callback data received
typedef void (*MessageReceivedHandler)(CTcpListener* listener, int socketId, std::string msg);

class CTcpListener {

public:

	CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler);
	~CTcpListener(); //Destruct the TCP Listener

	//Send message to specified client
	void Send(int clientSocket, std::string msg);
	
	//Initialize winsock
	bool Init();

	void Run();

	void Cleanup();

private:

	SOCKET CreateSocket();

	SOCKET WaitForConnection(SOCKET listening);

	std::string				m_ipAddress;
	int						m_port;
	MessageReceivedHandler	MessageReceived;
};
