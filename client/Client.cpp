#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFLEN 512

using std::cout;
using std::endl;
 

 
int main(int argc, char **argv)
{
	

	WSADATA wsaData;
	int iResult;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;
	 
	int recvbuflen = DEFAULT_BUFFLEN;
	const char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFFLEN];

	char connectIp[20] = "localhost";


	cout << "clientApp" << endl;

	if (argc == 2) {
		strcpy_s(connectIp, argv[1]);
	}
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(connectIp, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << "getaddrinfo failed: " << iResult << endl;
		WSACleanup();
		return 2;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			cout << "Error at socket(): " << WSAGetLastError() << endl;
			freeaddrinfo(result);
			WSACleanup();
			return 3;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Unable to connect to server!" << endl;
		WSACleanup();
		return 4;
	}

	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		cout << "send failed: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 5;
	}

	cout << "Bytes sent: " << iResult << endl;

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 6;
	}
	
	cout << "sending" << endl;

	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes received: " << iResult << endl;
		}
		else if (iResult == 0) {
			cout << "Connection closed" << endl;
		}
		else {
			cout << "recv failed: " << WSAGetLastError() << endl;
		}
	} while (iResult > 0);


	closesocket(ConnectSocket);
	WSACleanup();

    return 0;
}

