#include "pch.h"

#define DEFAULT_PORT "10000"
#define DEFAULT_BUFLEN 50


typedef struct _client {
	char username[15];
	SOCKET clientsocket;
	_client *nextclient = nullptr;
	HANDLE ClientThreadId = nullptr;
}_CLIENT;

_CLIENT *ClientTable = nullptr;
_CLIENT *ClientTableHead = nullptr;

SOCKET GetSocketDescriptor(char *IPAddress, char *PortNum, bool Bind);
VOID AddClientToTable();
int _stdcall ClientThreadRecv(void *args);

int main(int argc, char *argv[])
{
	//Winsock Initialization
	WSADATA wsa;
	WORD Result;
	Result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (Result != 0) { std::cout << "Error, could not inilialize WinSock2.2\n"; ExitProcess(EXIT_FAILURE); }
	std::cout << "WinSock2.2 Initialized\n";

	//Get Listening Socket
	SOCKET ListeningSocket = GetSocketDescriptor(nullptr, nullptr, true);

	//Listen for Connection Requests
	listen(ListeningSocket, 3);

	//Accept Request
	SOCKADDR_IN clientaddr;
	SOCKET ClientSocket;
	int clientsize = sizeof(clientaddr);

	//
	_CLIENT *Client;
	bool ClientHead = true;
	while (true)
	{
		ClientSocket = accept(ListeningSocket, (SOCKADDR *)&clientaddr, &clientsize);
		if (ClientSocket == INVALID_SOCKET) { std::cout << "Error, could not accept connection\n"; break; }
		std::cout << "Client Accepted\n";

		//Client Table
		if (ClientHead)
		{
			ClientTable = new _CLIENT();
			ClientTableHead = ClientTable;
			ClientHead = !ClientHead;
		}
		else
		{
			ClientTable = new _CLIENT();
		}
		ClientTable->clientsocket = ClientSocket;
		recv(ClientSocket, ClientTable->username, sizeof(ClientTable->username), 0);

		//Create Client Thread
		//ClientTable->ClientThreadId = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClientThreadRecv, (void *)ClientSocket, 0, 0);
		std::cout << "User: " << ClientTable->username << std::endl;

		ClientTable = ClientTable->nextclient;

		for (Client = ClientTableHead; Client != nullptr; Client = Client->nextclient)
		{
			//send(Client->clientsocket, recvbuf, sizeof(recvbuf), 0);
			std::cout << "\t\t" << Client->username << "\t" << Client->clientsocket << "\t" << Client->nextclient << std::endl;
		}

	}


	//Close Socket
	closesocket(ListeningSocket);
	WSACleanup();

	ExitProcess(EXIT_SUCCESS);
}
int _stdcall ClientThreadRecv(void *args) 
{
	SOCKET ClientSocket = (SOCKET)args;
	int iRecv;
	char recvbuf[50];
	_CLIENT *Client = nullptr;

	while (true)
	{
		iRecv = recv(ClientSocket, recvbuf, sizeof(recvbuf), 0);
		std::cout << "Received : " << recvbuf << std::endl;

		for (Client = ClientTableHead; Client != nullptr;Client = Client->nextclient)
		{
			//send(Client->clientsocket, recvbuf, sizeof(recvbuf), 0);
			std::cout << "\t\t" << Client->username << "\t" << Client->clientsocket << "\t" << Client->nextclient << std::endl;
		}
		ZeroMemory(recvbuf, sizeof(recvbuf));
		Sleep(10);
	}

	closesocket(ClientSocket);
}

SOCKET GetSocketDescriptor(char *IPAddress, char *PortNum, bool Bind)
{
	//TCP Socket
	SOCKET mysocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mysocket == INVALID_SOCKET) { std::cout << "Error, could not create socket\n"; ExitProcess(EXIT_FAILURE); }
	std::cout << "ListeningSocket Created\n";

	//Address Info
	SOCKADDR_IN Address;
	Address.sin_family = AF_INET;
	if (Bind) {
		Address.sin_addr.s_addr = INADDR_ANY;
		Address.sin_port = htons(atoi(DEFAULT_PORT));

		//Bind Server to Socket
		if (bind(mysocket, (SOCKADDR *)&Address, sizeof(Address))) { std::cout << "Error, could not bind to listening socket\n"; closesocket(mysocket); ExitProcess(EXIT_FAILURE); }
		std::cout << "Server bound to ListeningSocket\n";
	}
	else
	{
		Address.sin_addr.s_addr = inet_addr(IPAddress);
		Address.sin_port = htons(atoi(PortNum));

		//Connect to Server
		if (connect(mysocket, (SOCKADDR *)&Address, sizeof(Address))) { std::cout << "Could not connect to server\n"; closesocket(mysocket); ExitProcess(EXIT_FAILURE); }
		std::cout << "Connected to Server\n";
	}


	return mysocket;
}
