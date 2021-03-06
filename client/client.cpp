#include "pch.h"
 
 
#define DEFAULT_PORT "10000"
#define DEFAULT_BUFLEN 50

int _stdcall HostThreadRecv(void *arg);
SOCKET GetSocketDescriptor(char *IPAddress, char *PortNum, bool Bind);

int main(int argc, char *argv[])
{
	// Needed Variables for localhost
	HOSTENT *hostentry;
	IN_ADDR addr;
	//Initialize WinSock2.2
	WSADATA wsa;
	WORD Result = WSAStartup(MAKEWORD(2, 2), &wsa);

	//Get Server Info from Command Line
	int s_port;
	char hostaddr[20] = "localhost", hostport[10];
	char username[15];


	switch (argc) {
	case 1:
		memcpy(hostport, DEFAULT_PORT, sizeof(DEFAULT_PORT));
		hostentry = gethostbyname(hostaddr);
		memcpy(&addr, hostentry->h_addr_list[0], sizeof(addr));
		memcpy(hostaddr, inet_ntoa(addr), sizeof(hostaddr));
		std::cout << "IP = " << hostaddr << ", Port = " << hostport << "\n";
		break;
	case 2:
		memcpy(hostaddr, argv[1], sizeof(hostaddr));
		memcpy(hostport, DEFAULT_PORT, sizeof(DEFAULT_PORT));
		std::cout << "IP = " << hostaddr << ", Port = " << hostport << "\n";
		break;
	case 3:
		memcpy(hostaddr, argv[1], sizeof(hostaddr));
		memcpy(hostport, argv[2], sizeof(hostport));
		std::cout << "IP = " << hostaddr << ", Port = " << hostport << "\n";
		break;
	default:
		std::cout << "Error, invalid arguements\n";
		ExitProcess(EXIT_FAILURE);
	}

	//Initialize Socket
	SOCKET ClientSocket = GetSocketDescriptor(hostaddr, hostport, false);
	std::cout << "Input Username: ";
	std::cin.getline(username, sizeof(username));
	send(ClientSocket, username, sizeof(username), 0);
	HANDLE HostThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HostThreadRecv, (void *)ClientSocket, 0, 0);

	//Message Server
	char recvbuf[DEFAULT_BUFLEN], sendbuf[DEFAULT_BUFLEN];
	int ReceiveN, SendN;

	while (true)
	{
		std::cin.getline(sendbuf, sizeof(sendbuf));
		for (int i = 0; i < sizeof(sendbuf); i++) {if (sendbuf[i] == '\n')sendbuf[i] == '\0';}
		SendN = send(ClientSocket, sendbuf, sizeof(sendbuf), 0);
		ZeroMemory(sendbuf, sizeof(sendbuf));
		Sleep(10);
	}
	//Close Socket and Cleanup
	closesocket(ClientSocket);
	WSACleanup();

	ExitProcess(EXIT_SUCCESS);
}

int _stdcall HostThreadRecv(void *arg)
{
	SOCKET hostsocket = (SOCKET)arg;
	char recvbuf[DEFAULT_BUFLEN];
	int iRecv;

	while (true)
	{
		recv(hostsocket, recvbuf, sizeof(recvbuf), 0);
		std::cout << "[HOST] : " << recvbuf << std::endl;
		ZeroMemory(recvbuf, sizeof(recvbuf));
		Sleep(10);
	}

	closesocket(hostsocket);
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
