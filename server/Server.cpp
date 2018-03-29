#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFLEN 512

using std::cout;
using std::endl;

int main() {
	WSADATA wsaData;

	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed : " << iResult << endl;
		return 1;
	}

	struct addrinfo *result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << "getaddrinfo failed: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//т.к. соединены сокет прослушивающий и информация из result, можно очистить память result
	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}


	//сюда попадаем когда клиент подключился
	ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "accept failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	//соединение установлено
	char recvbuf[DEFAULT_BUFFLEN];
	//int iResult,
	int iSendResult;
	int recvbuflen = DEFAULT_BUFFLEN;

	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes received: " << iResult << endl;
			
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			cout << "Bytes sent: " << iSendResult << endl;
		}
		else {
			cout << "recv failed: " << WSAGetLastError()<<endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	//все данные переданы/получены, отключаем сервер
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	closesocket(ClientSocket);
	WSACleanup();


	return 0;
}