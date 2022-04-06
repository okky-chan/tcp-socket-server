#include "TCPListener.h"

CTcpListener::CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler) : m_ipAddress(ipAddress), m_port(port), MessageReceived(handler) {

}

//Destruct the TCP Listener
CTcpListener::~CTcpListener() {
	Cleanup();
}

//Send message to specified client
void CTcpListener::Send(int clientSocket, std::string msg) {
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

//Initialize winsock
bool CTcpListener::Init() {
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);

	return wsInit == 0;
}

void CTcpListener::Run() {

	char buf[MAX_BUFFER_SIZE];

	while (true) {
		SOCKET listening = CreateSocket();
		if (listening == INVALID_SOCKET) {
			break;
		}

		SOCKET client = WaitForConnection(listening);
		if (client != INVALID_SOCKET) {
			closesocket(listening);

			int bytesReceived = 0;
			do {
				ZeroMemory(buf, MAX_BUFFER_SIZE);
				bytesReceived = recv(client, buf, MAX_BUFFER_SIZE, 0);

				if (bytesReceived > 0) {
					if (MessageReceived != NULL) {
						MessageReceived(this, client, std::string(buf, 0, bytesReceived));
					}
				}
			} while (bytesReceived);
			closesocket(client);
		}
	}
}

void CTcpListener::Cleanup() {
	WSACleanup();

}

SOCKET CTcpListener::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET) {
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOk != SOCKET_ERROR) {
			int listenOk = listen(listening, SOMAXCONN);
			if (listenOk == SOCKET_ERROR) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	return listening;
}

SOCKET CTcpListener::WaitForConnection(SOCKET listening) {
	SOCKET client = accept(listening, NULL, NULL);
	return client;
}
